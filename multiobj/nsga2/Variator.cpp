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

string state_path = "selector/env/PISA_sta";
string arc_path = "selector/env/PISA_arc";
string sel_path = "selector/env/PISA_sel";

void print_population(vector<Individual*> &population)
{
    std::cout << "Print population:\n";
    for(auto ind : population) ind->print();
    
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
            state = read_state(state_path);
            if(state == -1) return -1;            
        }

        /* Update population:
            crie um novo array, new_population
            remova da população anterior e dos filhos os indivíduos selecionados para a nova população e insira-os em new_population
            para os indivíduos restantes:
                destrua-os se não estiverem no arquivo externo
            faça population = new_population
        */
        /* ----------- READ NEW POPULATION ----------- */
        vector<Individual*> new_population;
        result = read_new_pop(arc_path, new_population, population, offspring);
        if(result == -1)
        {
            std::cerr << "Error in reading new population.\n";
            return ERROR_IN_FILE;
        }

        /* ----------- UPDATE POPULATION ----------- */
        update_population(new_population, population, offspring);
            // After this method, new_population and offspring are empty

        vector<int> selected_for_crossover;
        read_selected(sel_path, selected_for_crossover, pop)
        // Apply crossover and mutation on selected individuals
    }
    
    //system("rm -r selector/env/");

}

void Variator::remove_individual(int position, vector<Individual*> &set)
{
    Individual *rem = set[i];
    swap(set[i], set.back());
    set.pop_back();
    delete rem;
    rem = NULL;
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
        cfg << "mu " << num_parents << "\n";
        cfg << "lambda " << offspring_size << "\n";
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

int Variator::read_new_pop(string &arc_file, vector<Individual*> &new_pop, 
                           vector<Individual*> &curr_pop, vector<Individual*> &curr_offspring)
{
    std::ifstream pop;
    pop.open(arc_file);
    if(not pop.is_open())
    {
        std::cerr << "Error in reading " << arc_file << " in variator.\n";
        return -1;
    }
    int new_pop_size;
    pop >> new_pop_size;    
    if (new_pop_size != pop_size)
    {
        std::cerr << "Error in reading new_pop_size in " << arc_file << " in variator.\n";
        return -1;
    }
    int new_pop_indexes[new_pop_size];    
    for(int i = 0; i < pop_size; i++)
        pop >> new_pop_indexes[i];
    
    string end; pop >> end;
    if(end != "END")
    {
        std::cerr << "Error in reading END in " << arc_file << " in variator.\n";
        return -1;
    }
        
    for(int i = 0; i < new_pop_size; i++)
    {        
        if (new_pop_indexes[i] < pop_size)
        {   // In this case, the individual selected to new population is in the current population
            new_pop.push_back( curr_pop[ new_pop_indexes[i] ] );
        }
        else new_pop.push_back( curr_offspring[ new_pop_indexes[i] - pop_size ] );
        new_pop[i]->selected_to_new_pop = true;
    }
    return OK;
}

int Variator::read_selected(string &sel_file, vector<int> &selected)
{
    std::ifstream sel;
    sel.open(sel_file);
    if(not sel.is_open())
    {
        std::cerr << "Error in reading " << sel_file << " in variator.\n";
        return -1;
    }
    int num_sel; sel >> num_sel;
    if(num_sel != num_parents)
    {
        std::cerr << "Error in reading num_parents in " << sel_file << " in variator.\n";
        return -1;
    }
    selected.resize(num_sel);
    for(int i = 0; i < num_sel; i++)
        sel >> selected[i];
    
    string end; sel >> end;
    if(end != "END")
    {
        std::cerr << "Error in reading END in " << sel_file << " in variator.\n";
        return -1;
    }
    return OK;
}

int Variator::update_population(vector<Individual*> &new_pop, 
                                vector<Individual*> &curr_pop, 
                                vector<Individual*> &curr_offspring)
{
    for(int i = 0; i < curr_pop.size(); i++)
    {
        if(not curr_pop[i]->selected_to_new_pop and
           curr_pop[i]->position_in_external_archive == NOT_IN_SET)
            // This individual was not selected to new population and is not in external archive: delete it
            remove_individual(i, curr_pop);
    }
    for(int i = 0; i < curr_offspring.size(); i++)
    {
        if(not curr_offspring[i]->selected_to_new_pop and
           curr_offspring[i]->position_in_external_archive == NOT_IN_SET)
            // This individual was not selected to new population and is not in external archive: delete it
            remove_individual(i, curr_offspring);
    }
    
    curr_pop.clear();
    curr_pop = new_pop;
    curr_offspring.clear();
    new_pop.clear();
}