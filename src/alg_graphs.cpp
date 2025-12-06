/******************************************************************************
 *  File: all_graphs.cpp
 *
 *  An implementation file of multiple graph-related classes. Implemenation is
 *  adapted from the implemenation of red Algorithms 4ed textbook which is
 *  available at https://algs4.cs.princeton.edu/code/.
 *
 *  Last modified by: Abdulmalek Al-Gahmi
 *  Last modified on: Aug 24, 2025
 ******************************************************************************/

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include "alg_graphs.h"

/******************************************************************************
 *  Class: BaseGraph
 *  A base graph class capturing what is common between undirected and directed
 *  classes.
 ******************************************************************************/
void BaseGraph::validate_vertex(int v) const
{
  if (v < 0 || v > _V)
  {
    throw std::runtime_error("vertex " + std::to_string(v) + " is not between 0 and " + std::to_string(_V - 1));
  }
}

void BaseGraph::copy_graph(const BaseGraph &g)
{
  V(g.V());
  _E = g.E();

  for (int v = 0; v < _V; v++)
  {
    for (int w : g.adj(v))
    {
      _adj[v].push_back(w);
    }
  }
}

// Constructors
BaseGraph::BaseGraph(int V) : _V(V)
{
  if (_V != 0)
  {
    _adj = new std::list<int>[V];
  }
}

// copy constructor and operator
BaseGraph::BaseGraph(const BaseGraph &g) : _V(0)
{
  copy_graph(g);
}

BaseGraph &BaseGraph::operator=(const BaseGraph &g)
{
  if (this != &g)
  {
    delete[] _adj;
    _V = 0;
    copy_graph(g);
  }

  return *this;
}

// Move constructor and operator
BaseGraph::BaseGraph(BaseGraph &&g) noexcept : _V(g._V), _E(g._E), _adj(g._adj)
{
  g._V = 0;
  g._E = 0;
  g._adj = nullptr;
}

BaseGraph &BaseGraph::operator=(BaseGraph &&g) noexcept
{
  delete[] _adj;
  _V = g._V;
  _E = g._E = 0;
  _adj = g._adj;

  g._V = 0;
  g._E = 0;
  g._adj = nullptr;

  return *this;
}

// Vertices and edges
int BaseGraph::V() const { return _V; }

void BaseGraph::V(int V)
{
  if (_V != 0 && _V != V)
  {
    throw std::runtime_error("Cannot change the number of vertices of an existing graph");
  }

  _V = V;
  _adj = new std::list<int>[V];
}

int BaseGraph::E() const { return _E; }

bool BaseGraph::edge(int v, int w) const
{
  validate_vertex(v);
  validate_vertex(w);
  auto &list = _adj[v];
  auto it = std::find(list.begin(), list.end(), w);
  return it != list.end();
}

std::list<int> BaseGraph::adj(int v) const
{
  validate_vertex(v);
  return _adj[v];
}

// Input/output
std::string BaseGraph::str() const
{
  std::ostringstream sout;
  sout << *this;
  return sout.str();
}

std::ostream &operator<<(std::ostream &out, const BaseGraph &g)
{
  out << g._V << std::endl
      << g._E << std::endl;
  for (int v = 0; v < g._V; v++)
  {
    out << v << ": ";
    for (int w : g._adj[v])
    {
      out << w << " ";
    }

    out << std::endl;
  }

  return out;
}

std::istream &operator>>(std::istream &in, BaseGraph &g)
{
  std::string line;
  while (std::getline(in, line))
  {
    if (g._V == 0)
    {
      g.V(std::stoi(line));
    }
    else
    {
      std::istringstream ss(line);

      int v;
      ss >> v;
      char colon;
      ss >> colon;
      int w;
      while (ss >> w)
      {
        if (g.is_directed())
        {
          g.add_edge(v, w);
        }
        else
        {
          g._E++;
          g._adj[v].push_back(w);
        }
      }
    }
  }

  if (!g.is_directed())
    g._E /= 2;

  return in;
}

// Clean up
BaseGraph::~BaseGraph() noexcept
{
  delete[] _adj;
}

/******************************************************************************
 *  Class: Graph
 *  A class representing undirected graphs
 ******************************************************************************/
// Constructors
Graph::Graph(int V) : BaseGraph(V) {}

// Copy and move constructors
Graph::Graph(Graph &g) : BaseGraph(g) {}
Graph::Graph(Graph &&g) noexcept : BaseGraph(g) {}

bool Graph::is_directed() const { return false; }

int Graph::degree(int v) const
{
  return this->adj(v).size();
}

// Adding/removing edges
void Graph::add_edge(int v, int w)
{
  validate_vertex(v);
  validate_vertex(w);
  _E++;
  _adj[v].push_back(w);
  _adj[w].push_back(v);
}

void Graph::remove_edge(int v, int w)
{
  validate_vertex(v);
  validate_vertex(w);
  auto &v_list = this->_adj[v];
  auto it = std::find(v_list.begin(), v_list.end(), w);
  v_list.erase(it);

  auto w_list = this->_adj[w];
  it = std::find(w_list.begin(), w_list.end(), v);
  w_list.erase(it);
  this->_E--;
}

// Cleanup
Graph::~Graph() noexcept {}

/******************************************************************************
 *  Class: Digraph
 *  A class representing directed graphs
 ******************************************************************************/
// Constructors
Digraph::Digraph(int V) : BaseGraph(V)
{
  if (V != 0)
  {
    indegree = new int[V]{0};
  }
}

// Copy constructor and  assignment operator
Digraph::Digraph(const Digraph &g) : BaseGraph(g)
{
  if (this->_V != 0)
  {
    indegree = new int[this->_V]{0};
    for (int v = 0; v < this->_V; v++)
    {
      indegree[v] = g.indegree[v];
    }
  }
}

Digraph &Digraph::operator=(const Digraph &g)
{
  if (this != &g)
  {
    this->BaseGraph::operator=(g);

    if (this->_V != 0)
    {
      indegree = new int[this->_V]{0};
      for (int v = 0; v < this->_V; v++)
      {
        indegree[v] = g.indegree[v];
      }
    }
  }

  return *this;
}

// Move constructor and  assignment operator
Digraph::Digraph(Digraph &&g) noexcept : BaseGraph(g)
{
  indegree = g.indegree;
  g.indegree = nullptr;
}

Digraph &Digraph::operator=(Digraph &&g) noexcept
{
  this->BaseGraph::operator=(g);
  indegree = g.indegree;
  g.indegree = nullptr;

  return *this;
}

// Vertices and edges
void Digraph::V(int V)
{
  BaseGraph::V(V);
  indegree = new int[V]{0};
}

bool Digraph::is_directed() const { return true; }

// Degrees
int Digraph::degree(int v) const
{
  return out_degree(v) + in_degree(v);
}

int Digraph::out_degree(int v) const
{
  return this->adj(v).size();
}

int Digraph::in_degree(int v) const
{
  this->validate_vertex(v);
  return indegree[v];
}

// Adding/removing edges
void Digraph::add_edge(int v, int w)
{
  validate_vertex(v);
  validate_vertex(w);
  _E++;
  _adj[v].push_back(w);
  indegree[w]++;
}

void Digraph::remove_edge(int v, int w)
{
  validate_vertex(v);
  validate_vertex(w);
  auto &v_list = this->_adj[v];
  auto it = std::find(v_list.begin(), v_list.end(), w);
  v_list.erase(it);
  this->_E--;
  indegree[w]--;
}

Digraph Digraph::reverse() const
{
  Digraph r;
  r.V(BaseGraph::V());
  for (int v = 0; v < BaseGraph::V(); v++)
  {
    for (int w : this->adj(v))
    {
      r.add_edge(w, v);
    }
  }

  return r;
}

// Cleanup
Digraph::~Digraph() noexcept
{
  delete[] indegree;
}

/******************************************************************************
 *  Class: DepthFistSearch
 *  A class implementing the depth first search algorithm
 ******************************************************************************/
std::ostream &operator<<(std::ostream &out, Color &c)
{
  switch (c)
  {
  case Color::White:
    return out << "W"; // White
  case Color::Grey:
    return out << "G"; // Grey
  case Color::Black:
    return out << "B"; // Black
  }

  return out << "U"; // Unknown
}

DepthFirstSearch::DepthFirstSearch(BaseGraph &g) : g(g), v_attributes(new VertexAttribute[g.V()])
{
  for (int v = 0; v < g.V(); v++)
  {
    if (v_attributes[v].color == Color::White)
    {
      dfs(v);
    }
  }
}

DepthFirstSearch::DepthFirstSearch(BaseGraph &g, int s) : g(g), v_attributes(new VertexAttribute[g.V()])
{
  if (v_attributes[s].color == Color::White)
  {
    dfs(s);
  }
}

DepthFirstSearch::DepthFirstSearch(BaseGraph &g, std::list<int> &sources) : g(g), v_attributes(new VertexAttribute[g.V()])
{
  for (int s : sources)
  {
    if (v_attributes[s].color == Color::White)
    {
      dfs(s);
    }
  }
}

void DepthFirstSearch::dfs(int u)
{
  time++;

  v_attributes[u].time[0] = time;
  v_attributes[u].color = Color::Grey;
  if (g.is_directed())
    pre.push_back(u);
  for (int v : g.adj(u))
  {
    if (v_attributes[v].color == Color::White)
    {
      v_attributes[v].parent = u;
      dfs(v);
    }
  }
  if (g.is_directed())
    post.push_back(u);
  v_attributes[u].color = Color::Black;
  v_attributes[u].component = c_count;
  if (v_attributes[u].parent == -1)
  {
    c_count++;
  }

  time++;
  v_attributes[u].time[1] = time;
}

std::stack<int> DepthFirstSearch::path_to(int v)
{
  std::stack<int> path;
  int x = v;
  while (v_attributes[x].parent != -1)
  {
    path.push(x);
    x = v_attributes[x].parent;
  }
  path.push(x);

  return path;
}

int DepthFirstSearch::component(int v)
{
  return v_attributes[v].component;
}

int DepthFirstSearch::components_count()
{
  return c_count;
}

bool DepthFirstSearch::reachable(int v)
{
  return v_attributes[v].color == Color::Black;
}

const std::list<int> &DepthFirstSearch::in_preorder()
{
  return pre;
}

const std::list<int> &DepthFirstSearch::in_postorder()
{
  return post;
}

std::stack<int> DepthFirstSearch::in_reverse_postorder()
{
  std::stack<int> reverse;
  for (int v : post)
  {
    reverse.push(v);
  }

  return reverse;
}

void DepthFirstSearch::show_in_preorder(std::ostream &out)
{
  for (int v : pre)
  {
    out << v << " ";
  }

  out << std::endl;
}

void DepthFirstSearch::show_in_postorder(std::ostream &out)
{
  for (int v : post)
  {
    out << v << " ";
  }

  out << std::endl;
}

void DepthFirstSearch::show_in_reverse_postorder(std::ostream &out)
{
  std::stack<int> reverse = in_reverse_postorder();
  while (!reverse.empty())
  {
    out << reverse.top() << " ";
    reverse.pop();
  }

  out << std::endl;
}

std::string DepthFirstSearch::str() const
{
  std::ostringstream sout;
  for (int v = 0; v < g.V(); v++)
  {
    sout << v << ": (" << v_attributes[v].time[0]
         << '/' << v_attributes[v].time[1] << ")"
         << " - " << v_attributes[v].color
         << " - " << v_attributes[v].parent
         << " - " << v_attributes[v].component << '\n';
  }

  return sout.str();
}

DepthFirstSearch::~DepthFirstSearch() noexcept
{
  delete[] v_attributes;
}
