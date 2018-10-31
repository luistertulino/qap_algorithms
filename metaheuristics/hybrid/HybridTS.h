#ifndef HYBRIDTS_H_
#define HYBRIDTS_H_

#include "../lib/definitions"
#include <vector>

using std::vector;

struct solution
{
    vector<int> p; // Permutation of QAP. p[i] = j indicates that item i is allocated on place j
    long cost; // Cost of that solution
};

class HybridTS
{
    int n_facs;
    int min_tabu_list;
    int delta_tabu;
    Matrix distances, flows;
    Matrix tabu_list;
    MatrixLong delta;

    HybridTS(prob_params &params, Matrix *dist, Matrix *flow)
    {
        n_facs = params.n_facs;
        min_tabu_list = params.min_tabu_list;
        delta_tabu = params.delta;
        distances = *dist;
        flows = *flow;

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
                tabu_list[i][j] = - min_tabu_list;
            }
        }
    }

    int init();
    bool isTabu(int i, int j, solution &s, int it);
    void make_tabu(int i, int j, int it);
    long compute_delta(int i, int j, solution &s);
    long compute_delta(int i, int j, int r, int s, solution &s);
    
};

#endif