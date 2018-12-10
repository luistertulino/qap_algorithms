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

        if(not update_nondom_set(ind, external_archive, dominated) and ind->position_in_external_archive == NOT_IN_SET)
            // This solution was not inserted in archive because it is dominated
            dominated.push_back(ind);
    }

    if(external_archive.size() <= pop_size)
    {
        pop = external_archive;        
        auto doms = dominated.begin();        
        while(pop.size() < pop_size)
        {
            pop.push_back(*doms);
            doms++;
        }
    }
    else
    {
        auto nond = external_archive.begin();
        while(pop.size() < pop_size)
        {
            pop.push_back(*nond);
            nond++;
        }
    }

    
    for(int i = 0; i < pop.size(); i++)
        pop[i]->index_in_population = i;

    for(int i = 0; i < external_archive.size(); i++)
    {
        if(external_archive[i]->index_in_population == CONSTRUCTED)
            external_archive[i]->index_in_population = NOT_IN_SET;
    }
        
    for(auto dom = dominated.begin(); dom != dominated.end(); )
    {
        // Delete the dominated solutions that are not in population
        if ((*dom)->index_in_population == NOT_IN_SET )
        {
            delete (*dom);
            (*dom) = NULL;
            dom = dominated.erase(dom);
        }
        else dom++;
        
    }
}

bool Variator::update_nondom_set(Individual *ind, 
                                 vector<Individual*> &non_dominated)
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

            dominated.push_back(rem);
            i--;            
        }
        else if( *non_dominated[i] <= *ind )
        {
            ind->position_in_external_archive = NOT_IN_SET;
            return false;
        }
        else if (*ind == *non_dominated[i])
        {
            // This solution is already in the set. Set position... to indicate this and return false
            ind->position_in_external_archive = non_dominated[i]->position_in_external_archive;
            return false;
        }            
    }    
    
    non_dominated.push_back(ind);
    ind->position_in_external_archive = last(non_dominated);
    return true;
}
