#ifndef _UNOPT_HOLLOW_HEAP_H_
#define _UNOPT_HOLLOW_HEAP_H_

#include <cstdlib>
#include <vector>
#include <cstring>
#include <queue>

#define DEBUG 0

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) \
    fprintf(stderr, "DEBUG: %d:%s(): " fmt, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

#define ohh_node UnoptHollowHeapNode<K,I>

template<typename K, typename I>
struct UnoptHollowHeapNode {
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
class UnoptHollowHeap {
private:
    typedef K key_type;
    typedef I item_type;

    unsigned root;

    unsigned* rankmap;
    int rankmap_alloc_size;

    inline void expand_rankmap(unsigned rank) {
        if (rank >= rankmap_alloc_size) {
            rankmap_alloc_size *= 2;
            rankmap = (unsigned*) realloc(rankmap, rankmap_alloc_size * sizeof(unsigned));
            memset(rankmap+rank, 0, rank * sizeof(unsigned));
        }
    }

    unsigned* to_delete;
    int to_delete_index;
    int to_delete_used;
    int to_delete_alloc_size;

    inline void expand_to_delete() {
        if (to_delete_used >= to_delete_alloc_size) {
            to_delete_alloc_size *= 2;
            to_delete = (unsigned*) realloc(to_delete, to_delete_alloc_size * sizeof(unsigned));
        }
    }

    int nodes_used;
    int nodes_alloc_size;
    ohh_node* nodes;

    unsigned link(unsigned v, unsigned w) {
        DEBUG_PRINT("call to link %d(%d) and %d(%d)\n", v, nodes[v].key, w, nodes[w].key);
        bool choose_v = nodes[v].key < nodes[w].key;

        if (choose_v) {
            DEBUG_PRINT("winner: %d(%d)\n", v, nodes[v].key);
            nodes[w].next = nodes[v].children;
            nodes[v].children = w;
            return v;
        }
        else {
            DEBUG_PRINT("winner: %d(%d)\n", w, nodes[w].key);
            nodes[v].next = nodes[w].children;
            nodes[w].children = v;
            return w;
        }
    }

public:
    typedef unsigned reference;

    /**
     * UnoptHollowHeap - constructor
     */
    UnoptHollowHeap() {
        root = 0;

        rankmap_alloc_size = 16;
        rankmap = (unsigned*) calloc(rankmap_alloc_size, sizeof(unsigned));

        to_delete_alloc_size = 32;
        to_delete = (unsigned*) malloc(to_delete_alloc_size * sizeof(unsigned));

        nodes_used = 0;
        nodes_alloc_size = 1024;
        nodes = (ohh_node*) malloc(nodes_alloc_size * sizeof(ohh_node));
    }

    ~UnoptHollowHeap() {
        free(rankmap);
        free(nodes);
    }

    inline ohh_node* make_new_node(const key_type& key, const item_type& item) {
        nodes_used++;

        ohh_node* result = nodes+nodes_used;
        result->id = nodes_used;
        result->next = result->children = result->second_parent = 0;
        result->rank = 0;
        result->hollow = 0;
        result->key = key;
        result->item = item;

        if (nodes_used+1 >= nodes_alloc_size) {
            nodes_alloc_size *= 2;
            nodes = (ohh_node*) realloc(nodes, (nodes_alloc_size) * sizeof(ohh_node));
        }

        return nodes+nodes_used;
    }

    /*
     * find_min - returns the minimum element in the heap
     *
     * Returns a pointer to the item corresponding to the minimum key if the
     * number of elements in the heap is at least 1.
     */
    inline item_type* find_min() {
        if (!root)
            return NULL;
        
        return &((nodes+root)->item);
    }

    /**
     * push - pushes a (key, item) pair into the heap
     *
     * @key:  a key object that is comparable
     * @item: the item itself
     *
     * Returns a pointer to a UnoptHollowHeapNode<K, I> that's supposed to serve
     * as a reference handle. This will be required for future operations.
     */
    reference push(const key_type& key, const item_type& item) {
        DEBUG_PRINT("push %d\n", key);

        // Create a new node and link it to the existing DAG.
        ohh_node* new_node = make_new_node(key, item);
        DEBUG_PRINT("new node %p(%d)\n", new_node, key);

        if (!root)
            root = new_node->id;
        else
            root = link(root, nodes_used);
        
        return nodes_used;
    }

    /**
     * decrease_key - decreases the key on a particular node
     *
     * @u:       a pointer to a UnoptHollowHeapNode<K, I>
     * @new_key: the new key value
     *
     * Returns a possibly new pointer to the same item (it may be stored in
     * a different node after the decrease_key operation).
     */
    reference decrease_key(reference u, const key_type& new_key) {
        DEBUG_PRINT("decreasing %d: %d->%d\n", u, nodes[u].key, new_key);

        // If this the given node is already the root node, decreasing the key
        // will not change the heap. Just set the new key and move on.
        if (nodes[u].id == root) {
            DEBUG_PRINT("given node is the root\n");
            nodes[u].key = new_key;
            return u;
        }

        // Otherwise create a new node and move the item.
        make_new_node(new_key, nodes[u].item);

        if (nodes[u].rank > 2)
            nodes[nodes_used].rank = nodes[u].rank - 2;
        else
            nodes[nodes_used].rank = 0;

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

    bool empty() {
        return !root;
    }

    void print(unsigned index, int level=0) {
        if (level == 0)
            printf("\n");

        for (int i = 0; i < level-1; i++)
            printf("|  ");
        if (level > 0)
            printf("+--");
        printf("%d.%d.%d\n", index, nodes[index].key, nodes[index].rank);

        unsigned next = nodes[index].children;
        if (next == 0)
            return;

        ohh_node* child = nodes+next;
        while (child != NULL) {
            next = child->next;

            print(child->id, level+1);

            if (next == 0)
                child = NULL;
            else
                child = nodes+next;
        }
    }

    void print_root() {
        print(root, 0);
    }

    void delete_min() {
        if (!root)
            return;

        int max_rank = -1;

        DEBUG_PRINT("pushing %d(%d) into `to_delete`\n", root, nodes[root].key);
        to_delete_index = 0;
        to_delete_used = 0;
        to_delete[to_delete_used++] = root;
        expand_to_delete();

        while (to_delete_index < to_delete_used) {
            ohh_node* parent = nodes+to_delete[to_delete_index];
            DEBUG_PRINT("outer loop: parent = %p(%d)\n", parent, parent->key);

            ohh_node* cur = NULL;
            if (parent->children)
                cur = nodes+parent->children;
            DEBUG_PRINT("first child: %p(%d)\n", cur, cur == NULL ? -1 : cur->key);

            ohh_node* next;
            while (cur != NULL) {
                next = NULL;
                if (cur->next)
                    next = nodes+cur->next;

                DEBUG_PRINT("[cur=%p(%d)] next=%p(%d)\n", cur, cur->key, next, next == NULL ? -1 : next->key);

                if (cur->hollow == 0) {
                    while (cur->rank < rankmap_alloc_size && rankmap[cur->rank]) {
                        ohh_node* other = nodes+rankmap[cur->rank];

                        rankmap[cur->rank] = 0;

                        DEBUG_PRINT("cur=%p(%d) other=%p(%d)\n", cur, cur->key, other, other->key);
                        cur = nodes+link(cur->id, other->id);

                        (cur->rank)++;
                    }

                    expand_rankmap(cur->rank);
                    rankmap[cur->rank] = cur->id;

                    if (max_rank < 0 || max_rank < cur->rank)
                        max_rank = cur->rank;
                }
                else {
                    if (!cur->second_parent) {
                        to_delete[to_delete_used++] = cur->id;
                        expand_to_delete();
                    }
                    else {
                        if (cur->second_parent == parent->id) {
                            cur->second_parent = 0;
                            break;
                        }
                        else {
                            cur->second_parent = 0;
                            cur->next = 0;
                        }
                    }
                }

                cur = next;
            }

            to_delete_index++;
        }

        if (max_rank < 0) {
            root = 0;
            return;
        }

        root = 0;
        for (int i = 0; i <= max_rank; i++) {
            if (rankmap[i]) {
                if (root)
                    root = link(root, rankmap[i]);
                else
                    root = rankmap[i];
                rankmap[i] = 0;
            }
        }

        DEBUG_PRINT("%d(%d) is now root\n", root, nodes[root].key);
    }
};

#endif // _UNOPT_HOLLOW_HEAP_H_
