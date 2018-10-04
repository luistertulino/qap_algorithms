#ifndef PLS_H_
#define PLS_H_

#define DISCARDED 0

#include "definitions.h"
#include "SolutionPLS.h"

#include <list>
#include <vector>

using std::list;
using std::vector;

class PLS
{
  public:
    int n_facs, n_objs;
    DistMatrix *distances;
    FlowMatrices *flows;
  public:
  	PLS(int n_f, int n_o)
    {
        n_facs = n_f;
        n_objs = n_o;
    }
  	~PLS();

    int init(bool num_avals_crit, int max_num_avals, float time_limit,
            list<SolutionPLS> &non_dominated);
        // Main procedure
        /*
          Parameters:
            - num_avals_crit -> indicates if the stopping criterium is the number of obj avaliations
            - num_avals      -> number of avaliations of obj allowed before method ending. It matters only when num_avals_crit = true
            - time_limit     -> time limit for the execution of the algorithm
            - If num_avals_crit = false or time_limit = 0, 
                the stopping criterium is only the emptiness of the non-visited solutions set
        */

    void compute_objs(Solution &s, int i, int j, vector<int> &new_objs;);
        // Function to quickly compute the objs of a new solution created by 2-opt neighborhood

    int create_neighborhood(Solution &s);
        // Given a solution, generate its neighbors, based on 2-opt neighborhood

    bool try_update_nondom_set(vector<int> &new_objs, list<SolutionPLS> &non_dominated);
        // Checks if a solution domintes, is dominated or non-dominated
 
};

#endif