#!/usr/bin/env python3

import matplotlib.pyplot as plt

measures = ["push", "find_min", "delete_min", "empty", "decrease_key"]

for x in ["comp"]:
    with open("up_ins_{}_measure".format(x)) as up:
        with open("down_ins_{}_measure".format(x)) as down:
            ups = []
            downs = []
            for line in up.readlines():
                ups.append([int(x) for x in line.split(",")[2:-1]])
            for line in down.readlines():
                downs.append([int(x) for x in line.split(",")[2:-1]])

            i = 0
            for u, d in zip(ups, downs):
                plt.plot(u)
                plt.plot(d)
                plt.legend(["0 to max-rank", "max-rank to 0"])
                plt.title(measures[i])
                plt.savefig("../graphs/instructions/{}_{}.png".format(x, measures[i]))
                plt.clf()
                i += 1
