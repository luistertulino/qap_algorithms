#ifndef HYBRIDTS_H_
#define HYBRIDTS_H_

#include "../lib/definitions.h"
#include <vector>
#include <algorithm>
#include <iostream>

using std::vector;

struct solution
{
    int n_facs;
    vector<int> p; // Permutation of QAP. p[i] = j indicates that item i is allocated on place j
    long cost; // Cost of that solution

    solution(int n)
    {
        p.resize(n);
        n_facs = n;
        for(int i = 0; i < n; i++)
        {
            p[i] = i;
        }
        cost = 0;
    }

    void shuffle()
    {
        std::random_shuffle(p.begin(), p.end());
    }

    void print()
    {
        std::cout << "Permutation: ";
        for (int i = 0; i < n_facs; ++i)
        {
            std::cout << i << "->" << p[i] << " ";
        }
        std::cout << "\n";
    }
};

class HybridTS
{
  public:
    int n_facs;
    int min_tabu_list;
    int delta_tabu;
    Matrix distances, flows;
    Matrix tabu_list;
    MatrixLong delta;

  public:
    HybridTS(prob_params &params, Matrix &dist, Matrix &flow)
    {
        n_facs = params.n_facs;
        min_tabu_list = params.min_tabu_list;
        delta_tabu = params.delta;
        distances = dist;
        flows = flow;

        tabu_list.resize(n_facs);
        delta.resize(n_facs);
        for(int i = 0; i < n_facs; i++)
        {
            tabu_list[i].resize(n_facs);
            delta[i].resize(n_facs);
        }

        // Initialize tabu list
        for(int i = 0; i < n_facs; i++)
        {
            for(int j = 0; j < n_facs; j++)
            {
                tabu_list[i][j] = -min_tabu_list;
            }
        }
    }

    int init();
    bool isTabu(int i, int j, solution &s, int curr_tabu, int it);
    void make_tabu(int i, int j, solution &s, int it);
    long compute_delta(int i, int j, solution &sol);
    long compute_delta(int i, int j, int r, int s, solution &sol);
    
};

#endif