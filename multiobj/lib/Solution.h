#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <vector>
#include <iostream>

#include "definitions.h"

class Solution
{
  public:
    std::vector<int> permutation; // permutation[i] indicates to where item i was assigned
    std::vector<int> objs;

  public:
    Solution(){}
    Solution(int n_facs, int n_objs)
    {
        permutation.resize(n_facs);
        objs.resize(n_objs);
    }
    Solution(int perm[], int _objs[], int n_facs, int n_objs)
    {
        permutation = std::vector<int>(perm, perm + n_facs);
        objs = std::vector<int>(_objs, _objs + n_objs);
    }

    Solution(Solution &s, int i, int j, std::vector<int> &new_objs)
    {
        permutation = s.permutation;

        int aux;
        aux = permutation[i];
        permutation[i] = permutation[j];
        permutation[j] = aux;

        objs = new_objs;
    }

    void compute_objs(DistMatrix &dist_mat, FlowMatrices &flow_mats);
    void print();
};

bool operator<=(Solution &s1, Solution &s2);
bool operator==(Solution &s1, Solution &s2);
bool incomparable(Solution &s1, Solution &s2);

#endif