#ifndef _COMPARE_NODE_H_
#define _COMPARE_NODE_H_

#include <utility>

/**
 * A comparison class that's used uniformly by Boost's heap data structures
 * to define the ordering between two keys of type pair<K, I>.
 */
template<class K, class I>
struct compare_node {
    bool operator()(const std::pair<K, I> n1, const std::pair<K, I> n2) const {
        return n1.first > n2.first;
    }
};

#endif  // _COMPARE_NODE_H_
