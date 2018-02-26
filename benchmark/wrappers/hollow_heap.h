#ifndef _HOLLOW_HEAP_WRAPPER_H_
#define _HOLLOW_HEAP_WRAPPER_H_

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <math.h>

#include "../../src/hollow_heap.hpp"

long perf_event_open(struct perf_event_attr *hw_event,
                     pid_t pid,
                     int cpu,
                     int group_fd,
                     unsigned long flags) {
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                  group_fd, flags);
    return ret;
}

template<class K, class I>
class WrapperHollowHeap {
    HollowHeap<K, I> h;

public:
    typedef typename HollowHeap<K, I>::reference reference;

    int fd;
    std::vector<long long> pushes, find_mins, delete_mins, empties, decrease_keys;

    WrapperHollowHeap() {
        struct perf_event_attr pe;

        memset(&pe, 0, sizeof(struct perf_event_attr));
        pe.type = PERF_TYPE_HARDWARE;
        pe.size = sizeof(struct perf_event_attr);
        pe.config = PERF_COUNT_HW_INSTRUCTIONS;
        pe.disabled = 1;
        pe.exclude_kernel = 1;
        pe.exclude_hv = 1;

        fd = perf_event_open(&pe, 0, -1, -1, 0);
        if (fd == -1) {
            fprintf(stderr, "Error opening leader %llx\n", pe.config);
            exit(EXIT_FAILURE);
        }
    }

    ~WrapperHollowHeap() {
        print();
        close(fd);
    }

    void print() {
        std::vector<long long> *vectors[] = {&pushes, &find_mins, &delete_mins, &empties, &decrease_keys};

        for (int i = 0; i < 5; i++)
            printf("%d ", vectors[i]->size());
        printf("\n");

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < vectors[i]->size(); j++)
                printf("%lld,", (*vectors[i])[j]);
            printf("\n");
        }
    }

    reference push(K key, I item) {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

        reference res = h.push(key, item);

        long long count;
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        read(fd, &count, sizeof(long long));
        pushes.push_back(count);

        return res;
    }

    void delete_min() {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

        h.delete_min();

        long long count;
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        read(fd, &count, sizeof(long long));
        delete_mins.push_back(count);
    }

    const I* find_min() {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

        I* res = h.find_min();

        long long count;
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        read(fd, &count, sizeof(long long));
        find_mins.push_back(count);

        return res;
    }

    bool empty() {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

        bool res = h.empty();

        long long count;
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        read(fd, &count, sizeof(long long));
        empties.push_back(count);

        return res;
    }

    reference decrease_key(reference u, K& new_key) {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

        reference res = h.decrease_key(u, new_key);

        long long count;
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        read(fd, &count, sizeof(long long));
        decrease_keys.push_back(count);

        return res;
    }
};

#endif  // _HOLLOW_HEAP_WRAPPER_H_
