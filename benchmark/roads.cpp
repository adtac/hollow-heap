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

    int benchmarks = ROADS;

    argument* args = init_args(0, seed);

    Benchmark<HollowHeap<int, int>>("hhb").run(benchmarks, args);
    Benchmark<UnoptHollowHeap<int, int>>("uhhb").run(benchmarks, args);
    Benchmark<WrapperBoostFibonacciHeap<int, int>>("fhb").run(benchmarks, args);
    Benchmark<WrapperBoostRelaxedHeap<int, int>>("rhb").run(benchmarks, args);
    Benchmark<WrapperBoostPairingHeap<int, int>>("phb").run(benchmarks, args);

    printf("\n");

    return 0;
}
