/******************************************************************************
 *  File: alg_graphs.h
 *
 *  A header file defining multiple graph-related classes. Implemenation is
 *  adapted from the implemenation of red Algorithms 4ed textbook which is
 *  available at https://algs4.cs.princeton.edu/code/.
 *
 *  Last modified by: Abdulmalek Al-Gahmi
 *  Last modified on: Aug 24, 2025
 ******************************************************************************/

#ifndef _ADV_ALG_GRAPHS_H_
#define _ADV_ALG_GRAPHS_H_

#include <iostream>
#include <string>
#include <list>
#include <stack>
#include <cassert>

/******************************************************************************
 *  Class: BaseGraph
 *  A base graph class capturing what is common between undirected and directed
 *  classes.
 ******************************************************************************/
class BaseGraph
{
protected:
  int _V = 0, _E = 0; // Number of vertices and edges
  std::list<int> *_adj = nullptr;
  void validate_vertex(int v) const;
  void copy_graph(const BaseGraph &g);

public:
  // Constructors
  BaseGraph() = default;
  explicit BaseGraph(int V);

  // Copy constructor and operator
  BaseGraph(const BaseGraph &);
  BaseGraph &operator=(const BaseGraph &);

  // Move constructor and operator
  BaseGraph(BaseGraph &&) noexcept;
  BaseGraph &operator=(BaseGraph &&) noexcept;

  // Vertices and edges
  int V() const;
  virtual void V(int v);
  int E() const;
  bool edge(int v, int w) const;
  std::list<int> adj(int v) const;

  virtual bool is_directed() const = 0;

  // Degrees
  virtual int degree(int v) const = 0;

  // Adding/removing
  virtual void add_edge(int v, int w) = 0;
  virtual void remove_edge(int v, int w) = 0;

  // Input/output
  virtual std::string str() const;
  friend std::ostream &operator<<(std::ostream &out, const BaseGraph &g);
  friend std::istream &operator>>(std::istream &in, BaseGraph &g);

  // Clean up
  virtual ~BaseGraph() noexcept;
};

/******************************************************************************
 *  Class: Graph
 *  A class representing undirected graphs
 ******************************************************************************/
class Graph : public BaseGraph
{
public:
  // Constructors
  Graph() = default;
  explicit Graph(int V);

  // Copy and move constructors
  Graph(Graph &);
  Graph(Graph &&) noexcept;

  // Vertices and edges
  bool is_directed() const override;

  // Degrees
  int degree(int v) const override;

  // Adding/removing/reversing
  void add_edge(int v, int w) override;
  void remove_edge(int v, int w) override;

  // Cleanup
  ~Graph() noexcept;
};

/******************************************************************************
 *  Class: Digraph
 *  A class representing directed graphs
 ******************************************************************************/
class Digraph : public BaseGraph
{
private:
  int *indegree = nullptr;

public:
  // Constructors
  Digraph() = default;
  explicit Digraph(int V);

  // Copy constructor and assignment operator
  Digraph(const Digraph &);
  Digraph &operator=(const Digraph &);

  // Move constructor and assignment operator
  Digraph(Digraph &&) noexcept;
  Digraph &operator=(Digraph &&) noexcept;

  // Vertices and edges
  void V(int v) override;
  bool is_directed() const override;

  // Degrees
  int degree(int v) const override;
  int out_degree(int v) const;
  int in_degree(int v) const;

  // Adding/removing/reversing
  void add_edge(int v, int w) override;
  void remove_edge(int v, int w) override;
  // void reverse(Digraph& r) const;
  Digraph reverse() const;

  // Clean up
  ~Digraph() noexcept;
};

/******************************************************************************
 *  Class: DepthFistSearch
 *  A class implementing the depth first search algorithm
 ******************************************************************************/
enum class Color
{
  White,
  Grey,
  Black
};
std::ostream &operator<<(std::ostream &out, Color &c);

struct VertexAttribute
{
  int parent = -1;
  Color color = Color::White;
  int time[2] = {0, 0};
  int component = 0;
};

class DepthFirstSearch
{
private:
  BaseGraph &g;
  VertexAttribute *v_attributes;
  int time = 0;
  int c_count = 0; // Components count
  std::list<int> pre, post;

public:
  DepthFirstSearch(BaseGraph &g);
  DepthFirstSearch(BaseGraph &g, int s);
  DepthFirstSearch(BaseGraph &g, std::list<int> &sources);

  void dfs(int u);

  int component(int v);
  int components_count();

  std::stack<int> path_to(int v);
  bool reachable(int v);

  const std::list<int> &in_preorder();
  const std::list<int> &in_postorder();
  std::stack<int> in_reverse_postorder();

  void show_in_preorder(std::ostream &out);
  void show_in_postorder(std::ostream &out);
  void show_in_reverse_postorder(std::ostream &out);

  std::string str() const;

  ~DepthFirstSearch() noexcept;
};

#endif
