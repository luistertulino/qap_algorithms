#include "PLS.h"

#define BEST_IMPROVEMENT  1
#define FIRST_IMPROVEMENT 2

#include <list>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <utility> // for std::pair

#include <chrono>
#include <thread>

using std::list;
using std::vector;
using std::swap;
using std::pair;

std::random_device rand_dev;
std::mt19937 gen(rand_dev());

void PLS::random_solution(SolutionPLS &s)
{
    for (int i = 0; i < s.n_facs; ++i)
        s.permutation[i] = i;

    std::random_shuffle(s.permutation.begin(), s.permutation.end());

    s.compute_objs(*distances, *flows);
}

bool check_dominance(SolutionPLS *solution, vector<SolutionPLS*> &non_dominated_set);


time_eval PLS::init(bool num_avals_crit, int max_num_avals, float time_limit,
              vector<SolutionPLS*> &non_dominated)
{
    SolutionPLS *s = new SolutionPLS(n_facs, n_objs);
    random_solution(*s);

    vector<int> non_visited;
    non_visited.push_back(0);
    // non_visited contains the indexes of non_dominated which were not visited yet

    non_dominated.push_back(s);

    int num_avals = 0;

    time_t begin, now;
    time(&begin);
    
    int rule = FIRST_IMPROVEMENT;

    while(!non_visited.empty() 
          and 
          ( num_avals_crit ? num_avals < max_num_avals : true ) // The number of evaluations of objs is only considered when num_avals_crit is set to true
          and
          ( time_limit > 0 and difftime(time(&now),begin) < time_limit ) // The elapsed time is only considered when the parameter is > 0
          )
    {
        // Select a random solution from non_visited to be examined
        std::uniform_int_distribution<int> rand(0, last(non_visited));
        int r = rand(gen);
        int sol_index = non_visited[r];
        swap(non_visited[r], non_visited.back()); non_visited.pop_back(); // Remove that solution index from non_visited

        SolutionPLS *curr = non_dominated[sol_index];

        // Explore the neighborhood
        /*
          At the beginning, the first-improvement ruled is employed, to quickly find local optima
          When all solutions in non_dominated are marked as visited, we switch to the best-improvement rule,
          and the mark all solutions to non-visited.
        */
        bool found = false;
        vector<int> deltas(n_objs, 0);

        list<SolutionPLS*> candidates;
        
        for (int i = 0; i < n_facs and not found; ++i)
        {
            for (int j = i+1; j < n_facs and not found; ++j)
            {
                SolutionPLS *new_sol = new SolutionPLS(*curr, i, j, deltas);
                new_sol->compute_objs(*distances, *flows); //new_sol->print();
                num_avals++;

                if (*curr <= *new_sol)
                {
                    // If the new solution is dominated by the current one, it is discarded
                    delete new_sol;
                    continue;
                }
                else if (not check_dominance(new_sol, non_dominated))
                {
                    // The new solution is dominated by someone in the non_dominated set,
                    // and must be discarded
                    delete new_sol;
                    continue;
                }
                else
                {
                    candidates.push_back(new_sol);

                    if (rule == FIRST_IMPROVEMENT) found = true;                    
                }
            }
        }

        curr->visited = true;

        for (auto sol : candidates)
            update_nondom_set(sol, non_dominated);

        non_visited.clear();
        for (int i = 0; i < non_dominated.size(); ++i)
        {
            if (not non_dominated[i]->visited)
                non_visited.push_back(i);
        }

        if (non_visited.empty() and rule == FIRST_IMPROVEMENT)
        {
            // Switch to the best-improvement rule            
            rule = BEST_IMPROVEMENT;
            for (int i = 0; i < non_dominated.size(); ++i)
            {
                non_dominated[i]->visited = false;
                non_visited.push_back(i);
            }
        }
    }

    time_eval p(difftime(time(&now),begin), num_avals);
    return p;
}

bool check_dominance(SolutionPLS *solution, vector<SolutionPLS*> &non_dominated_set)
{
    for (auto sol : non_dominated_set)
    {
        if (*sol <= *solution)
            return false;
    }
    return true;
}

void PLS::compute_deltas(SolutionPLS &sol, int i, int j, vector<int> &deltas)
{
    if (i != sol.last_i and i != sol.last_j
        and
        j != sol.last_i and j != sol.last_j
        and
        sol.last_i != -1 and sol.last_j != -1)
    {
        for (int k = 0; k < n_objs; ++k)
        {
            deltas[k] = sol.deltas[k];

            int flow_mod = (*flows)[k][i][sol.last_i] - (*flows)[k][i][sol.last_j] + 
                           (*flows)[k][j][sol.last_j] - (*flows)[k][j][sol.last_i];

            int dist_mod = (*distances)[sol.permutation[j]][sol.permutation[sol.last_i]] - 
                           (*distances)[sol.permutation[j]][sol.permutation[sol.last_j]] + 
                           (*distances)[sol.permutation[i]][sol.permutation[sol.last_j]] - 
                           (*distances)[sol.permutation[i]][sol.permutation[sol.last_i]];

            deltas[k] += 2 * flow_mod * dist_mod;
        }
    }
    else
    {
    	for (int k = 0; k < n_objs; ++k)
    	{
            deltas[k] = 0;
    		for (int l = 0; l < n_facs; ++l)
    		{
    			if (l != i and l != j)
    			{
    				int flow_mod = (*flows)[k][l][i] - (*flows)[k][l][j];

                    int dist_mod = (*distances)[sol.permutation[l]][sol.permutation[j]] - 
                                   (*distances)[sol.permutation[l]][sol.permutation[i]];

                    deltas[k] += 2 * flow_mod * dist_mod;
    			}
    		}
    	}
    }
}

bool PLS::update_nondom_set(SolutionPLS *solution, vector<SolutionPLS*> &non_dominated)
{
    for (int i = 0; i < non_dominated.size(); ++i)
    {
        if (solution->permutation == non_dominated[i]->permutation)
        {
            // This solution is already in the set. Return false to indicate this.
            return false;
        }
        else if ( *solution <= *(non_dominated[i]) )
        {
            // The new solution dominates someone in the set: remove this one.
            SolutionPLS *rem = non_dominated[i];
            swap(non_dominated[i], non_dominated.back());
            non_dominated.pop_back();
            delete rem;

            i--;
        }
    }
    
    non_dominated.push_back(solution);
    return true;
}