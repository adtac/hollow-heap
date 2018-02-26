#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include <cmath>
#include <utility>
#include <vector>

typedef struct {
    int freq;
    int val;
    int left;
    int right;
} huffman_node;

template<class Heap>
long long Benchmark<Heap>::compression(int N, int* freq_table) {
    log("running compression\n");

    log("computing the Huffman tree\n");
    long long int pre_compute = elapsed();

    Heap h;
    std::vector<huffman_node> nodes;
    int heap_size = 0;
    for (int i = 0; i < N; i++) {
        if (freq_table[i] > 0) {
            nodes.push_back(huffman_node {freq_table[i], i, -1, -1});
            h.push(freq_table[i], nodes.size()-1);
            heap_size++;
        }
    }

    while (heap_size > 1) {
        int l1 = *h.find_min();
        h.delete_min();

        int l2 = *h.find_min();
        h.delete_min();

        nodes.push_back(huffman_node {nodes[l1].freq+nodes[l2].freq, -1, l1, l2});
        h.push(nodes[l1].freq+nodes[l2].freq, nodes.size()-1);
        heap_size--;
    }

    long long int post_compute = elapsed();
    log("elapsed = %lld us\n", post_compute - pre_compute);

    return post_compute - pre_compute;
}

#endif
