import sys
import matplotlib.pyplot as plt
from collections import defaultdict

m = defaultdict(lambda: 0)
vs = []
tn, td = 0, 0
with open(sys.argv[1]) as f:
    for line in f.readlines():
        ratio = line.split(" ")[-1]
        num, den = [int(x) for x in ratio[1:-2].split("/")]
        if den == 0:
            continue

        r = num/den
        vs.append(r)
        tn += num
        td += den
        m[r] += 1
print(m)
print(len(vs))
print(tn, td, tn/td)

plt.hist(vs, bins=25)
plt.savefig("../graphs/freq.png")
