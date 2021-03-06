#include "../lib/Solution.h"

#include <algorithm>
#include <vector>

using std::swap;
using std::vector;

class SolutionPLS : public Solution
{
  public:
    int n_facs, n_objs;
    bool visited; // true if this solution was explored; false otherwise
    int last_i, last_j; // Indicates the last movement made in the solution
    vector<int> deltas; // stores the variation of objs at the previous step of the iteration

    SolutionPLS()
    {
        visited = false;
        last_i = last_j = -1;
        n_facs = n_objs = 0;
    }
    SolutionPLS(int n_f, int n_o)
    {
        n_facs = n_f;
        n_objs = n_o;
        permutation.resize(n_facs);
        objs.resize(n_objs);
        visited = false;
        last_i = last_j = -1;
        deltas.resize(n_objs,0);
    }
    SolutionPLS(SolutionPLS &sol, int l_i, int l_j, vector<int> &_deltas)
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

        deltas = sol.deltas;

        visited = false;
    }

    SolutionPLS & operator=(SolutionPLS &s)
    {
        permutation = s.permutation;
        objs = s.objs;
        n_facs = s.n_facs;
        n_objs = s.n_objs;
        last_i = s.last_i;
        last_j = s.last_j;
        deltas = s.deltas;
        visited = s.visited;
        return *this;
    }
    
};

/*bool operator<=(SolutionPLS &s1, SolutionPLS &s2)
{
    for (int i = 0; i < s1.objs.size(); ++i)
    {
        if(s1.objs[i] > s2.objs[i])
            return false;
    }
    return true;
}

bool operator==(SolutionPLS &s1, SolutionPLS &s2)
{
    if (s1.objs != s2.objs) return false;

    return (s1.permutation == s2.permutation);
}

bool incomparable(SolutionPLS &s1, SolutionPLS &s2)
{
    return !(s1 <= s2) and !(s2 <= s1);
}*/