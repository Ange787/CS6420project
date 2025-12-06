/******************************************************************************
 *  File: alg_maxsubarraysum.h
 *
 *  A header file of a utility class for measuring the running time of an
 *  algorithm. Implemenation is adapted from that of the red Algorithms 4ed
 *  textbook which is available at https://algs4.cs.princeton.edu/code/.
 *
 *  Last modified by: Abdulmalek Al-Gahmi
 *  Last modified on: Aug 24, 2025
 ******************************************************************************/

#ifndef _ADV_ALG_MAX_SUBARRAY_SUM_H_
#define _ADV_ALG_MAX_SUBARRAY_SUM_H_

#include <vector>
#include <algorithm>
/******************************************************************************
 *  Class: MaxSubarraySum
 *  A class providing multiple algorithmss for solving the max subarray sum
 *  probelm.
 ******************************************************************************/
class MaxSubarraySum
{
private:
  std::vector<int> a;

public:
  MaxSubarraySum(std::vector<int> &arr) : a(arr) {}

  int cubic_alg()
  {
    int bestSum = 0;
    for (int i = 0; i < a.size(); i++)
    {
      for (int j = i; j < a.size(); j++)
      {
        int currSum = 0;
        for (int k = i; k <= j; k++)
        {
          currSum += a[k];
        }

        if (currSum > bestSum)
        {
          bestSum = currSum;
        }
      }
    }

    return bestSum;
  }

  int quadratic_alg()
  {
    int bestSum = 0;
    for (int i = 0; i < a.size(); i++)
    {
      int currSum = 0;
      for (int j = i; j < a.size(); j++)
      {
        currSum += a[j];

        if (currSum > bestSum)
        {
          bestSum = currSum;
        }
      }
    }

    return bestSum;
  }

  int linearithmic_alg(int left, int right)
  {
    if (left == right)
    {
      return a[left] > 0 ? a[left] : 0;
    }

    int middle = (left + right) / 2;

    int lSum = linearithmic_alg(left, middle);
    int rSum = linearithmic_alg(middle + 1, right);

    int bestLeftSum = 0, currLeftSum = 0;
    for (int i = middle; i >= left; i--)
    {
      currLeftSum += a[i];
      if (currLeftSum > bestLeftSum)
      {
        bestLeftSum = currLeftSum;
      }
    }

    int bestRightSum = 0, currRightSum = 0;
    for (int i = middle + 1; i <= right; i++)
    {
      currRightSum += a[i];
      if (currRightSum > bestRightSum)
      {
        bestRightSum = currRightSum;
      }
    }

    return std::max(lSum, std::max(rSum, bestLeftSum + bestRightSum));
  }

  int linearithmic_alg()
  {
    return linearithmic_alg(0, a.size() - 1);
  }

  int linear_alg()
  {
    int bestSum = 0, currSum = 0;
    ;
    for (int i = 0; i < a.size(); i++)
    {
      currSum = std::max(0, currSum + a[i]);
      bestSum = std::max(bestSum, currSum);
    }
    return bestSum;
  }
};

#endif