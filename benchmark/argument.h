#ifndef _ARGUMENT_H_
#define _ARGUMENT_H_

#include <cmath>

#include "graphs.h"

typedef struct {
    int N;

    int* sort_ints;

    int* assorted_ints;
    int* assorted_idxs;
    int* assorted_decs;

    Graph* sparse_graph;
    Graph* dense_graph;

    Graph* nyc_graph;
    Graph* bay_graph;

    int* comp_ints;
    int* freq_table;
} argument;

argument* init_args(int N, int seed) {
    argument* a = new argument;

    a->N = N;

    if (N == 0) {
        char* cities[] = {"nyc.input", "bay.input", NULL};
        Graph** g[] = {&a->nyc_graph, &a->bay_graph, NULL};

        int j = 0;
        char* line = (char*) malloc(100);

        while (cities[j] != NULL) {
            FILE* city = fopen(cities[j], "r");
            int cn, cm;
            if (city == NULL) {
                fprintf(stderr, "cannot find %s", cities[j]);
                exit(1);
            }

            while (fgets(line, 100, city)) {
                if (*line == 'p') {
                    sscanf(line, "p sp %d %d", &cn, &cm);
                    break;
                }
            }

            *g[j] = new Graph(cn, cm);

            while (fgets(line, 100, city)) {
                if (*line == 'a') {
                    int f, t, d;
                    sscanf(line, "a %d %d %d", &f, &t, &d);
                    (*g[j])->vertices[f-1].add_edge(t-1, d);
                }
            }

            j++;
        }

        free(line);
    }
    else {
        srand(seed);
        fprintf(stderr, "generating sort_ints...\n");
        a->sort_ints = new int[N];
        for (int i = 0; i < N; i++)
            a->sort_ints[i] = rand() % N;

        srand(seed);
        fprintf(stderr, "generating assorted_ints...\n");
        a->assorted_ints = new int[N];
        for (int i = 0; i < N; i++)
            a->assorted_ints[i] = rand() % N;
        fprintf(stderr, "generating assorted_idxs, assorted_decs...\n");
        a->assorted_idxs = new int[2*N];
        a->assorted_decs = new int[2*N];
        for (int i = 0; i < 2*N; i++) {
            a->assorted_idxs[i] = rand() % N;
            a->assorted_decs[i] = rand()%100 + 1;
        }

        srand(seed);
        int V = N / 8;
        fprintf(stderr, "generating sparse_graph...\n");
        a->sparse_graph = new Graph(V, V * log(V));
        a->sparse_graph->generate_random(seed);
        fprintf(stderr, "generating dense_graph...\n");
        a->dense_graph = new Graph(V, pow(V, 1.75));
        a->dense_graph->generate_random(seed);

        srand(seed);
        int num_elems = N * sqrt(N);
        fprintf(stderr, "generating comp_ints, freq_table...\n");
        a->comp_ints = new int[num_elems];
        a->freq_table = new int[N];
        for (int i = 0; i < N; i++)
            a->freq_table[i] = 0;
        for (int i = 0; i < num_elems; i++) {
            a->comp_ints[i] = rand() % N;
            a->freq_table[a->comp_ints[i]]++;
        }
    }

    return a;
}

#endif  // _ARGUMENT_H_
