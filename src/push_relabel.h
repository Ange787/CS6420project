#ifndef PUSH_RELABEL_H
#define PUSH_RELABEL_H

#include <vector>
#include <queue>
#include "push_relabel.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <limits>
#include <vector> 
#include <tuple>

using namespace std;

// ====================
// Baseline Push- Relabel
// Class includes two main parts: 
// 1) FIFO queue
// 2) Global relabeling
// ======================

class PushRelabel
{
public:
  struct Edge
  {
    int to; // head vertex of the edge
    int reverse; // index of the reverse edge
    int capacity; // residual capacity on edge
  };

  // create graph with n vertices
  explicit PushRelabel(int n)
      : n_(n),
        G_(n),
        height_(n, 0),
        excess_(n, 0),
        in_queue_(n, false) {}

  // Add a diected edge u -> v with initial capapcity
  // create reverse edge v -> u with capacity = 0
  void add_edge(int u, int v, int capacity)
  {
    Edge a{v, static_cast<int>(G_[v].size()), capacity};
    Edge b{u, static_cast<int>(G_[u].size()), 0};
    G_[u].push_back(a);
    G_[v].push_back(b);
  }

  // Compute max from from s(source) to t (sink)
  int max_flow(int s, int t)
  {
    // reset per run
    height_.assign(n_, 0);
    excess_.assign(n_, 0);

    while (!q_.empty())
      q_.pop();
    std::fill(in_queue_.begin(), in_queue_.end(), false);

    // preflow initialization 
    height_[s] = n_;

    // push full capacity on edges from the source
    for (auto &e : G_[s])
    {
      if (e.capacity > 0)
      {
        int flow = e.capacity;
        e.capacity -= flow;
        G_[e.to][e.reverse].capacity += flow;
        excess_[e.to] += flow;
        
        // any vertex that gets excess becomes active (EXCEPT SOURCE AND SINK)
        if (e.to != s && e.to != t && !in_queue_[e.to])
        {
          q_.push(e.to);
          in_queue_[e.to] = true;
        }
      }
    }

    // number of vertices discharged
    // this will be used to trigger global relabeling if needed
    int discharge_count = 0;

    // Process active vertices in FIFO order 
    while (!q_.empty())
    {
      int u = q_.front();
      q_.pop();
      in_queue_[u] = false;

      discharge(u, s, t);

      // Increment vertex discharged counter
      discharge_count++;

      // Run global relabel to update the height labels 
      if (discharge_count % n_ == 0)
      {
        global_relabel(t);
      }

      // If u is still has excess after discharge, activate it again
      if (excess_[u] > 0 && u != s && u != t)
      {
        q_.push(u);
        in_queue_[u] = true;
      }
    }

    // The max flow value is the excess at t (sink vertex)
    return excess_[t];
  }

private:
  int n_;                               // # of vertices
  std::vector<std::vector<Edge>> G_;    // residual graph adjacency list
  std::vector<int> height_;             // height labels
  std::vector<int> excess_;             // excess flow of each vertex
  std::queue<int> q_;                   // FIFO queue of active vertices
  std::vector<bool> in_queue_;          // verify if in queue? T or F

  // push operation from vertex u along edge e
  void push(int u, Edge &e)
  {
    int v = e.to;
    int send = std::min(excess_[u], e.capacity);
    if (send <= 0)
      return;
    
    // update residual capacities
    e.capacity -= send;
    G_[v][e.reverse].capacity += send;

    // update excess at end points
    excess_[u] -= send;
    excess_[v] += send;
  }

  // relabel by increasing height[u] to 1 + min height[v]
  void relabel(int u)
  {
    int min_height = std::numeric_limits<int>::max();
    for (const auto &e : G_[u])
    {
      if (e.capacity > 0)
      {
        min_height = std::min(min_height, height_[e.to]);
      }
    }
    if (min_height < std::numeric_limits<int>::max())
    {
      height_[u] = min_height + 1;
    }
  }
  // admissible edge: residual edge allowed to push flow
  // push out admissible edges out of u 
  // relabel if there are no admissible edges available until: excess[u] = 0  
  void discharge(int u, int s, int t)
  {
    // keep pushing/relabeling until vertex u has no excess
    while (excess_[u] > 0)
    {
      bool pushed_any = false;

      for (auto &e : G_[u])
      {
        if (excess_[u] == 0)
          break;

        // residual cap > 0 and height[u] = height[v] + 1
        if (e.capacity > 0 && height_[u] == height_[e.to] + 1)
        {
          int before_excess_v = excess_[e.to];
          push(u, e);
          pushed_any = true;

          int v = e.to;
          // If v gains excess and is not s or t, activate it
          if (v != s && v != t && excess_[v] > 0 && !in_queue_[v])
          {
            q_.push(v);
            in_queue_[v] = true;
          }
        }
      }

      if (!pushed_any)
      {
        // no edges; no need to relabel
        relabel(u);
      }
    }
  }
  // global replaces all heights at one with the exact shortest distance
  // to sink in the residual graph
  // Different ways to trigger this method: after a certain number of push/relabel operations
  void global_relabel(int t)
  {
    int INF = 1e9;
    std::fill(height_.begin(), height_.end(), INF);

    std::queue<int> q;
    height_[t] = 0;
    q.push(t);

    while (!q.empty())
    {
      int v = q.front();
      q.pop();

      for (const auto &e : G_[v])
      {
        // check reverse residual edge into v
        const auto &reverse = G_[e.to][e.reverse];
        if (reverse.capacity > 0 && height_[e.to] == INF)
        {
          height_[e.to] = height_[v] + 1;
          q.push(e.to);
        }
      }
    }
  }
};

// =======================================================
// Push-Relabel with highest label selection
// Class includes: 
// 1) Active vertices bucketed by height
// 2) Process the highgest-label active vertex 
// 3) Gap heuristics: skip unaccesible regions of a graph
// ======================================================
class PushRelabelHL
{
public:
  struct Edge
  {
    int to;   // head_vertex
    int rev; // index of reverse edge in G_[to]
    int cap; // residual capacity
  };

  explicit PushRelabelHL(int n)
      : n_(n),
        G_(n),
        height_(n, 0),
        excess_(n, 0),
        active_(n, false),
        max_height_active_(-1),
        height_count_() {}

  // add directed edge u -> with capacity cap and reverse edge v-> u
  void add_edge(int u, int v, int cap)
  {
    Edge a{v, (int)G_[v].size(), cap};
    Edge b{u, (int)G_[u].size(), 0};
    G_[u].push_back(a);
    G_[v].push_back(b);
  }

  // Calculate max flow from s to t with: 
  // * highest label selection 
  // * gap heuristics 
  // * global relabeling (found in baseline HL too)
  int max_flow(int s, int t)
  {
    // heights are at most 2n
    int H = 2 * n_;
    active_by_height_.assign(H, {});
    std::fill(height_.begin(), height_.end(), 0);
    std::fill(excess_.begin(), excess_.end(), 0);
    std::fill(active_.begin(), active_.end(), false);
    max_height_active_ = -1;

    // number of vertices with height h
    height_count_.assign(2 * n_ + 2, 0);

    // preflow intialization
    height_[s] = n_;
    for (int v = 0; v < n_; ++v)
    {
      height_count_[height_[v]]++;
    }

    // push full capacity out of the source vertex
    for (auto &e : G_[s])
    {
      if (e.cap > 0)
      {
        int flow = e.cap;
        e.cap -= flow;
        G_[e.to][e.rev].cap += flow;
        excess_[e.to] += flow;

        if (e.to != s && e.to != t)
        {
          activate(e.to);
        }
      }
    }

    // to keep track of numbers of discharged vertices
    int discharge_count = 0;

    // Main loop of class
    // Always process an active vertex with the highest height
    while (max_height_active_ >= 0)
    {
      // skip buckets that are 1) empty or 2) go over array
      if (max_height_active_ >= (int)active_by_height_.size())
      {
        max_height_active_--;
        continue;
      }

      if (active_by_height_[max_height_active_].empty())
      {
        // move down to next non-empty height
        max_height_active_--;
        continue;
      }

      int u = active_by_height_[max_height_active_].back();
      active_by_height_[max_height_active_].pop_back();
      active_[u] = false;

      discharge(u, s, t);

      // update discharged vertice counter
      discharge_count++;

      // constantly relabeling and also bucket rebuilding 
      if (discharge_count % n_ == 0)
      {
        global_relabel_with_rebuild(t);
      }
    }

    return excess_[t];
  }

private:
  int n_;                                           // # of vertices
  std::vector<std::vector<Edge>> G_;                // residual graph
  std::vector<int> height_;                               
  std::vector<int> excess_;                         // excess flow at vertices

  // Highest label data structures
  std::vector<std::vector<int>> active_by_height_; // bucketed active vertices
  std::vector<bool> active_;                       // is vertex currently in some bucket?
  int max_height_active_;                           // highest h with non empty bucket

  // gap heuristic tracking
  std::vector<int> height_count_; // # of vertices at each height

  // mark vertex v as active and place in correct buckets
  void activate(int v)
  {
    if (!active_[v] && excess_[v] > 0 && height_[v] < (int)active_by_height_.size())
    {
      active_[v] = true;
      active_by_height_[height_[v]].push_back(v);
      if (height_[v] > max_height_active_)
      {
        max_height_active_ = height_[v];
      }
    }
  }

  // push operation using highest label 
  // better to use vertices s and t so no activation occurs
  void push(int u, Edge &e, int s, int t)
  {
    int v = e.to;
    int send = std::min(excess_[u], e.cap);
    if (send <= 0)
      return;

    e.cap -= send;
    G_[v][e.rev].cap += send;

    excess_[u] -= send;
    excess_[v] += send;

    if (v != s && v != t)
    {
      activate(v);
    }
  }

  // update label if gap detected 
  void relabel(int u)
  {
    int min_height = std::numeric_limits<int>::max();
    for (const auto &e : G_[u])
    {
      if (e.cap > 0)
      {
        min_height = std::min(min_height, height_[e.to]);
      }
    }
    if (min_height < std::numeric_limits<int>::max())
    {
      int old_h = height_[u];
      int new_h = min_height + 1;

      height_[u] = new_h;
      // update height counts
      height_count_[old_h]--;
      height_count_[new_h]++;

      // gap function is triggered if: 
      // no vertex has height old_h AND old_h a number between 0 and n 
      if (old_h > 0 && old_h < n_ && height_count_[old_h] == 0)
      {
        gap(old_h);
        rebuild_buckets();
      }
    }
  }

  // Same function as in the baseline relabel clas but we need to track more here
  // Now we have height buckets, height counts and gap heuristics
  void discharge(int u, int s, int t)
  {
    while (excess_[u] > 0)
    {
      bool pushed_any = false;
      for (auto &e : G_[u])
      {
        if (excess_[u] == 0)
          break;

        if (e.cap > 0 && height_[u] == height_[e.to] + 1)
        {
          push(u, e, s, t);
          pushed_any = true;
        }
      }
      if (!pushed_any)
      {
        // no admissible edges, relabel and start again
        relabel(u);
      }
    }

    // After discharge, if u still has excess, re-activate it
    if (excess_[u] > 0 && u != s && u != t)
    {
      activate(u);
    }
  }

  // Gap function
  void gap(int k)
  {
    int HINF = n_ + 1;
    for (int v = 0; v < n_; ++v)
    {
      int h = height_[v];
      if (h > k && h < n_)
      {
        height_count_[h]--;
        height_[v] = HINF;
        height_count_[HINF]++;
      }
    }
  }

  void global_relabel(int t)
  {
    int INF = 2 * n_;
    std::fill(height_.begin(), height_.end(), INF);

    std::queue<int> q;
    height_[t] = 0;
    q.push(t);

    while (!q.empty())
    {
      int v = q.front();
      q.pop();

      for (const auto &e : G_[v])
      {
        // Reverse residual: look at edges INTO v
        const auto &rev = G_[e.to][e.rev];
        if (rev.cap > 0 && height_[e.to] == INF)
        {
          height_[e.to] = height_[v] + 1;
          q.push(e.to);
        }
      }
    }

    // recompute height_count_ after global relabel
    std::fill(height_count_.begin(), height_count_.end(), 0);
    for (int v = 0; v < n_; ++v)
    {
      int h = height_[v];
      if (h < (int)height_count_.size())
      {
        height_count_[h]++;
      }
    }
  }

  // rebuild height buckets based on current active and height 
  void rebuild_buckets()
  {
    int H = active_by_height_.size();
    active_by_height_.assign(H, {});
    max_height_active_ = -1;

    for (int v = 0; v < n_; ++v)
    {
      if (active_[v] && excess_[v] > 0 && height_[v] < H)
      {
        active_by_height_[height_[v]].push_back(v);
        if (height_[v] > max_height_active_)
        {
          max_height_active_ = height_[v];
        }
      }
    }
  }

  // Two in one: global relabeling and rebulding highest label buckets
  // reuse already computed functions
  void global_relabel_with_rebuild(int t)
  {
    global_relabel(t);
    rebuild_buckets();
  }
};


#endif
