#include "../lib/Solution.h"

#include <algorithm>
#include <vector>

using std::swap;
using std::vector;

class SolutionPLS : public Solution
{
  public:
    bool visited; // true if this solution was explored; false otherwise
    int last_i, last_j; // Indicates the last movement made that was made in the solution

    SolutionPLS()
    {
        visited = false;
        last_i = last_j = -1;
        n_facs = n_objs = 0;
    }
    SolutionPLS(int n_facs, int n_objs) : Solution(n_facs, n_objs)
    {
        last_i = last_j = -1;
    }
    SolutionPLS(SolutionPLS &sol, int l_i, int l_j, vector<int> &_objs)
    {
        n_facs = sol.n_facs;
        n_objs = sol.n_objs;

        last_i = l_i;
        last_j = l_j;

        permutation = sol.permutation;
        swap(permutation[i], permutation[j]);

        objs = _objs;

        visited = false;
    }
    
};