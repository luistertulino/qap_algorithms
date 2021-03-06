#include "Solution.h"

#include <vector>
#include <iostream>

void Solution::compute_objs(DistMatrix &dist_mat, FlowMatrices &flow_mats)
{
    int pi, pj;
    int n_objs = flow_mats.size();
    int n_facs = dist_mat.size();
    
    for (int k = 0; k < n_objs; ++k)
    {
        objs[k] = 0;
        for (int i = 0; i < n_facs; ++i)
        {
            for (int j = 0; j < n_facs; ++j)
            {
                pi = permutation[i];
                pj = permutation[j];

                objs[k] += flow_mats[k][i][j] * dist_mat[pi][pj];
            }
        }
    }
}

void Solution::print()
{
    //std::cout << "-----------------------------------------------------------\n";
    std::cout << "Permutation: ";
    for (int i = 0; i < permutation.size(); ++i)
    {
        std::cout << i << "-->" << permutation[i] << ", ";
    }
    std::cout << "\nObjs: ";
    for (int i = 0; i < objs.size(); ++i)
    {
        std::cout << objs[i] << " ";
    }
    std::cout << "\n\n";
    //std::cout << "\n-----------------------------------------------------------\n";
}

bool operator<=(Solution &s1, Solution &s2)
{
    bool is_equal = true;
    for (int i = 0; i < s1.objs.size(); ++i)
    {
        if(s1.objs[i] > s2.objs[i]) return false;
        is_equal = (s1.objs[i] == s2.objs[i]) and is_equal;
    }
    return (not is_equal);
}

bool operator==(Solution &s1, Solution &s2)
{
    if (s1.objs != s2.objs) return false;

    return (s1.permutation == s2.permutation);
}

bool incomparable(Solution &s1, Solution &s2)
{
    return ( not(s1 <= s2) and not(s2 <= s1) );
}