#define CATCH_CONFIG_MAIN // Tells Catch2 to provide a main() function
#include <catch2/catch_all.hpp>
#include <fstream>
#include <string>
#include "../src/push_relabel.h"

using EdgeTuple = std::tuple<int, int, int>;

using namespace std;
// Helper to run both algorithms and compare against expected flow
static void run_maxflow_test(
    int n, int s, int t,
    const std::vector<EdgeTuple> &edges,
    int expected_flow)
{
    // Baseline: FIFO + Global relabel
    PushRelabel pr(n);
    for (auto [u, v, c] : edges) {
        pr.add_edge(u, v, c);
    }
    int flow1 = pr.max_flow(s, t);

    // Highest-label + Gobal rebal + Gap
    PushRelabelHL pr_hl(n);
    for (auto [u, v, c] : edges) {
        pr_hl.add_edge(u, v, c);
    }
    int flow2 = pr_hl.max_flow(s, t);

    // Flow must match expected
    REQUIRE(flow1 == expected_flow);
    REQUIRE(flow2 == expected_flow);

    // Flows must be the same 
    REQUIRE(flow1 == flow2);
}


// Test 1: Single edge
TEST_CASE("Single edge graph", "[simple_case]") {
    int n = 2, s = 0, t = 1;
    std::vector<EdgeTuple> edges = {
        EdgeTuple{0, 1, 5}
    };
    int expected_flow = 5;

    run_maxflow_test(n, s, t, edges, expected_flow);
}

// Test 2: Two parallel paths from s to t
// 0 -> 1 -> 3 (cap 3), 0 -> 2 -> 3 (cap 5)
// max flow = 8
TEST_CASE("Two parallel s-t paths", "[parallel_paths]") {
    int n = 4, s = 0, t = 3;
    std::vector<EdgeTuple> edges = {
        EdgeTuple{0,1,3},
        EdgeTuple{1,3,3},
        EdgeTuple{0,2,5},
        EdgeTuple{2,3,5}
    };
    int expected_flow = 8;

    run_maxflow_test(n, s, t, edges, expected_flow);
}

// Test 3: no path from s to t -> flow must be 0
TEST_CASE("No path to sink", "[edgecase1]") {
    int n = 3, s = 0, t = 2;
    std::vector<EdgeTuple> edges = {
        EdgeTuple{0,1,10}  // no edge into 2
    };
    int expected_flow = 0;

    run_maxflow_test(n, s, t, edges, expected_flow);
}

// Test 4: Shared bottleneck at the sink side
// 0 -> 1 (10), 1 -> 3 (3)
// 0 -> 2 (10), 2 -> 3 (3)
// total outgoing from 3 is 6 => max flow = 6
TEST_CASE("Shared bottleneck", "[bottleneck]") {
    int n = 4, s = 0, t = 3;
    std::vector<EdgeTuple> edges = {
        EdgeTuple{0,1,10},
        EdgeTuple{1,3,3},
        EdgeTuple{0,2,10},
        EdgeTuple{2,3,3}
    };
    int expected_flow = 6;

    run_maxflow_test(n, s, t, edges, expected_flow);
}

// Test 5: 0 capacity edges have no change to the flow
// 0 -> 1 (0),  0 -> 2 (4)
// 2 -> 1 (1),  1 -> 3 (1)
// 2 -> 3 (3)
// Paths: 0-2-3: 3, + 0-2-1-3: 1 => total 4
TEST_CASE("Zero-capacity edges", "[edgecase2]") {
    int n = 4, s = 0, t = 3;
    std::vector<EdgeTuple> edges = {
        EdgeTuple{0,1,0},
        EdgeTuple{0,2,4},
        EdgeTuple{2,1,1},
        EdgeTuple{1,3,1},
        EdgeTuple{2,3,3}
    };
    int expected_flow = 4;

    run_maxflow_test(n, s, t, edges, expected_flow);
}

// Test 6: Source has no outgoing edges: flow = 0
TEST_CASE("Source with no outgoing edges", "[edgecase3]") {
    int n = 3, s = 0, t = 2;
    std::vector<EdgeTuple> edges = {
        EdgeTuple{1, 2, 7},  // sink is reachable from 1
        EdgeTuple{1, 0, 3}   // but source 0 has no outgoing edges
    };
    int expected_flow = 0;

    run_maxflow_test(n, s, t, edges, expected_flow);
}

// Test 7: Anti parallel edges between nodes (0 <-> 1) and 2 s -> t routes
// 0 -> 1 (5), 1 -> 0 (3), 1 -> 2 (4), 0 -> 2 (1)
// Possible paths: 
//   0 -> 2: 1
//   0 -> 1 -> 2: 4
// max flow = 5
TEST_CASE("Anti-parallel edges", "[anti_parallel]") {
    int n = 3, s = 0, t = 2;
    std::vector<EdgeTuple> edges = {
        EdgeTuple{0, 1, 5},
        EdgeTuple{1, 0, 3},
        EdgeTuple{1, 2, 4},
        EdgeTuple{0, 2, 1}
    };
    int expected_flow = 5;

    run_maxflow_test(n, s, t, edges, expected_flow);
}