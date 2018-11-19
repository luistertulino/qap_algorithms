#ifndef HYBRIDTS_H_
#define HYBRIDTS_H_

#include "solution.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

using std::vector;
using std::string;

class HybridTS
{
    solution best;
    int iteration_found; // Iteration when the solution from qaplib (or better) was found

  public:
    int n_facs;        // Problem size
    int min_tabu_list; // Minimun tabu list size
    int delta_tabu;    // Maximum variation of tabu list size
    int max_fails;     // Maximum number of fails in improving best solution: when an improvement is not made, the simulated annealing is used
    int threshold;     // The minimum problem size to use second aspiration function
    int aspiration;    // The extra iterations of second aspiration function
    long qaplib_sol;    // Solution value available in solution file from QAPLIB
    Matrix distances, flows;
    Matrix tabu_list;
    MatrixLong delta;

  public:
    HybridTS(prob_params &params, Matrix &dist, Matrix &flow)
    {
        n_facs = params.n_facs;
        min_tabu_list = params.min_tabu_list;
        delta_tabu = params.delta;
        max_fails = params.max_fails;
        threshold = params.threshold;
        aspiration = params.aspiration;
        qaplib_sol = params.qaplib_sol;
        distances = dist;
        flows = flow;

        tabu_list.resize(n_facs);
        delta.resize(n_facs);
        for(int i = 0; i < n_facs; i++)
        {
            tabu_list[i].resize(n_facs);
            delta[i].resize(n_facs);
        }

        iteration_found = -1;
        best.resize(n_facs);
    }

    void init();
    bool isTabu(int i, int j, solution &s, int it);
    void make_tabu(int i, int j, solution &s, int curr_tabu, int it);
    long compute_delta(int i, int j, solution &sol);
    long compute_delta(int i, int j, int r, int s, solution &sol);
    void update_delta_matrix(int i, int j, solution &s);

    void write_results(string &instance);
};

#endif