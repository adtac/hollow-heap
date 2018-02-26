#ifndef _SORT_H_
#define _SORT_H_

#include <vector>

template<class Heap>
long long int Benchmark<Heap>::sort(int N, int* nums) {
    log("running sort\n");

    std::vector<int> numbers;
    std::vector<int> results;

    log("generating numbers\n");
    for (int i = 0; i < N; i++)
        numbers.push_back(nums[i]);

    log("inserting and retrieving numbers\n");
    long long int pre_insert = elapsed();

    Heap h;
    for (int i = 0; i < N; i++)
        h.push(numbers[i], numbers[i]);

    while (!h.empty()) {
        results.push_back(*h.find_min());
        h.delete_min();
    }

    long long int post_retrieval = elapsed();
    log("elapsed: %lld us\n", post_retrieval - pre_insert);

    log("sorting using STL\n");
    std::sort(numbers.begin(), numbers.end());

    log("verifying\n");
    if (numbers.size() != results.size()) {
        log("incorrect: numbers.size() != results.size() (%d != %d)\n",
            numbers.size(), results.size());
        return -1;
    }
    else {
        for (int i = 0; i < results.size(); i++) {
            if (numbers[i] != results[i]) {
                log("incorrect: numbers[%d] != results[%d] (%d != %d)\n",
                    i, i, numbers[i], results[i]);
                return -1;
            }
        }

        log("correct!\n", heap_name);
        return post_retrieval - pre_insert;
    }
}

#endif  // _SORT_H_
