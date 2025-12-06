#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <chrono>
#include <set>
#include "push_relabel.h"

using EdgeTuple = std::tuple<int,int,int>;

// Generate a random directed graph for max flow 
// n  = number of vertices (0 -> n-1)
// m  = number of directed edges
// max_cap = maximum capacity for an edge
// We always need edges going out of s and into t=n-1
std::vector<EdgeTuple> generate_random_flow_graph(int n, int m, int max_cap, std::mt19937 &rng) {
    int s = 0;
    int t = n - 1;

    // random number generators (probability distributions)
    std::uniform_int_distribution<int> node_dist(0, n - 1);
    std::uniform_int_distribution<int> cap_dist(1, max_cap);

    std::set<std::pair<int,int>> used;
    std::vector<EdgeTuple> edges;
    // eliminates need to resice and reallocate
    // reserving m amount ahead of time
    edges.reserve(m);

    // No duplicate edges so no self loops
    auto add_edge_unique = [&](int u, int v, int cap) {
        if (u == v) return false;
        auto key = std::make_pair(u, v);
        if (used.find(key) != used.end()) return false;
        used.insert(key);
        edges.emplace_back(u, v, cap);
        return true;
    };

    // Must have a few edges out of s
    for (int k = 0; k < 3 && (int)edges.size() < m; ++k) {
        int v = node_dist(rng);
        if (v == s) { --k; continue; }
        int cap = cap_dist(rng);
        if (!add_edge_unique(s, v, cap)) --k;
    }

    // Must have a few edges into t
    for (int k = 0; k < 3 && (int)edges.size() < m; ++k) {
        int u = node_dist(rng);
        if (u == t) { --k; continue; }
        int cap = cap_dist(rng);
        if (!add_edge_unique(u, t, cap)) --k;
    }

    // Fill the rest of graph with random edges
    while ((int)edges.size() < m) {
        int u = node_dist(rng);
        int v = node_dist(rng);
        int cap = cap_dist(rng);
        add_edge_unique(u, v, cap);
    }

    return edges;
}

// Run each algorithm on a random graph and calculate time
// Function returns runtime in milliseconds.
template <typename Algo>
double run_and_time(Algo &algo,
                    const std::vector<EdgeTuple> &edges,
                    int n, int s, int t,
                    const std::string &label)
{
    for (auto [u, v, c] : edges) {
        algo.add_edge(u, v, c);
    }

    auto start = std::chrono::steady_clock::now();
    int flow = algo.max_flow(s, t);
    auto end   = std::chrono::steady_clock::now();

    // display max flow and time 
    std::chrono::duration<double, std::milli> ms = end - start;
    std::cout << "  " << label
              << " max flow = " << flow
              << ", time = " << ms.count() << " ms\n";
    return ms.count();
}

int main() {
    std::mt19937 rng(42);  // for reproducibility

    // 
    std::vector<int> node_counts = {1000, 5000, 10000};
    int edges_per_node = 4;   // m is aproximately edges_per_node * n
    int max_capacity   = 20;
    int repetitions    = 3; // 3 runs per benchmark

    for (int n : node_counts) {
        int s = 0;
        int t = n - 1;
        int m = edges_per_node * n;
        std::cout << "\n=== Benchmark: n = " << n << " nodes, m = " << m << " edges ===\n";

        for (int rep = 1; rep <= repetitions; ++rep) {
            std::cout << " Run " << rep << ":\n";

            // Generate a random graph for each repetition
            auto edges = generate_random_flow_graph(n, m, max_capacity, rng);

            // Baseline (FIFO + Global Relabel)
            {
                PushRelabel pr(n);
                run_and_time(pr, edges, n, s, t, "Baseline  (FIFO + GR)");
            }

            // Heuristic (Highest-Label + Global Relabel + Gap)
            {
                PushRelabelHL pr_hl(n);
                run_and_time(pr_hl, edges, n, s, t, "Heuristic (HL + GR + Gap)");
            } 
            cout <<"\n---------------------------------------------\n";
        }
    }

    return 0;
}
