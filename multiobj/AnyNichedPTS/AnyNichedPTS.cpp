#include "AnyNichedPTS.h"

#define BEST_IMPROVEMENT  1
#define FIRST_IMPROVEMENT 2

#include <list>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <utility> // for std::pair
#include <cmath>
#include <numeric>
#include <climits> 

#include <chrono>
#include <thread>

using std::list;
using std::vector;
using std::swap;
using std::pair;

std::random_device rand_dev;
std::mt19937 gen(rand_dev());

#define DOMINATED -1
int check_dominance(SolutionTS *solution, vector<SolutionTS*> &non_dominated_set);

time_eval AnyNichedPTS::init(bool num_avals_crit, int max_num_avals, float time_limit,
              vector<SolutionTS*> &non_dominated)
{
    vector<long> min_objs(n_objs, LONG_MAX);
    vector<long> max_objs(n_objs, 0);

    // Initialize tabu list
    for(int i = 0; i < n_facs; i++)
        for(int j = 0; j < n_facs; j++) tabu_list[i][j] = 0;

    // Step 1: generate a random archive and remove the dominated solutions
    vector<int> non_visited; // Indexes of non-visited solutions
    random_archive(non_dominated, size_arc_init, non_visited, min_objs, max_objs);

    int num_avals = 0;

    time_t begin, now;
    time(&begin);
    
    int rule = FIRST_IMPROVEMENT;

    std::uniform_int_distribution<int> distribution(min_tabu_list, min_tabu_list+delta_tabu);
    int curr_tabu = distribution(gen); // Current size of tabu list

    list<SolutionTS*> candidates; // For neighbor solutions

    while(!non_visited.empty() 
          and 
          ( num_avals_crit ? num_avals < max_num_avals : true ) // The number of evaluations of objs is only considered when num_avals_crit is set to true
          and
          ( time_limit > 0 and difftime(time(&now),begin) < time_limit ) // The elapsed time is only considered when the parameter is > 0
          )
    {
        num_iter++;
        // Randomly select the size of tabu list at each s_max iterations, where s_max = minimal tabu list size + delta
        // Inspired by Robust Tabu Search
        if(num_iter % (2*(min_tabu_list+delta_tabu)) == 0) curr_tabu = distribution(gen);        

        // Select a random solution based on niche count from non_visited to be examined
        int nv = select_solution(non_dominated, non_visited, min_objs, max_objs);
        int sol_index = non_visited[nv];
        swap(non_visited[nv], non_visited.back()); non_visited.pop_back(); // Remove that solution index from non_visited

        SolutionTS *curr = non_dominated[sol_index];

        // Explore the neighborhood
        /*
          At the beginning, the first-improvement ruled is employed, to quickly find local optima
          When all solutions in non_dominated are marked as visited, we switch to the best-improvement rule,
          and the mark all solutions to non-visited.
        */
        bool found = false;

        /* ------------ Explore neighborhood of current solution ------------ */
        for (int i = 0; i < n_facs and not found; ++i)
        {
            for (int j = i+1; j < n_facs and not found; ++j)
            {
                if(i == curr->last_i and j == curr->last_j) continue;

                SolutionTS *new_sol = new SolutionTS(*curr, i, j);
                new_sol->compute_objs(*distances, *flows); //new_sol->print();
                for(int j = 0; j < n_objs; j++) // Update min e max objs
                {
                    min_objs[j] = std::min(min_objs[j], s->objs[j]);
                    max_objs[j] = std::max(max_objs[j], s->objs[j]);
                }
                num_avals++;
                
                int dom_count = check_dominance(new_sol, non_dominated);
                if(rule == FIRST_IMPROVEMENT ? dom_count > 0 : dom_count >= 0)
                {   //Aspiration criteria
                    candidates.push_back(new_sol); new_sol->status = 1;
                    if (rule == FIRST_IMPROVEMENT) found = true;
                }
                else if(not isTabu(i, j, *curr, curr_tabu))
                {
                    if(rule == FIRST_IMPROVEMENT and *new_sol <= *curr)
                    {
                        candidates.push_back(new_sol); new_sol->status = 2;
                        if (rule == FIRST_IMPROVEMENT) found = true;
                    }
                    else if(rule == BEST_IMPROVEMENT and not(*curr <= *new_sol))
                        candidates.push_back(new_sol); new_sol->status = 2;
                }
                else
                {
                    delete new_sol;
                    continue;
                }
            }
        }
        /* ------------ Explore neighborhood of current solution ------------ */

        curr->visited = true;

        for (auto sol : candidates)
        {
            /*bool updated =*/ update_nondom_set(sol, non_dominated);
            make_tabu(sol->last_i, sol->last_j, *sol, curr_tabu, num_iter);
        }
        candidates.clear();

        non_visited.clear();
        for (int i = 0; i < non_dominated.size(); ++i)
        {
            if(not non_dominated[i]->visited)
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

    // >>>>>>>>>> TO DO: REMOVE DOMINATED SOLUTIONS <<<<<<<<<<<

    time_eval p(difftime(time(&now),begin), num_avals);
    return p;
}

bool AnyNichedPTS::isTabu(int i, int j, SolutionTS &sol, int it)
{
    if( tabu_list[i][sol.p[j]] >= it and
        tabu_list[j][sol.p[i]] >= it ) return true;
    else return false;
}

void AnyNichedPTS::make_tabu(int i, int j, SolutionTS &sol, int curr_tabu, int it)
{
    tabu_list[i][sol.p[i]] = it + curr_tabu;
    tabu_list[j][sol.p[j]] = it + curr_tabu;
}

void AnyNichedPTS::random_solution(SolutionTS &s)
{
    for (int i = 0; i < s.n_facs; ++i)
        s.permutation[i] = i;

    std::shuffle(s.permutation.begin(), s.permutation.end(), gen);
    s.compute_objs(*distances, *flows);
}

void AnyNichedPTS::random_archive(vector<SolutionTS*> &archive, int size_arc, vector<int> &non_visited,
                                  vector<long> &min_objs, vector<long> &max_objs)
{
    for(int i = 0; i < size_arc; i++) // Generate random solutions
    {
        SolutionTS *s = new SolutionTS(n_facs, n_objs);
        random_solution(*s);
        for(int j = 0; j < n_objs; j++) // Update min e max objs
        {
            min_objs[j] = std::min(min_objs[j], s->objs[j]);
            max_objs[j] = std::max(max_objs[j], s->objs[j]);
        }
        if(not update_nondom_set(s, archive)) delete s;
    }
    non_visited.resize(archive.size());
    for(int i = 0; i < archive.size(); i++)
        non_visited[i] = i;
}

int AnyNichedPTS::select_solution(vector<SolutionTS*> &archive, vector<int> &non_visited,
                                  vector<long> &min_objs, vector<long> &max_objs)
{
    // Select the solutions (with uniform distribution) to construct the reference set
    int ref_size = int(archive.size()*refset_size); // refset_size is a percentage
    std::set<int> refset;
    std::uniform_int_distribution<int> rand(0, archive.size());
    for(int i = 0; i < ref_size; i++)
    {
        int r = rand(gen);
        while(refset.count(r) != 0) r = rand(gen); // The selected numbers are the index of archive solutions
        refset.insert(r);
    }

    /* -------------- For the non-visited solutions, compute their niche count -------------- */
    // Step 1: Compute d_ij for each non-visited solution
    double d_ijs[non_visited.size()*ref_size];

    int i = 0;
    for(auto nv : non_visited)
    {
        int j = 0;
        for(auto ref : refset)
        {
            d_ijs[i*ref_size + j] = dij(*(archive[nv]), *(archive[ref]), min_objs, max_objs);
            j++;
        }
        i++;
    }
    double r_niche = std::accumulate(d_ijs, d_ijs+non_visited.size()*ref_size, 0.0);
    r_niche /= ref_size * (ref_size - 1);

    // Step 2: Given the d_ijs, compute the niche count for each non-visited solution
    i = 0;
    vector<double> reverse_count(non_visited.size());
    for(auto nv : non_visited)
    {
        non_dominated[nv]->niche_count = 0;
        for(int j = 0; j < ref_size; j++)
            non_dominated[nv]->niche_count += (d_ijs[i*ref_size + j] <= r_niche ? (1 - d_ijs[i*ref_size + j]/r_niche ) : 0);
        reverse_count[i] = 1.0 / ( 1.0 + non_dominated[nv]->niche_count);
    }
    /* -------------- For the non-visited solutions, compute their niche count -------------- */

    // Based on niche count, choose at random a non-visited solution (lesser count leads to higher probability)
    std::discrete_distribution<int> dist(reverse_count.begin(), reverse_count.end());
    return dist(gen);
}

double AnyNichedPTS::dij(SolutionTS &s1, SolutionTS &s2, vector<long> &min_objs, vector<long> &max_objs)
{
    double dij = 0;
    for(int i = 0; i < n_objs; i++)
        dij += std::pow( ( double(s1.objs[i] - s2.objs[i]) / ( max_objs[i] - min_objs[i] ) ), 2);
    return std::sqrt(dij);
}

int check_dominance(SolutionTS *solution, vector<SolutionTS*> &non_dominated_set)
{
    int dom_count = 0;
    for (auto sol : non_dominated_set)
    {
        if (*sol <= *solution) return DOMINATED;
        else if(*solution <= *sol) domination_count++;
    }
    return true;
}

bool AnyNichedPTS::update_nondom_set(SolutionTS *solution, vector<SolutionTS*> &non_dominated)
{
    for (int i = 0; i < non_dominated.size(); ++i)
    {        
        if (solution->permutation == non_dominated[i]->permutation)
        {
            delete solution;
            solution = NULL;
            return false; // This solution is already in the set. Return false to indicate this.
        }
        else if ( *solution <= *(non_dominated[i]) )
        {
            // The new solution dominates someone in the set: remove this one.
            SolutionTS *rem = non_dominated[i];
            swap(non_dominated[i], non_dominated.back());
            non_dominated.pop_back();
            delete rem;
            i--;
        }
        else if(solution->status == -1 and *(non_dominated[i]) <= *solution )
            return false;
            // Note that when status == 1 (aspiration), the second condition will never be satisfied
            // When status == 2, the second condition is ignored
    }    
    non_dominated.push_back(solution);
    return true;
}