#ifndef _ASSORTED_H_
#define _ASSORTED_H_

#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <climits>

template<class Heap>
long long Benchmark<Heap>::assorted(int N, int* nums, int* idxs, int* decs) {
    log("running assorted\n");

    std::vector<std::pair<int, int>> ns(N);
    std::vector<std::pair<int, typename Heap::reference>> m(N);

    log("beginning %d inserts, %d decrease-keys and %d delete-mins\n", N, 2*N, N);
    long long int pre_push = elapsed();

    Heap h;
    long long int total_time = 0;
    for (int i = 0; i < N; i++) {
        int x = nums[i];
        m[i] = std::make_pair(x, h.push(x, i));
        ns[i] = std::make_pair(x, i);
    }

    for (int i = 0; i < 2*N; i++) {
        int idx = idxs[i];

        int x = m[idx].first;
        x -= decs[i];

        if (i == 5)
            continue;

        ns[idx].first = x;

        m[idx].first = x;
        m[idx].second = h.decrease_key(m[idx].second, x);
    }
    total_time += elapsed() - pre_push;

    std::sort(ns.begin(), ns.end());
    std::vector<int> end_val(N);
    for (int i = 0; i < N; i++)
        end_val[ns[i].second] = ns[i].first;

    long long int pre_retrieval = elapsed();
    int correct = 1;
    int prev = INT_MIN;
    for (int i = 0; i < N; i++) {
        int* res = (int*) h.find_min();
        if (end_val[*res] > prev)
            correct = 1;
        prev = end_val[*res];
        h.delete_min();
    }
    total_time += elapsed() - pre_retrieval;
    log("elapsed = %lld us\n", total_time);

    if (correct)
        log("correct!\n");
    else
        log("incorrect!\n");

    return total_time;
}

#endif  // _ASSORTED_H_
