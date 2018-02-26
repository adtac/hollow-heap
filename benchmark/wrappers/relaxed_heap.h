#ifndef _RELAXED_HEAP_H_
#define _RELAXED_HEAP_H_

#include <boost/pending/relaxed_heap.hpp>

std::vector<std::pair<int, int>> vals;

template<class K, class I>
class relaxed_compare {
public:
    bool operator()(int a, int b) const {
        return vals[a].first < vals[b].first;
    }
};

template<class K, class I>
class WrapperBoostRelaxedHeap {
    boost::relaxed_heap<int, relaxed_compare<K, I>> h;
    int size;

public:
    typedef int reference;

    WrapperBoostRelaxedHeap() : h(1000000) {
        size = 0;
    }

    reference push(K key, I item) {
        vals.resize(size+1);
        vals[size] = std::make_pair(key, item);
        h.push(size);

        size++;
        return size-1;
    }

    void delete_min() {
        h.remove(h.top());
    }

    const I* find_min() {
        return &vals[h.top()].second;
    }

    bool empty() {
        return h.empty();
    }

    reference decrease_key(reference u, K& new_key) {
        vals[u].first = new_key;
        h.update(u);
        return u;
    }
};

#endif  // _RELAXED_HEAP_H_
