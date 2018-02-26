#ifndef _GRAPHS_H_
#define _GRAPHS_H_

#include <vector>
#include <utility>

class Node {
public:
    std::vector<std::pair<int, int>> out_edges;
    int label;

    Node(int _label) {
        label = _label;
    }

    void add_edge(int _u, int _w) {
        out_edges.push_back(std::make_pair(_u, _w));
    }
};

class Graph {
public:
    int N;
    long long int M;

    std::vector<Node> vertices;

    Graph(int _N, long long int _M) {
        N = _N;
        M = _M;

        for (int i = 0; i < N; i++) {
            vertices.push_back(Node(i));
        }
    }

    void generate_random(int seed) {
        long long int edges_left = M;
        long long int max_edges = N;
        max_edges = (max_edges * (max_edges-1)) / 2;
        double threshold_p = double(edges_left) / double(max_edges);

        srand(seed);

        for (int i = 0; i < N; i++) {
            for (int j = i+1; j < N; j++) {
                double p = (rand() % 1000000) / 1000000.00;
                if (p < threshold_p) {
                    int w = 1 + (rand()%100);
                    vertices[i].add_edge(j, w);
                    vertices[j].add_edge(i, w);
                }
            }
        }
    }
};

#endif  // _GRAPH_H_
