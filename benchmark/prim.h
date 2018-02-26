#ifndef _PRIM_H_
#define _PRIM_H_

#include <cmath>
#include <vector>
#include <climits>
#include <utility>

#include "graphs.h"

template<class Heap>
long long Benchmark<Heap>::prim(Graph* g) {
    log("running prim\n");

    std::vector<int> in_mst(g->N, 0);
    std::vector<int> weight_in(g->N, -1);
    std::vector<int> d(g->N, 1e9);
    std::vector<typename Heap::reference> node_map(g->N);

    long long int total_weight = 0;

    d[0] = 0;

    log("computing the shortest path tree\n");
    long long int pre_compute = elapsed();

    Heap h;
    node_map[0] = h.push(0, 0);
    weight_in[0] = 0;
    for (int i = 1; i < g->N; i++)
        node_map[i] = h.push(INT_MAX, i);

    while (!h.empty()) {
        int u = *h.find_min();
        if (weight_in[u] == -1)
            break;

        in_mst[u] = 1;
        total_weight += weight_in[u];

        h.delete_min();

        for (int i = 0; i < g->vertices[u].out_edges.size(); i++) {
            std::pair<int, int> vw = g->vertices[u].out_edges[i];
            int v = vw.first;
            int w = vw.second;

            if (in_mst[v] == 0 && w < d[v]) {
                d[v] = w;
                node_map[v] = h.decrease_key(node_map[v], d[v]);
                weight_in[v] = w;
            }
        }
    }
    long long int post_compute = elapsed();
    log("elapsed = %lld us\n", post_compute - pre_compute);

    log("computed MST weight = %lld\n", total_weight);

    return post_compute - pre_compute;
}

#endif  // _PRIM_H_
