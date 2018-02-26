#ifndef _TOGGLE_HOLLOW_HEAP_H_
#define _TOGGLE_HOLLOW_HEAP_H_

#include <cstdlib>
#include <vector>
#include <cstring>
#include <queue>

#include "optimizations.hpp"

#define DEBUG 0

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) \
    fprintf(stderr, "DEBUG: %d:%s(): " fmt, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

#define toggle_hh_node ToggleHollowHeapNode<K,I>

template<typename K, typename I>
struct ToggleHollowHeapNode {
    K key;
    I item;

    unsigned id;
    unsigned children;
    unsigned next;
    unsigned second_parent;

    unsigned rank;
    bool hollow;
};

template<typename K, typename I>
class ToggleHollowHeap {
public:
    typedef unsigned reference;

private:
    typedef K key_type;
    typedef I item_type;

    reference root;

    #if OPT_PREALLOC_RANKMAP == 1
    reference* rankmap;
    int rankmap_alloc_size;

    #if OPT_INLINE_EVERYTHING == 1
    inline
    #endif
    void expand_rankmap(unsigned rank) {
        if (rank >= rankmap_alloc_size) {
            #if OPT_QUAD_MULTIPLIER == 1
            rankmap_alloc_size *= 4;
            #else
            rankmap_alloc_size *= 2;
            #endif
            rankmap = (unsigned*) realloc(rankmap, rankmap_alloc_size * sizeof(unsigned));
            memset(rankmap+rank, 0, rank * sizeof(unsigned));
        }
    }
    #else
    #if OPT_INLINE_EVERYTHING == 1
    inline
    #endif
    void expand_rankmap(reference** rankmap, int* rankmap_alloc_size, unsigned rank) {
        if (rank >= *rankmap_alloc_size) {
            #if OPT_QUAD_MULTIPLIER == 1
            *rankmap_alloc_size *= 4;
            #else
            *rankmap_alloc_size *= 2;
            #endif
            *rankmap = (reference*) realloc(*rankmap, *rankmap_alloc_size * sizeof(reference));
        }
    }
    #endif

    #if OPT_PREALLOC_TO_DELETE == 1
    reference* to_delete;
    int to_delete_index;
    int to_delete_used;
    int to_delete_alloc_size;

    #if OPT_INLINE_EVERYTHING == 1
    inline
    #endif
    void expand_to_delete() {
        if (to_delete_used >= to_delete_alloc_size) {
            #if OPT_QUAD_MULTIPLIER == 1
            to_delete_alloc_size *= 4;
            #else
            to_delete_alloc_size *= 2;
            #endif
            to_delete = (unsigned*) realloc(to_delete, to_delete_alloc_size * sizeof(unsigned));
        }
    }
    #endif

    int nodes_used;
    int nodes_alloc_size;
    toggle_hh_node* nodes;

    #if OPT_INLINE_EVERYTHING == 1
    inline
    #endif
    unsigned link(unsigned v, unsigned w) {
        DEBUG_PRINT("call to link %d and %d\n", v, w);

        if (nodes[v].key < nodes[w].key) {
            DEBUG_PRINT("winner: %d\n", v);
            nodes[w].next = nodes[v].children;
            nodes[v].children = w;
            return v;
        }
        else {
            DEBUG_PRINT("winner: %d\n", w);
            nodes[v].next = nodes[w].children;
            nodes[w].children = v;
            return w;
        }
    }

    #if OPT_INLINE_EVERYTHING == 1
    inline
    #endif
    toggle_hh_node* make_new_node(const key_type& key, const item_type& item) {
        nodes_used++;

        toggle_hh_node* result = nodes+nodes_used;
        result->id = nodes_used;
        result->next = result->children = result->second_parent = 0;
        result->rank = 0;
        result->hollow = 0;
        result->key = key;
        result->item = item;

        if (nodes_used+1 >= nodes_alloc_size) {
            #if OPT_QUAD_MULTIPLIER == 1
            nodes_alloc_size *= 4;
            #else
            nodes_alloc_size *= 2;
            #endif
            nodes = (toggle_hh_node*) realloc(nodes, nodes_alloc_size * sizeof(toggle_hh_node));
        }

        return nodes+nodes_used;
    }

public:
    /**
     * HollowHeap - constructor
     */
    ToggleHollowHeap() {
        root = 0;

        #if OPT_PREALLOC_RANKMAP == 1
        rankmap_alloc_size = 16;
        rankmap = (unsigned*) calloc(rankmap_alloc_size, sizeof(unsigned));
        #endif

        #if OPT_PREALLOC_TO_DELETE == 1
        to_delete_alloc_size = 16;
        to_delete = (unsigned*) malloc(to_delete_alloc_size * sizeof(unsigned));
        #endif

        nodes_used = 0;
        nodes_alloc_size = 1024;
        nodes = (toggle_hh_node*) malloc(nodes_alloc_size * sizeof(toggle_hh_node));
    }

    ~ToggleHollowHeap() {
        #if OPT_PREALLOC_RANKMAP == 1
        free(rankmap);
        #endif

        free(nodes);
    }

    /*
     * find_min - returns the minimum element in the heap
     *
     * Returns a pointer to the item corresponding to the minimum key if the
     * number of elements in the heap is at least 1.
     */
    #if OPT_INLINE_EVERYTHING == 1
    inline
    #endif
    item_type* find_min() {
        if (!root)
            return NULL;
        
        return &nodes[root].item;
    }

    /**
     * push - pushes a (key, item) pair into the heap
     *
     * @key:  a key object that is comparable
     * @item: the item itself
     *
     * Returns a pointer to a HollowHeapNode<K, I> that's supposed to serve
     * as a reference handle. This will be required for future operations.
     */
    #if OPT_INLINE_EVERYTHING == 1
    inline
    #endif
    reference push(const key_type& key, const item_type& item) {
        DEBUG_PRINT("push %d\n", key);

        // Create a new node and link it to the existing DAG.
        toggle_hh_node* new_node = make_new_node(key, item);
        DEBUG_PRINT("new node %p\n", new_node);

        if (!root)
            root = new_node->id;
        else
            root = link(root, nodes_used);
        
        return nodes_used;
    }

    /**
     * decrease_key - decreases the key on a particular node
     *
     * @u:       a pointer to a HollowHeapNode<K, I>
     * @new_key: the new key value
     *
     * Returns a possibly new pointer to the same item (it may be stored in
     * a different node after the decrease_key operation).
     */
    #if OPT_INLINE_EVERYTHING == 1
    inline
    #endif
    reference decrease_key(reference u, const key_type& new_key) {
        // If this the given node is already the root node, decreasing the key
        // will not change the heap. Just set the new key and move on.
        if (nodes[u].id == root) {
            DEBUG_PRINT("given node is the root\n");
            nodes[u].key = new_key;
            return u;
        }

        // Otherwise create a new node and move the item.
        make_new_node(new_key, nodes[u].item);

        #if OPT_DEC_KEY_RANK_DEC == 0
        nodes[nodes_used].rank = nodes[u].rank;
        #else
        if (nodes[u].rank > OPT_DEC_KEY_RANK_DEC)
            nodes[nodes_used].rank = nodes[u].rank - OPT_DEC_KEY_RANK_DEC;
        else
            nodes[nodes_used].rank = 0;
        #endif


        nodes[u].hollow = 1;

        // If the original root is the winner, the old node gains a second
        // parent in the form of the new node.
        unsigned old_root = root;
        root = link(root, nodes_used);
        if (root == old_root) {
            nodes[nodes_used].children = nodes[u].id;
            nodes[u].second_parent = nodes_used;
        }

        // Return v so that the caller may update their node table.
        return nodes_used;
    }

    #if OPT_INLINE_EVERYTHING == 1
    inline
    #endif
    bool empty() {
        return !root;
    }

    void delete_min() {
        if (!root)
            return;

        int max_rank = -1;

        DEBUG_PRINT("pushing %d into `to_delete`\n", root);

        #if OPT_PREALLOC_TO_DELETE == 1
        to_delete_index = 0;
        to_delete_used = 1;
        to_delete[0] = root;
        #else
        std::queue<reference> to_delete;
        to_delete.push(root);
        #endif

        #if OPT_ARRAY_OVER_MAP_RANKMAP == 1
        #if OPT_PREALLOC_RANKMAP == 1
        #if OPT_ZERO_RANKMAP_INLINE == 0
        memset(rankmap, 0, sizeof(unsigned) * rankmap_alloc_size);
        #endif
        #else
        int rankmap_alloc_size = 32;
        reference* rankmap = (reference*) calloc(rankmap_alloc_size, sizeof(reference));
        #endif
        #else
        std::map<unsigned, reference> rankmap;
        #endif

        #if OPT_PREALLOC_TO_DELETE == 1
        while (to_delete_index < to_delete_used) {
            toggle_hh_node* parent = nodes+to_delete[to_delete_index];
        #else
        while (!to_delete.empty()) {
            toggle_hh_node* parent = nodes+to_delete.front();
        #endif

            parent->next = 0;
            DEBUG_PRINT("outer loop: parent = %d\n", parent->id);

            toggle_hh_node* cur = NULL;
            if (parent->children)
                cur = nodes+parent->children;
            DEBUG_PRINT("first child: %p\n", cur);

            toggle_hh_node* next;
            while (cur != NULL) {
                next = NULL;
                if (cur->next)
                    next = nodes+cur->next;
                cur->next = 0;

                DEBUG_PRINT("[cur=%d] next=%p\n", cur->id, next);

                if (cur->hollow == 0) {
                    #if OPT_ARRAY_OVER_MAP_RANKMAP == 1
                    while (max_rank >= 0 && cur->rank <= max_rank && rankmap[cur->rank]) {
                    #else
                    while (max_rank >= 0 && cur->rank <= max_rank && rankmap.find(cur->rank) != rankmap.end()) {
                    #endif
                        toggle_hh_node* other = nodes+rankmap[cur->rank];

                        #if OPT_ARRAY_OVER_MAP_RANKMAP == 1
                        rankmap[cur->rank] = 0;
                        #else
                        rankmap.erase(rankmap.find(cur->rank));
                        #endif

                        DEBUG_PRINT("cur=%p other=%p\n", cur, other);
                        cur = nodes+link(cur->id, other->id);

                        ++(cur->rank);
                    }

                    #if OPT_ARRAY_OVER_MAP_RANKMAP == 1
                    #if OPT_PREALLOC_RANKMAP == 1
                    expand_rankmap(cur->rank);
                    #else
                    expand_rankmap(&rankmap, &rankmap_alloc_size, cur->rank);
                    #endif
                    #endif

                    rankmap[cur->rank] = cur->id;

                    if (max_rank < 0 || max_rank < cur->rank)
                        max_rank = cur->rank;
                }
                else {
                    if (!cur->second_parent) {
                        #if OPT_PREALLOC_TO_DELETE == 1
                        to_delete[to_delete_used++] = cur->id;
                        expand_to_delete();
                        #else
                        to_delete.push(cur->id);
                        #endif
                    }
                    else {
                        cur->second_parent = 0;
                        if (cur->second_parent == parent->id)
                            break;
                        else
                            cur->next = 0;
                    }
                }

                cur = next;
            }

            #if OPT_PREALLOC_TO_DELETE == 1
            ++to_delete_index;
            #else
            to_delete.pop();
            #endif
        }

        if (max_rank < 0) {
            root = 0;
            goto end;
        }

        #if OPT_ARRAY_OVER_MAP_RANKMAP == 1
        #if OPT_REV_RANKMAP_ITER == 1
        root = rankmap[max_rank];

        #if OPT_ZERO_RANKMAP_INLINE == 1
        rankmap[max_rank] = 0;
        #endif

        for (int i = max_rank-1; i >= 0; --i) {
            if (rankmap[i]) {
                root = link(root, rankmap[i]);

                #if OPT_ZERO_RANKMAP_INLINE == 1
                rankmap[i] = 0;
                #endif
            }
        }
        #else
        root = 0;
        for (int i = 0; i <= max_rank; i++) {
            if (rankmap[i]) {
                if (!root)
                    root = rankmap[i];
                else
                    root = link(root, rankmap[i]);

                #if OPT_ZERO_RANKMAP_INLINE == 1
                rankmap[i] = 0;
                #endif
            }
        }
        #endif
        #else
        root = 0;
        for (std::map<unsigned, reference>::iterator it = rankmap.begin(); it != rankmap.end(); it++) {
            if (root == 0)
                root = it->second;
            else
                root = link(root, it->second);
        }
        #endif

end:
        #if OPT_PREALLOC_RANKMAP == 0 && OPT_ARRAY_OVER_MAP_RANKMAP == 1
        free(rankmap);
        #endif

        DEBUG_PRINT("%d is now root\n", root);
    }
};

#endif // _HOLLOW_HEAP_H_
