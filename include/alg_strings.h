/******************************************************************************
 *  File: alg_strings.h
 *
 *  A header file defining multiple string-matching classes. Implemenation is
 *  adapted from that of the red Algorithms 4ed textbook which is available at
 *  https://algs4.cs.princeton.edu/code/.
 *
 *  Last modified by: Abdulmalek Al-Gahmi
 *  Last modified on: Aug 24, 2025
 ******************************************************************************/

#ifndef _ADV_ALG_STRINGS_H_
#define _ADV_ALG_STRINGS_H_

#include <iostream>
#include <string>
#include "alg_graphs.h"

/******************************************************************************
 *  Class: RegExMatcher
 *  A class recognizing a regex pattern.
 ******************************************************************************/
class RegExMatcher
{
private:
  static Digraph construct_nfa(const std::string &pattern, int M);

public:
  static bool recognizes(const std::string &pattern, const std::string &text);
};

/******************************************************************************
 *  Class: RabinKarp
 *  A class implementing the Rabin-Karp algorithm
 ******************************************************************************/
class RabinKarp
{
private:
  std::string pat;
  long pat_hash; // Pattern hash value
  int m;         // Pattern length
  long q;        // A large prime, small enough to avoid overflow
  int R;         // Radix
  long RM;       // R^(M-1) % Q

  long hash(const std::string &key, int m) const;
  bool check(const std::string &txt, int i) const;

  static long long_random_prime();

public:
  RabinKarp(const std::string &pat);
  int search(const std::string &txt) const;
};

#endif