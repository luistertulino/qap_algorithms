#ifndef PLS_H_
#define PLS_H_

#define DISCARDED 0

#include "../lib/definitions.h"
#include "SolutionPLS.h"

#include <list>
#include <vector>
#include <utility> // for std::pair

using std::list;
using std::vector;
using std::pair;

typedef pair<double,int> time_eval;

class PLS
{
  public:
    int n_facs, n_objs;
    DistMatrix *distances;
    FlowMatrices *flows;
    vector<Matrix> deltas; // Delta matrix for each objective (based on RoTS by Eric Taillard)
  public:
  	PLS(int n_f, int n_o, DistMatrix *dist, FlowMatrices *_flows)
    {
        n_facs = n_f;
        n_objs = n_o;
        distances = dist;
        flows = _flows;

        deltas.resize(n_objs);
        for (int k = 0; k < n_objs; ++k)
        {
            deltas[k].resize(n_facs);
            for (int i = 0; i < n_facs; ++i)
            {
                deltas[k][i].resize(n_facs);
            }
        }
    }

    void random_solution(SolutionPLS &s);

    time_eval init(bool num_avals_crit, int max_num_avals, float time_limit,
            vector<SolutionPLS*> &non_dominated);
        // Main procedure
        /*
          Parameters:
            - num_avals_crit -> indicates if the stopping criterium is the number of obj avaliations
            - num_avals      -> number of avaliations of obj allowed before method ending. It matters only when num_avals_crit = true
            - time_limit     -> time limit for the execution of the algorithm
            - If num_avals_crit = false or time_limit = 0, 
                the stopping criterium is only the emptiness of the non-visited solutions set
        */

    void compute_deltas(SolutionPLS &sol, int i, int j, vector<int> &deltas);
        // Compute the variation for each objective

    bool update_nondom_set(SolutionPLS *solution, vector<SolutionPLS*> &non_dominated);
        // Update the non-dominated set
};

#endif