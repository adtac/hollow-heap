#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <map>
#include <utility>

#include "graphs.h"
#include "argument.h"

#define SORT               0x1
#define ASSORTED           0x2
#define DIJKSTRA           0x4
#define PRIM               0x8
#define COMPRESSION        0x10
#define ROADS              0x20

template<class Heap>
class Benchmark {
    std::chrono::high_resolution_clock::time_point start;
    char* heap_name;

public:
    Benchmark(char* _heap_name) {
        heap_name = _heap_name;

        start = std::chrono::high_resolution_clock::now();
    }

    long long int elapsed() {
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }

    void log(const char *format, ...) {
        va_list args;
        va_start(args, format);

        fprintf(stderr, "[%9lld] [%-5s] ", elapsed(), heap_name);
        vfprintf(stderr, format, args);

        va_end(args);
    }

    long long sort(int, int*);

    long long assorted(int, int*, int*, int*);

    long long dijkstra(Graph*);

    long long prim(Graph*);

    long long compression(int, int*);

    void run(int benchmarks, argument* args) {
        if (benchmarks & SORT) {
            printf("%s_sort=%lld ", heap_name, sort(args->N, args->sort_ints));
        }

        if (benchmarks & ASSORTED)
            printf("%s_assorted=%lld ", heap_name, assorted(args->N, args->assorted_ints, args->assorted_idxs, args->assorted_decs));

        if (benchmarks & DIJKSTRA) {
            printf("%s_dijkstra_sparse=%lld ", heap_name, dijkstra(args->sparse_graph));
            printf("%s_dijkstra_dense=%lld ", heap_name, dijkstra(args->dense_graph));
        }

        if (benchmarks & PRIM) {
            printf("%s_prim_sparse=%lld ", heap_name, prim(args->sparse_graph));
            printf("%s_prim_dense=%lld ", heap_name, prim(args->dense_graph));
        }

        if (benchmarks & COMPRESSION) {
            printf("%s_compression=%lld ", heap_name, compression(args->N, args->freq_table));
        }

        if (benchmarks & ROADS) {
            printf("%s_dijkstra_nyc=%lld ", heap_name, dijkstra(args->nyc_graph));
            printf("%s_prim_nyc=%lld ", heap_name, prim(args->nyc_graph));
            printf("%s_dijkstra_bay=%lld ", heap_name, dijkstra(args->bay_graph));
            printf("%s_prim_bay=%lld ", heap_name, prim(args->bay_graph));
        }
    }
};

#include "sort.h"
#include "assorted.h"
#include "dijkstra.h"
#include "prim.h"
#include "compression.h"

#endif  // _BENCHMARK_H_
