#ifndef PLS_H_
#define PLS_H_

#include "definitions.h"
#include "Solution.h"

#include <list>

using std::list;

class PLS
{
  public:
  	PLS();
  	~PLS();

    int init(bool num_avals_crit, int num_avals, list<Solution> &non_dominated);
        // Main procedure
        /*
          Parameters:
            - num_avals_crit -> indicates if the stopping criterium is the number of obj avaliations
            - num_avals      -> number of avaliations of obj allowed before method ending. It matters only when num_avals_crit = true
            - If num_avals_crit = false, the stopping criterium is only the emptiness of the non-visited solutions set
        */

    int create_neighborhood(Solution &s);
        // Given a solution, generate its neighbors, based on 2-opt neighborhood

    int non_dominance_set(Solution &s, list<Solution> &non_dominated);
        // Checks if a solution domintes, is dominated or non-dominated
 
};

#endif