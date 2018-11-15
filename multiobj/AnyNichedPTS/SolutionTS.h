#ifndef SOLUTIONTS_H_
#define SOLUTIONTS_H_

#define CASE_ASPIRED 1
#define CASE_ANYWAY 2

#include "../lib/Solution.h"

#include <algorithm>
#include <vector>

using std::swap;
using std::vector;

class SolutionTS : public Solution
{
  public:
    int n_facs, n_objs;
    bool visited; // true if this solution was explored; false otherwise
    int last_i, last_j; // Indicates the last movement made in the solution
    int status; // if 1, that solution is at least non-dominated;
                // if 2, that solution enters in the archive anyway
    double niche_count;

    SolutionTS()
    {
        visited = false;
        last_i = last_j = -1;
        n_facs = n_objs = 0;
        niche_count = 1;
    }
    SolutionTS(int n_f, int n_o)
    {
        n_facs = n_f;
        n_objs = n_o;
        permutation.resize(n_facs);
        objs.resize(n_objs);
        visited = false;
        last_i = last_j = -1;
        status = -1;
        niche_count = 1;
    }
    SolutionTS(SolutionTS &sol, int l_i, int l_j, int _status)
    {
        // Creates a new solution based in a solution, the movement made and the variation on objs
        n_facs = sol.n_facs;
        n_objs = sol.n_objs;
        last_i = l_i;
        last_j = l_j;
        permutation = sol.permutation;
        swap(permutation[l_i], permutation[l_j]);
        objs = sol.objs;
        for (int k = 0; k < n_objs; ++k)
        {
            objs[k] += _deltas[k];
        }
        visited = false;
        status = _status;
        niche_count = sol.niche_count;
    }

    SolutionTS & operator=(SolutionTS &s)
    {
        permutation = s.permutation;
        objs = s.objs;
        n_facs = s.n_facs;
        n_objs = s.n_objs;
        last_i = s.last_i;
        last_j = s.last_j;
        visited = s.visited;
        status = s.status;
        sol.niche_count;
        return *this;
    }
    
};

#endif