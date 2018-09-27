#ifndef PLS_H_
#define PLS_H_

#define DISCARDED 0

#include "definitions.h"
#include "Solution.h"

#include <list>

using std::list;

class PLS
{
  public:
    int n_facs, n_objs;
  public:
  	PLS(int n_f, int n_o){
        n_facs = n_f;
        n_objs = n_o;
    }
  	~PLS();

    int init(DistMatrix &distances, FlowMatrices &flows, bool num_avals_crit, int max_num_avals, list<Solution> &non_dominated);
        // Main procedure
        /*
          Parameters:
            - num_avals_crit -> indicates if the stopping criterium is the number of obj avaliations
            - num_avals      -> number of avaliations of obj allowed before method ending. It matters only when num_avals_crit = true
            - If num_avals_crit = false, the stopping criterium is only the emptiness of the non-visited solutions set
        */

    void compute_objs(Solution &s, int i, int j, vector<int> &new_objs; DistMatrix &distances, FlowMatrices &flows);
        // Function to quickly compute the objs of a new solution created by 2-opt neighborhood

    int create_neighborhood(Solution &s);
        // Given a solution, generate its neighbors, based on 2-opt neighborhood

    bool try_update_nondom_set(vector<int> &new_objs, list<Solution> &non_dominated);
        // Checks if a solution domintes, is dominated or non-dominated
 
};

#endif