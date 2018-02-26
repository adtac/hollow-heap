# Hollow Heap

A [hollow heap (2015)](http://www.cs.au.dk/~tdh/papers/Hollow-heaps.pdf) is
a simple data structure that's built with the aim of surpassing Fibonacci heaps
in terms of simplicity and pracitcality.

This is a C++ implementation with a benchmarking tool to compare the practical
performance of hollow heaps with Fibonacci heaps, and pairing heaps by running
different measurements:

### Compiling and running the benchmark

This project uses CMake to make targets. To generate the Makefile for your
system, run:

```bash
$ cmake .
```

This should generate a `Makefile` in the source directory. Then run:

```bash
$ make
```

This will produce a binary `all_tests`. Running the binary with one argument, N,
will produce benchmark results.
