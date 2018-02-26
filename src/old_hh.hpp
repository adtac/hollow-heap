#ifndef _OLD_HOLLOW_HEAP_H_
#define _OLD_HOLLOW_HEAP_H_

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

#define old_node OldHollowHeapNode<K, I>

int max(int a, int b) {
    return a > b ? a : b;
}

template<typename K, typename I>
class OldHollowHeapNode {
private:
    typedef K key_type;
    typedef I item_type;

public:
    key_type key;
    item_type item;

    old_node* children;
    old_node* next;
    old_node* second_parent;

    bool hollow;

    int rank;

    /**
     * node - constructor
     */
    OldHollowHeapNode(const key_type& _key, const item_type& _item) {
        key = _key;
        item = _item;

        children = NULL;
        next = NULL;
        second_parent = NULL;

        rank = 0;

        hollow = false;
    }

    void add_child(old_node* child) {
        child->next = children;
        children = child;
    }
};

template<typename K, typename I>
class OldHollowHeap {
private:
    typedef K key_type;
    typedef I item_type;

    int size;

    old_node* root;

    old_node* link(old_node* v, old_node* w) {
        DEBUG_PRINT("call to link %x and %x\n", v, w);
        if (v->key < w->key) {
            DEBUG_PRINT("winner: %x\n", v);
            v->add_child(w);
            return v;
        }
        else {
            DEBUG_PRINT("winner: %x\n", w);
            w->add_child(v);
            return w;
        }
    }

    void expand_rankmap(OldHollowHeapNode<K, I>*** rankmap, int& rankmap_alloc_size, int rank) {
        while (rank > rankmap_alloc_size) {
            rankmap_alloc_size *= 2;
            *rankmap = (old_node**) realloc(*rankmap, rankmap_alloc_size * sizeof(old_node*));
        }
    }

public:
    typedef old_node* reference;

    /**
     * OldHollowHeap - constructor
     */
    OldHollowHeap() {
        size = 0;
        
        root = NULL;
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
     * Returns a pointer to a OldHollowHeapNode<K, I> that's supposed to serve
     * as a reference handle. This will be required for future operations.
     */
    old_node* push(const key_type& key, const item_type& item) {
        DEBUG_PRINT("call to insert (key=%d, item=%d)\n", key, item);
        // Increase the count of number of items in the heap.
        size++;
        DEBUG_PRINT("size goes up to %d\n", size);

        // Create a new node and link it to the existing DAG.
        old_node* new_node = new OldHollowHeapNode<key_type, item_type>(key, item);
        DEBUG_PRINT("new node %x\n", new_node);

        if (root == NULL) {
            DEBUG_PRINT("since the tree is empty, %x is now root\n", new_node);
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
     * @u:       a pointer to a OldHollowHeapNode<K, I>
     * @new_key: the new key value
     *
     * Returns a possibly new pointer to the same item (it may be stored in
     * a different node after the decrease_key operation).
     */
    old_node* decrease_key(old_node* u, const key_type& new_key) {
        // This is an illegal operation; the new key MUST be lower than
        // the existing key value.
        if (new_key >= u->key) {
            DEBUG_PRINT("illegal operation; new_key (%d) >= u->key (%d)\n", new_key, u->key);
            return u;
        }

        DEBUG_PRINT("request to decrease key of %x from %d to %d\n", u, u->key, new_key);

        // If this the given node is already the root node, decreasing the key
        // will not change the heap. Just set the new key and move on.
        if (u == root) {
            DEBUG_PRINT("given node is the root\n");
            u->key = new_key;
            return u;
        }

        // Otherwise create a new node and move the item.
        old_node* v = new OldHollowHeapNode<key_type, item_type>(new_key, u->item);
        DEBUG_PRINT("created new node %x\n", v);

        v->rank = (u->rank >= 2 ? (u->rank-2) : 0);
        DEBUG_PRINT("new node %x's rank is %d\n", v, v->rank);

        u->hollow = true;
        DEBUG_PRINT("%x is now hollow\n", u);

        // If the original root is the winner, the old node (u) gains a second
        // parent as v.
        old_node* old_root = root;
        root = link(root, v);  // Using a ranked link is supposed to
                               // guarantee efficiency, but the paper
                               // recommends using an unranked link
                               // here.
        if (root == old_root) {
            v->children = u;
            u->second_parent = v;
        }

        DEBUG_PRINT("new address %x\n", v);
        // Return v so that the caller may update their node table.
        return v;
    }

    bool empty() {
        return size == 0;
    }

    void delete_min() {
        DEBUG_PRINT("call to delete_min (root = %x)\n", root);

        if (root == NULL)
            return;

        delete_node(root);
    }

    void delete_node(old_node* u) {
        DEBUG_PRINT("call to delete %x\n", u);

        // Decrease the count of number of items in the heap.
        size--;
        DEBUG_PRINT("size goes down to %d\n", size);

        u->hollow = true;
        DEBUG_PRINT("%x is now hollow\n", u);

        if (u != root) {
            // If it's not the root node, we can simply set it to be hollow
            // and move on.
            return;
        }
        else {
            DEBUG_PRINT("%x was the root\n", u);
        }

        std::queue<old_node*> to_delete;

        int rankmap_alloc_size = 32;
        old_node** rankmap = new old_node*[rankmap_alloc_size];
        memset(rankmap, 0, rankmap_alloc_size * sizeof(OldHollowHeapNode<K, I>*));

        int max_rank = -1;

        DEBUG_PRINT("pushing %x into `to_delete`\n", root);
        to_delete.push(root);

        while (!to_delete.empty()) {
            old_node* parent = to_delete.front();
            DEBUG_PRINT("outer loop: parent = %x\n", parent);

            old_node* cur = parent->children;
            DEBUG_PRINT("first child: %x\n", cur);

            old_node* prev = NULL;
            old_node* next = NULL;
            while (cur != NULL) {
                next = cur->next;
                prev = cur;
                DEBUG_PRINT("prev=%x [cur=%x] next=%x\n", prev, cur, next);

                if (cur->hollow)
                    DEBUG_PRINT("%x is hollow\n", cur);
                else
                    DEBUG_PRINT("%x is not hollow\n", cur);

                DEBUG_PRINT("%x->second_parent = %x\n", cur, cur->second_parent);

                if (cur->hollow && cur->second_parent == NULL) {
                    to_delete.push(cur);
                }
                else if (cur->hollow && cur->second_parent == to_delete.front()) {
                    cur->second_parent = NULL;
                    break;
                }
                else if (cur->hollow && cur->second_parent != to_delete.front()) {
                    cur->second_parent = NULL;
                    cur->next = NULL;
                }
                else if (!cur->hollow) {
                    expand_rankmap(&rankmap, rankmap_alloc_size, cur->rank);

                    if (rankmap[cur->rank] == NULL) {
                        rankmap[cur->rank] = cur;

                        if (max_rank < cur->rank)
                            max_rank = cur->rank;

                        goto end;
                    }

                    while (rankmap[cur->rank] != NULL) {
                        expand_rankmap(&rankmap, rankmap_alloc_size, cur->rank);
                        old_node* other = rankmap[cur->rank];

                        rankmap[cur->rank] = NULL;

                        cur = link(cur, other);

                        cur->rank++;
                    }

                    expand_rankmap(&rankmap, rankmap_alloc_size, cur->rank);
                    rankmap[cur->rank] = cur;

                    if (max_rank < cur->rank)
                        max_rank = cur->rank;
                }

                end:
                cur = next;
            }

            delete[] parent;
            to_delete.pop();
        }

        root = NULL;
        for (int i = 0; i <= max_rank; i++) {
            if (rankmap[i] == NULL)
                continue;

            if (root == NULL) {
                root = rankmap[i];
                continue;
            }

            root = link(root, rankmap[i]);
        }

        delete[] rankmap;

        DEBUG_PRINT("%x is now root\n", root);
    }
};

#endif // _HOLLOW_HEAP_H_
