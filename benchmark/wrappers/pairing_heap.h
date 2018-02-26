#ifndef _PAIRING_HEAP_H_
#define _PAIRING_HEAP_H_

#include <boost/heap/pairing_heap.hpp>

#include "compare_node.h"

template<class K, class I>
class WrapperBoostPairingHeap {
    boost::heap::pairing_heap<std::pair<K, I>, boost::heap::compare<compare_node<K, I>>> h;

public:
    typedef typename boost::heap::pairing_heap<std::pair<K, I>, boost::heap::compare<compare_node<K, I>>>::handle_type reference;

    reference push(K key, I item) {
        return h.push(std::make_pair(key, item));
    }

    void delete_min() {
        h.pop();
    }

    const I* find_min() {
        return &(h.top().second);
    }

    bool empty() {
        return h.empty();
    }

    reference decrease_key(reference u, K& new_key) {
        (*u).first = new_key;
        h.decrease(u);
        return u;
    }
};

#endif  // _PAIRING_HEAP_H_
