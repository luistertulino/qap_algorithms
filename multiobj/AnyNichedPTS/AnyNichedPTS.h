#ifndef AnyNichedPTS_H_
#define AnyNichedPTS_H_

#define DISCARDED 0

#include "../lib/definitions.h"
#include "SolutionTS.h"

#include <list>
#include <vector>
#include <utility> // for std::pair

using std::list;
using std::vector;
using std::pair;

typedef pair<double,int> time_eval;

class AnyNichedPTS
{
  public:
    int n_facs, n_objs;
    DistMatrix *distances;
    FlowMatrices *flows;
    int size_arc_init
    double refset_size; // In percentage
    int size_arc_init;
  public:
  	AnyNichedPTS(int n_f, int n_o, DistMatrix *dist, FlowMatrices *_flows, 
      int refset_size, int size_arc_init) /* >>>>>> Change it later to receive a struct with all parameters <<<<<< */
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

    void random_solution(SolutionTS &s);

    void random_archive(vector<SolutionTS*> &archive, int size_arc, vector<int> &non_visited);

    int select_solution(vector<SolutionTS*> &archive, vector<int> &non_visited,
                        vector<long> &min_objs, vector<long> &max_objs);

    double dij(SolutionTS &s1, SolutionTS &s2, vector<long> &min_objs, vector<long> &max_objs);

    time_eval init(bool num_avals_crit, int max_num_avals, float time_limit,
            vector<SolutionTS*> &non_dominated);
        // Main procedure
        /*
          Parameters:
            - num_avals_crit -> indicates if the stopping criterium is the number of obj avaliations
            - num_avals      -> number of avaliations of obj allowed before method ending. It matters only when num_avals_crit = true
            - time_limit     -> time limit for the execution of the algorithm
            - If num_avals_crit = false or time_limit = 0, 
                the stopping criterium is only the emptiness of the non-visited solutions set
        */

    bool update_nondom_set(SolutionTS *solution, vector<SolutionTS*> &non_dominated);
        // Update the non-dominated set
};

#endif