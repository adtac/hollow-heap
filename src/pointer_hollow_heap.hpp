#ifndef _POINTER_HOLLOW_HEAP_H_
#define _POINTER_HOLLOW_HEAP_H_

#include <cstdlib>
#include <vector>
#include <cstring>
#include <queue>

#define DEBUG 0

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) \
    fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
            __FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

#define pointer_hh_node PointerHollowHeapNode<K,I>

template<typename K, typename I>
class PointerHollowHeapNode {
private:
    typedef K key_type;
    typedef I item_type;

public:
    key_type key;
    item_type item;

    pointer_hh_node* children;
    pointer_hh_node* next;
    pointer_hh_node* second_parent;

    bool hollow;

    int rank;

    /**
     * node - constructor
     */
    PointerHollowHeapNode(const key_type& _key, const item_type& _item) {
        key = _key;
        item = _item;

        children = NULL;
        next = NULL;
        second_parent = NULL;

        rank = 0;

        hollow = false;
    }

    inline void add_child(pointer_hh_node* child) {
        child->next = children;
        children = child;
    }
};

template<typename K, typename I>
class PointerHollowHeap {
private:
    typedef K key_type;
    typedef I item_type;

    pointer_hh_node* root;

    pointer_hh_node* link(pointer_hh_node* v, pointer_hh_node* w) {
        DEBUG_PRINT("call to link %p and %p\n", v, w);
        if (v->key < w->key) {
            DEBUG_PRINT("winner: %p\n", v);
            v->add_child(w);
            return v;
        }
        else {
            DEBUG_PRINT("winner: %p\n", w);
            w->add_child(v);
            return w;
        }
    }

    pointer_hh_node** rankmap;
    int rankmap_alloc_size;

    void expand_rankmap(int rank) {
        if (rank >= rankmap_alloc_size) {
            rankmap_alloc_size *= 2;
            rankmap = (pointer_hh_node**) realloc(rankmap, rankmap_alloc_size * sizeof(pointer_hh_node*));
            memset(rankmap+rank, 0, rank*sizeof(pointer_hh_node*));
        }
    }

    std::vector<pointer_hh_node*> to_delete;
    int to_delete_index;

public:
    typedef pointer_hh_node* reference;

    /**
     * PointerHollowHeap - constructor
     */
    PointerHollowHeap() {
        root = NULL;

        rankmap_alloc_size = 32;
        rankmap = (pointer_hh_node**) malloc(rankmap_alloc_size * sizeof(pointer_hh_node**));

        to_delete_index = 0;
        to_delete.resize(32);
    }

    ~PointerHollowHeap() {
        free(rankmap);
    }

    /*
     * find_min - returns the minimum element in the heap
     *
     * Returns a pointer to the item corresponding to the minimum key if the
     * number of elements in the heap is at least 1.
     */
    item_type* find_min() {
        if (root == NULL)
            return NULL;
        
        return &(root->item);
    }

    /**
     * push - pushes a (key, item) pair into the heap
     *
     * @key:  a key object that is comparable
     * @item: the item itself
     *
     * Returns a pointer to a PointerHollowHeapNode<K, I> that's supposed to serve
     * as a reference handle. This will be required for future operations.
     */
    pointer_hh_node* push(const key_type& key, const item_type& item) {
        // Create a new node and link it to the existing DAG.
        pointer_hh_node* new_node = new PointerHollowHeapNode<key_type, item_type>(key, item);
        DEBUG_PRINT("new node %p\n", new_node);

        if (root == NULL) {
            DEBUG_PRINT("since the tree is empty, %p is now root\n", new_node);
            root = new_node;
        }
        else {
            root = link(root, new_node);
        }
        
        return new_node;
    }

    /**
     * decrease_key - decreases the key on a particular node
     *
     * @u:       a pointer to a PointerHollowHeapNode<K, I>
     * @new_key: the new key value
     *
     * Returns a possibly new pointer to the same item (it may be stored in
     * a different node after the decrease_key operation).
     */
    pointer_hh_node* decrease_key(pointer_hh_node* u, const key_type& new_key) {
        // If this the given node is already the root node, decreasing the key
        // will not change the heap. Just set the new key and move on.
        if (u == root) {
            DEBUG_PRINT("given node is the root\n");
            u->key = new_key;
            return u;
        }

        // Otherwise create a new node and move the item.
        pointer_hh_node* v = new PointerHollowHeapNode<key_type, item_type>(new_key, u->item);
        DEBUG_PRINT("created new node %p\n", v);

        v->rank = (u->rank > 1 ? (u->rank-1) : 0);
        DEBUG_PRINT("new node %p's rank is %d\n", v, v->rank);

        u->hollow = true;
        DEBUG_PRINT("%p is now hollow\n", u);

        // If the original root is the winner, the old node (u) gains a second
        // parent as v.
        pointer_hh_node* old_root = root;
        root = link(root, v);
        if (root == old_root) {
            v->children = u;
            u->second_parent = v;
        }
        DEBUG_PRINT("%p->second_parent = %p\n", u, u->second_parent);

        DEBUG_PRINT("new address %p\n", v);
        // Return v so that the caller may update their node table.
        return v;
    }

    bool empty() {
        return root == NULL;
    }

    void delete_min() {
        delete_node(root);
    }

    void delete_node(pointer_hh_node* u) {
        DEBUG_PRINT("call to delete %p\n", u);

        u->hollow = true;
        DEBUG_PRINT("%p is now hollow\n", u);

        if (u != root) {
            // If it's not the root node, we can simply set it to be hollow
            // and move on.
            return;
        }

        DEBUG_PRINT("%p was the root\n", u);

        memset(rankmap, 0, rankmap_alloc_size * sizeof(PointerHollowHeapNode<K, I>*));

        int max_rank = -1;

        DEBUG_PRINT("pushing %p into `to_delete`\n", root);
        to_delete_index = 0;
        to_delete.clear();
        to_delete.push_back(root);

        while (to_delete_index < to_delete.size()) {
            pointer_hh_node* parent = to_delete[to_delete_index];
            DEBUG_PRINT("outer loop: parent = %p\n", parent);

            pointer_hh_node* cur = parent->children;
            DEBUG_PRINT("first child: %p\n", cur);

            pointer_hh_node* prev = NULL;
            pointer_hh_node* next = NULL;
            while (cur != NULL) {
                next = cur->next;
                prev = cur;
                DEBUG_PRINT("prev=%p [cur=%p] next=%p\n", prev, cur, next);

                if (!cur->hollow) {
                    expand_rankmap(cur->rank);

                    while (rankmap[cur->rank] != NULL) {
                        pointer_hh_node* other = rankmap[cur->rank];

                        rankmap[cur->rank] = NULL;

                        cur = link(cur, other);

                        cur->rank++;
                        expand_rankmap(cur->rank);
                    }

                    rankmap[cur->rank] = cur;

                    if (max_rank < cur->rank)
                        max_rank = cur->rank;
                }
                else {
                    if (cur->second_parent == NULL)
                        to_delete.push_back(cur);
                    else {
                        cur->second_parent = NULL;
                        if (cur->second_parent == parent)
                            break;
                        else
                            cur->next = NULL;
                    }
                }

                cur = next;
            }

            delete[] parent;
            to_delete_index++;
        }

        if (max_rank == -1) {
            root = NULL;
            return;
        }

        root = rankmap[max_rank];
        for (int i = max_rank-1; i >= 0; i--) {
            if (rankmap[i] == NULL)
                continue;

            root = link(root, rankmap[i]);
        }

        DEBUG_PRINT("%p is now root\n", root);
    }
};

#endif // _POINTER_HOLLOW_HEAP_H_
