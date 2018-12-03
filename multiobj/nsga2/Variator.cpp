#include "Variator.h"

#include <algorithm>
#include <vector>
#include <list>
#include <iostream>
#include <random>

using std::swap;
using std::vector;
using std::list;

std::random_device rand_dev;
std::mt19937 gen(rand_dev());

void Variator::init(vector<Individual*> &non_dominated)
{
    /* ---------------------------------------------*/
    /* --------- GENERATE RANDOM SOLUTIONS ---------*/
    /* ---------------------------------------------*/
    vector<Individual*> population, offspring;
    random_pop(population, non_dominated);
}

void Variator::random_pop(vector<Individual*> &pop, vector<Individual*> &external_archive)
{
    list<Individual*> dominated;
    for (int i = 0; i < generated_solutions_num; ++i)
    {
        Individual *ind = new Individual(n_facs, n_objs);
        ind->randomize(gen, *distances, *flows);

        update_nondom_set(ind, external_archive);
    }
}

bool Variator::update_nondom_set(Individual *ind, 
                                 vector<Individual*> &non_dominated, list<Individual*> &dominated)
{
    for (int i = 0; i < non_dominated.size(); ++i)
    {        
        if ( *ind <= *(non_dominated[i]) )
        {   // The new individual dominates someone in the set: remove this one.
            Individual *rem = non_dominated[i];            
            swap(non_dominated[i], non_dominated.back());
            non_dominated.pop_back();
            rem->position_in_external_archive = NOT_IN_SET;

            if (rem->index_in_population == NOT_IN_SET)
            {
                delete rem; rem = NULL;
            }
            i--;            
        }
        else if( *non_dominated[i] <= *ind )
        {
            ind->position_in_external_archive = NOT_IN_SET;
            return false;
        }
        else if (*ind == *non_dominated[i])
        {
            ind->position_in_external_archive = non_dominated[i]->position_in_external_archive;
            return false; // This solution is already in the set. Return false to indicate this.
        }
            
    }    
    
    non_dominated.push_back(ind);
    ind->position_in_external_archive = last(non_dominated);
    return true;
}
