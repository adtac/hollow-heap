#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

#include <cmath>
#include <vector>
#include <climits>
#include <utility>

#include "graphs.h"

template<class Heap>
long long Benchmark<Heap>::dijkstra(Graph* g) {
    log("running dijkstra\n");

    std::vector<int> in_tree(g->N, 0);
    std::vector<int> dist(g->N, 1e9);
    std::vector<int> parent(g->N, -1);
    std::vector<typename Heap::reference> node_map(g->N);

    in_tree[0] = 1;
    dist[0] = 0;

    log("computing the shortest path tree\n");

    Heap h;
    long long int pre_compute = elapsed();
    node_map[0] = h.push(0, 0);
    for (int i = 1; i < g->N; i++)
        node_map[i] = h.push(1e9, i);

    while (!h.empty()) {
        int u = *h.find_min();
        int d = dist[u];

        h.delete_min();

        for (int i = 0; i < g->vertices[u].out_edges.size(); i++) {
            std::pair<int, int> vw = g->vertices[u].out_edges[i];
            int v = vw.first;
            int w = vw.second;

            if (in_tree[v] == 0 && d+w < dist[v]) {
                dist[v] = d+w;
                in_tree[v] = 1;
                node_map[v] = h.decrease_key(node_map[v], dist[v]);
                parent[v] = u;
            }
        }
    }
    long long int post_compute = elapsed();
    log("elapsed = %lld us\n", post_compute - pre_compute);

    return post_compute - pre_compute;
}

#endif  // _DIJKSTRA_H_
