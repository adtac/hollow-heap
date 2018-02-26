import matplotlib.pyplot as plt
import numpy as np

measures = [
    "sort",
    "assorted",
    "dijkstra_sparse",
    "dijkstra_dense",
    "prim_sparse",
    "prim_dense",
    "compression",
]

n = len(measures)

slowLinks = [
   (1784629, 2502135),
   (1801300, 2775390),
   (216397, 318468),
   (217828, 321684),
   (118468, 224346),
   (18950, 121984),
   (3754504, 5041263),
]

fastLinks = [
   (1883068, 2977441),
   (1862456, 3213463),
   (141154, 247206),
   (140110, 244589),
   (88274, 189119),
   (18996, 121133),
   (2075618, 3501989),
]

print("slow:")
for i in range(n):
    print("    {}: {:.2f}%".format(measures[i], 100*slowLinks[i][0]/slowLinks[i][1]))

print("fast:")
for i in range(n):
    print("    {}: {:.2f}%".format(measures[i], 100*fastLinks[i][0]/fastLinks[i][1]))

plt.ylabel("Number of ranked links")
plt.xlabel("Benchmark")

ind = np.arange(n)
width = 0.2

fig, ax = plt.subplots()
fig.autofmt_xdate(bottom=0.2, rotation=30, ha='right')
r1 = ax.bar(ind, [x[0] for x in slowLinks], width, color='r')
r2 = ax.bar(ind+width, [x[0] for x in fastLinks], width, color='b')
ax.legend((r1[0], r2[0]), ("Original data structure", "Optimized version"))
ax.set_xticks(ind + width/2)
ax.set_xticklabels(tuple(measures))

plt.savefig("../graphs/ranked.png", dpi=200)
plt.clf()

plt.ylabel("Total number of links (ranked and unranked)")
plt.xlabel("Benchmark")

fig, ax = plt.subplots()
fig.autofmt_xdate(bottom=0.2, rotation=30, ha='right')
r1 = ax.bar(ind, [x[1] for x in slowLinks], width, color='r')
r2 = ax.bar(ind+width, [x[1] for x in fastLinks], width, color='b')
ax.legend((r1[0], r2[0]), ("Original data structure", "Optimized version"))
ax.set_xticks(ind + width/2)
ax.set_xticklabels(tuple(measures))

plt.savefig("../graphs/total.png", dpi=200)
plt.clf()
