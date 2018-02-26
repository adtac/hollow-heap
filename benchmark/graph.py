#!/usr/bin/env python3

import os
import glob
from math import log
import numpy as np
import matplotlib.pyplot as plt
import math

from collections import defaultdict


bar_width = 0.2

heaps = {
    "hhb":  ("s", "Hollow Heap (Optimized)", "hhb\\_opt"),
    "uhhb": ("o", "Hollow Heap (Direct)", "hhb\\_dir"),
    "fhb":  ("v", "Fibonacci Heap", "fhb"),
    "phb":  ("^", "Pairing Heap", "phb"),
}

ho = [x for x in heaps]

num_benchmarks = [
    ("sort",),
    ("compression",),
    ("assorted",),
    ("dijkstra_sparse",),
    ("dijkstra_dense",),
    ("prim_sparse",),
    ("prim_dense",),
]

roads = [
    ("nyc",),
    ("bay",),
    ("rome",),
]

plt.figure(figsize=(4, 3))
plt.rc("font", size=6)

def med(l):
    return sorted(l)[int(len(l)/2)]
    return value(l)

def value(l):
    a = np.array([float(x) for x in l])
    return a.prod() ** (1.0/len(a))

def stddev(l):
    a = np.array(l)
    return int(np.std(a, dtype=np.float64))

def process_nums():
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(lambda: list())))

    for filename in glob.glob("benchmark_output_*"):
        if filename.endswith("roads"):
            continue

        with open(filename) as f:
            for line in f.readlines():
                line = line.strip()
                splits = line.split(" ")

                _, n = splits[0].split("=")
                n = int(n)

                i = 1
                while i < len(splits):
                    name, val = splits[i].split("=")
                    name = name.strip()
                    heapname = name.split("_")[0]
                    benchmark = "_".join(name.split("_")[1:])

                    val = int(val.strip())

                    if heapname in heaps:
                        data[benchmark][heapname][n].append(val)

                    i += 1

    count = 0
    for benchmark in num_benchmarks:
        plt.ylabel("Time (in μs)")
        if "_" in benchmark[0]:
            plt.xlabel("|V|")
        else:
            plt.xlabel("N")
        plt.subplots_adjust(left=0.09, right=0.99, top=0.99, bottom=0.13)
        plt.yticks(rotation=90)

        legend = []
        vs = {}
        nss = {}
        for heapname in heaps:
            ns = sorted([int(x) for x in data[benchmark[0]][heapname].keys()])
            vals = [med(data[benchmark[0]][heapname][n]) for n in ns]
            vs[heapname] = vals
            if "_" in benchmark[0]:
                ns = [x/8 for x in ns]
            nss[heapname] = ns
            plt.semilogx(ns, vals, marker=heaps[heapname][0], markersize=3, linewidth=0.75, basex=2)
            legend.append(heaps[heapname][1])

        if benchmark[0] == "assorted":
            continue

        nums = defaultdict(lambda: {})
        for heapname in heaps:
            for n, v in zip(nss[heapname], vs[heapname]):
                nums[n][heapname] = v
        for n in nums:
            prev, end = "\\textbf{", "}"
            nums[n] = {x: prev + ("%.2f" % (nums[n][x]/list(nums[n].values())[0])) + end if nums[n][x] == min(nums[n].values()) else ("%.2f" % (nums[n][x]/list(nums[n].values())[0])) for x in nums[n]}

        count += 1
        if count % 2 == 1:
            print("""\n\\begin{table}[H]
  \\centering
  \\begin{minipage}{.45\\textwidth}""")

        print("""    \\begin{table}[H]
      \\centering
      \\caption{\\texttt{""" + benchmark[0].replace("_", "\\_") + """}}
      \\label{table:app_""" + benchmark[0] + """}

      \\centering
      \\begin{tabular}{|c|c|c|c|c|} 
        \\hline""")
        print("        {} ".format("$|V|$" if "_" in benchmark[0] else "$N$"), end="")
        for heapname in heaps:
            print("& {} ".format("\\texttt{" + heaps[heapname][2] + "}"), end="")
        print(" \\\\\n    \\hline")
        for n in nums:
            print("        {}".format("$2^{" + str(int(log(n, 2))) + "}$"), end=" ")
            for heapname in heaps:
                print("& {} ".format(nums[n][heapname]), end="")
            print(" \\\\\n        \\hline")
        print("""      \\end{tabular}
    \\end{table}""")

        if count % 2 == 1:
            print("""  \\end{minipage}
  \\begin{minipage}{.45\\textwidth}""")
        else:
            print("""  \\end{minipage}
\end{table}""")

        plt.legend(legend)

        plt.savefig("../graphs/{}.png".format(benchmark[0]), dpi=200)
        plt.clf()

def process_roads():
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(lambda: list())))

    with open("benchmark_output_roads") as f:
        for line in f.readlines():
            line = line.strip()
            splits = line.split(" ")

            for split in splits:
                name, val = split.split("=")
                name = name.strip()

                heapname = name.split("_")[0]
                benchmark = name.split("_")[1]
                city = name.split("_")[2]

                val = int(val.strip())
                if heapname in heaps:
                    data[benchmark][city][heapname].append(val)

    for b in data:
        for c in data[b]:
            plt.ylabel("Time (in μs)")
            plt.xlabel("Heap Variant")
            plt.subplots_adjust(left=0.15, right=0.99, top=0.99, bottom=0.13)
            plt.yticks(rotation=45)
            plt.xticks(rotation=10)

            minv = 1e9

            vs = []
            stds = []
            xticks = []

            for h in ho:
                xticks.append(heaps[h][1])

                v = med(data[b][c][h])
                vs.append(v)
                if v < minv:
                    minv = v

                stds.append(stddev(data[b][c][h]))

            print(vs)
            rects = plt.bar(np.arange(len(vs)), vs, 0.35, color="r", yerr=stds)
            plt.xticks(np.arange(len(vs)), tuple(xticks))

            for i in range(len(vs)):
                if vs[i] == minv:
                    rects[i].set_color("b")

            plt.savefig("../graphs/{}_{}.png".format(b, c), dpi=200)
            plt.clf()

if __name__ == "__main__":
    process_nums()
    process_roads()
