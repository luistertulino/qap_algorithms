#include "Variator.h"

#include <algorithm>
#include <vector>
#include <list>
#include <string>
#include <random>

// For output and files
#include <iostream>
#include <sstream>
#include <fstream>

#include <stdlib.h> // for system

// For sleep
#include <chrono>
#include <thread>

using std::swap;
using std::vector;
using std::list;
using std::string;

std::random_device rand_dev;
std::mt19937 gen(rand_dev());

void print_population(vector<Individual*> &population)
{
    std::cout << "Print population:\n";
    for(auto ind : population)
    {
        ind->print();
    }
    
}

int Variator::init(vector<Individual*> &non_dominated)
{
    /* ---------------------------------------------*/
    /* --------- GENERATE RANDOM SOLUTIONS ---------*/
    /* ---------------------------------------------*/
    vector<Individual*> population, offspring;
    random_pop(population, non_dominated);
    //print_population(population);

    /* --------------------------------------------- */
    /* -------- SETUP SELECTOR ENVIRONMENT --------- */
    /* --------------------------------------------- */
    int result = set_env(population);
    if(result == ERROR_IN_FILE)
    {
        std::cerr << "Error in creating environment.\n";
        return ERROR_IN_FILE;
    }

    system("./selector/nsga2 selector/env/nsga2_param.txt selector/env/PISA_ 0.25");
    /* --------------------------------------------- */
    /* ---------------- MAIN LOOP ------------------ */
    /* --------------------------------------------- */
    int state = 1;
    for(int i = 0; i < num_iter; i++)
    {
        while(state != 2)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            state = read_state("selector/env/PISA_sta");
            if(state == -1) return -1;            
        }

        /* Update population:
            crie um novo array, new_population
            remova da população anterior e dos filhos os indivíduos selecionados para a nova população e insira-os em new_population
            para os indivíduos restantes:
                destrua-os se não estiverem no arquivo externo
            faça population = new_population
        */

        // Apply crossover and mutation on selected individuals
    }
    
    system("rm -r selector/env/");

}

int Variator::set_env(vector<Individual*> &population)
{
    // Create environment folder and files
    system("mkdir selector/env/");

    std::ofstream cfg;
    cfg.open("selector/env/PISA_cfg", std::ofstream::out);    
    if (cfg.is_open())
    {
        cfg << "alpha " << pop_size << "\n";
        cfg << "mu " << n_facs << "\n";
        cfg << "lambda " << n_facs << "\n";
        cfg << "dim " << n_objs << "\n";
        cfg.close();
    }
    else
    {
        std::cerr << "Error in creating and opening selector/env/PISA_cfg file.\n";
        return ERROR_IN_FILE;
    }

    std::ofstream params;
    params.open("selector/env/nsga2_param.txt", std::ofstream::out);
    if(params.is_open())
    {
        params << "seed " << gen() << "\n";
        params << "tournament " << 2 << "\n";
        params.close();
    }
    else
    {
        std::cerr << "Error in creating and opening selector/env/PISA_cfg file.\n";
        return ERROR_IN_FILE;
    }

    std::ofstream ini;
    ini.open("selector/env/PISA_ini", std::ofstream::out);
    if(ini.is_open())
    {
        ini << (pop_size * (n_objs+1)) << "\n";
        for(auto ind : population)
        {
            ini << ind->index_in_population << " ";            
            for(int i = 0; i < n_objs; i++)
                ini << ind->objs[i] << " ";
            ini << "\n";            
        }
        ini << "END";
        ini.close();
    }
    else
    {
        std::cerr << "Error in creating and opening selector/env/PISA_ini file.\n";
        return ERROR_IN_FILE;
    }

    std::ofstream state;
    state.open("selector/env/PISA_sta", std::ofstream::out);
    if(state.is_open())
    {
        state << 1 << "\n";
        state.close();
    }
    else
    {
        std::cerr << "Error in creating and opening selector/env/PISA_sta file.\n";
        return ERROR_IN_FILE;
    }

    return OK;
}

void Variator::random_pop(vector<Individual*> &pop, vector<Individual*> &external_archive)
{
    list<Individual*> dominated;
    for (int i = 0; i < generated_solutions_num; ++i)
    {
        //std::cout << "i = " << i << "\n";
        Individual *ind = new Individual(n_facs, n_objs);
        ind->randomize(gen, *distances, *flows);
        //ind->print();

        if(not update_nondom_set(ind, external_archive, dominated) and ind->position_in_external_archive == NOT_IN_SET)
            // This solution was not inserted in archive because it is dominated
            dominated.push_back(ind);
    }

    /* -----------------------------------
       ------ CONSTRUCT POPULATION -------
       -----------------------------------
       Give priority to non-dominated solutions.
       If the number of non-dominated solutions is less than or equal to the population size,
       put those solutions in population and fill the remaning capacity with dominated solutions
       Otherwise, just put the first pop_size solutions of the archive in population
    */
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

int Variator::read_state(string &state_file)
{
    std::ifstream state;
    state.open(state_file);
    if(state.is_open())
    {
        int sta;
        state >> sta;
        return sta;
    }
    else
    {
        std::cerr << "Error in reading " << state_file << " in variator.\n";
        return -1;
    }
}
