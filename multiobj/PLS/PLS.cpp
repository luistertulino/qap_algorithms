#include "PLS.h"

#define BEST_IMPROVEMENT  1
#define FIRST_IMPROVEMENT 2

#include <list>
#include <vector>
#include <random>

using std::list;
using std::vector;

std::random_device rand_dev;
std::mt19937 gen(rand_dev());

void random_solution(SolutionPLS &s)
{
    // Just for now - CHANGE LATER!!!

    for (int i = 0; i < s.n_facs; ++i)
    {
        s.permutation[i] = i;
    }

    // SHUFFLE

    s.compute_objs();
}


int PLS::init(bool num_avals_crit, int max_num_avals, float time_limit,
              vector<SolutionPLS> &non_dominated)
{
    SolutionPLS s(n_facs, n_objs);
    random_solution(s);

    vector<int> non_visited;
    non_visited.push_back(0);
    // non_visited contains the indexes of non_dominated which were not visited yet

    non_dominated.push_back(s);

    int num_avals = 0;

    time_t begin;
    time(&begin);

    time_t now;
    time(&now);

    bool all_visited = false;
    int rule = FIRST_IMPROVEMENT;

    while(!non_visited.empty() 
          or 
          ( num_avals_crit and num_avals < max_num_avals ) // The number of evaluations of objs is only considered when num_avals_crit is set to true
          or
          ( time_limit > 0 and difftime(now,begin) < time_limit ) // The elapsed time is only considered when the parameter is > 0
          )
    {
        // Select a random solution from non_visited to be examined
        std::uniform_int_distribution<int> rand(0, last(non_visited));
        int sol_index = rand(gen);

        // Explore the neighborhood
        /*
          At the beginning, the first-improvement ruled is employed, to quickly find local optima
          When all solutions in non_dominated are marked as visited, we switch to the best-improvement rule,
          and the mark all solutions to non-visited.
        */
        if (rule == FIRST_IMPROVEMENT)
        {
            bool found = false;
            while(not found)
            {
                for (int i = 0; i < n_facs; ++i)
                {
                    for (int j = i+1; j < n_facs; ++j)
                    {
                        /* code */
                    }
                }
            }
        }
        else if (rule == BEST_IMPROVEMENT)
        {
            /* code */
        }

        for (int i = 0; i < n_facs; ++i)
        {
            for (int j = i+1; j < n_facs; ++j)
            {
                compute_objs(*solution, i, j, new_objs, distances, flows);

                if (!(solution->objs <= new_objs) or solution->objs == new_objs)
                {
                    /* 
                        We only try to update the non-dominated set 
                        if the new "solution" is not dominated by the current solution 
                    */
                    if (try_update_nondom_set(new_objs, non_dominated))
                    {
                        // It was possible to add the new solution to the non-dominated set
                        // The new solution is added to the non-visited solutions set

                        non_visited.emplace_back(*solution, i, j, new_objs);
                    }
                }
            }
        }
    }
}

bool PLS::try_update_nondom_set(vector<int> &new_objs, list<Solution> &non_dominated){
    auto sol = non_dominated.begin();

    while(sol != non_dominated.end()){

        if (s <= (*sol)) 
        {
            if (s.objs != sol->objs)
            {
                // In this case, s dominates sol, and sol must me removed from the set
                sol = non_dominated.erase(sol);
            }
        }
        else if ((*sol) <= s)
        {
            // In this case, s is dominated by sol, and then must be discarded
            return DISCARDED;
        }
    }
}