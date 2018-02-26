#include <cstdio>
#include <cmath>

#include "graphs.h"
#include "argument.h"
#include "benchmark.h"
#include "../src/hollow_heap.hpp"
#include "../src/unopt_hollow_heap.hpp"
#include "wrappers/wrappers.h"

int main(int argc, char* argv[]) {
    int seed = 0;
    int n = 101;

    if (argc > 1) {
        sscanf(argv[1], "%d", &n);
    }

    printf("n=%d ", n);

    int benchmarks = SORT               |
                     ASSORTED           |
                     DIJKSTRA           |
                     PRIM               |
                     COMPRESSION        |
                     0;

    argument* args = init_args(n, seed);

    Benchmark<UnoptHollowHeap<int, int>>("uhhb").run(benchmarks, args);
    Benchmark<HollowHeap<int, int>>("hhb").run(benchmarks, args);
    Benchmark<WrapperBoostFibonacciHeap<int, int>>("fhb").run(benchmarks, args);
    Benchmark<WrapperBoostPairingHeap<int, int>>("phb").run(benchmarks, args);

    printf("\n");

    return 0;
}
