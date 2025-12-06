/******************************************************************************
 *  File: alg_uf.h
 * 
 *  A header file defining multiple union-find classes. Implemenation is 
 *  adapted from the implemenation of red Algorithms 4ed textbook which is 
 *  available at https://algs4.cs.princeton.edu/code/.
 * 
 *  Last modified by: Abdulmalek Al-Gahmi
 *  Last modified on: Jan 9, 2021
 ******************************************************************************/

#ifndef _ADV_ALG_UF_H_
#define _ADV_ALG_UF_H_

#include <vector>
/******************************************************************************
 *  Class: UFable
 *  An abstract class capturing the essence of UF classes
 ******************************************************************************/
class UFable {
public:
  UFable() {}
  virtual int components_count() const = 0;
  virtual bool connected(int p, int q) {
    return _find(p) == _find(q);
  }

  virtual int _find(int p) = 0;
  virtual bool _union(int p, int q) = 0;

  virtual ~UFable() noexcept {}
};

/******************************************************************************
 *  Class: UF
 *  A simple implementation of Union-Find
 ******************************************************************************/
class UF : public UFable {
protected:
  std::vector<int> id;
  int count;

public:
  UF(int N): count(N) {
    for (int i = 0; i < N; i++) {
      id.push_back(i);
    }
  }

  int components_count() const override { return count; }

  int _find(int p) override { return id[p]; }

  bool _union(int p, int q) override {
    int pID = _find(p);
    int qID = _find(q);

    if (pID == qID) return false;
    for (int i = 0; i < id.size(); i++) {
      if (id[i] == pID) id[i] = qID;
    }

    count--;
    return true;
  }
};

/******************************************************************************
 *  Class: QuickUF
 *  A quick implementation of Union-Find
 ******************************************************************************/
class QuickUF : public UF {
public:
  QuickUF(int N): UF(N){}

  int _find(int p) override {
    while (p != id[p]){
      p = id[p];
    }

    return p;
  }

  bool _union(int p, int q) override {
    int pRoot = _find(p);
    int qRoot = _find(q);
    if (pRoot == qRoot) return false;
    id[pRoot] = qRoot;
    this->count--;
    return true;
  }
};

/******************************************************************************
 *  Class: WeightedQuickUF
 *  A quick weighted implementation of Union-Find
 ******************************************************************************/
class WeightedQuickUF :public QuickUF {
protected:
  std::vector<int> sz;

public:
  WeightedQuickUF(int N) : QuickUF(N) {
    for (int i = 0; i < N; i++) {
      sz.push_back(1);
    }
  }

  bool _union(int p, int q) override{
    int pRoot = _find(p);
    int qRoot = _find(q);
    if (pRoot == qRoot) return false;

    if(sz[pRoot] < sz[qRoot]){
      id[pRoot] = qRoot;
      sz[qRoot] += sz[pRoot];
    }else{
      id[qRoot] = pRoot;
      sz[pRoot] += sz[qRoot];
    }

    this->count--;
    return true;
  }
};

/******************************************************************************
 *  Class: PCWQuickUF
 *  A quick weighted implementation of Union-Find with path compression
 ******************************************************************************/
class PCWQuickUF : public WeightedQuickUF {
public:
  PCWQuickUF(int N) : WeightedQuickUF(N) {}

  int _find(int p) override {
    while (p != id[p]) {
      id[p] = id[id[p]];  // path compression by halving
      p = id[p];
    }
    return p;
  }
};

/* TESTING
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include "alg_uf.h"
#include "alg_stopwatch.h"

using namespace std;

int main() {
  // TODO

  struct Algorithm {
    string name;
    UFable *impl;
    long runningTime;
  };

  string inputFiles [] = {
    "../resources/tinyUF.txt", "../resources/mediumUF.txt"
    };

  ostringstream report;
  report << setw(27) << "Input"
         << setw(7) << "N"
         << setw(10) << "Algorithm"
         << setw(11) << "Components"
         << setw(16) << "Time(millisecs)" << endl;
  report << string(72, '=') << endl;
  for (string inputFile : inputFiles) {
    ifstream in(inputFile);
    int n;

    in >> n;
    Algorithm algorithms [] = {
        { "UF", new UF(n) },
        { "QU", new QuickUF(n) },
        { "WQ", new WeightedQuickUF(n) }, 
        { "PC", new PCWQuickUF(n) }
    };
    in.close();

    for (Algorithm a : algorithms) {
      int p, q;
      ifstream in(inputFile);
      in >> n;
      StopWatch sw;

      while (in >> p >> q) {
        cout << p << '-' << q << endl;
        a.impl->_union(p, q);
      }

      a.runningTime = sw.elapsed_time();
      in.close();
      report << setw(27) << inputFile
             << setw(7) << n
             << setw(10) << a.name
             << setw(11) << a.impl->components_count()
             << setw(16) << a.runningTime << endl;

      delete a.impl;

      in.close();
    }
  }

  cout << report.str() << endl;

  return 0;
}*/
#endif

