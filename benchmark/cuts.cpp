#include <stdio.h>
#include <math.h>

#include "graphs.h"
#include "argument.h"
#include "benchmark.h"
#include "wrappers/wrappers.h"

int main(int argc, char* argv[]) {
    int seed = 0;
    int n = 9;

    int benchmarks = COMPRESSION;

    argument* args = init_args(n, seed);

    Benchmark<WrapperHollowHeap<int, int>>("hhb").run(benchmarks, args);

    return 0;
}
