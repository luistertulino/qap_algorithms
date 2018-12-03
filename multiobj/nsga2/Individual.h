#ifndef INDIVIDUAL_H_
#define INDIVIDUAL_H_

#include "../lib/definitions.h"

#define CONSTRUCTED -2
#define NOT_IN_SET  -1

#include <algorithm>
#include <vector>
#include <iostream>

using std::swap;
using std::vector;

class Individual
{
  public:
    int n_facs, n_objs;
    vector<int> p; // Permutation
    vector<long> objs; // Objective functions
    int last_i, last_j; // Indicates the last movement made in the solution
    vector<long> deltas; // stores the variation of objs at the previous step of the iteration

    int domination_count; // Number of indivuals dominated by this one

    int position_in_external_archive;
    int index_in_population; /* Control variables. 
                                If some of them is -1, then this individual is not in that set */

    /*---------------------------------------------*/
    /*---------------- CONSTRUCTORS ---------------*/
    /*---------------------------------------------*/
    Individual(int n_f, int n_o)
    {
        n_facs = n_f;
        n_objs = n_o;
        p.resize(n_facs);
        objs.resize(n_objs);
        last_i = last_j = -1;
        domination_count = -1;
        position_in_external_archive = index_in_population = CONSTRUCTED;
    }
    Individual(Individual &ind)
    {
        n_facs = ind.n_facs;
        n_objs = ind.n_objs;
        p.resize(n_facs);
        objs.resize(n_objs);
        last_i = ind.last_i;
        last_j = ind.last_j;
        domination_count = -1;
        position_in_external_archive = index_in_population = CONSTRUCTED;
    }

    void randomize(std::mt19937 &gen); // Generate a random permutation

    /*------------------------------------------------------------*/
    /*---------------- OBJECTIVE FUNCTIONS METHODS ---------------*/
    /*------------------------------------------------------------*/
    void compute_deltas(int it1, int it2, FlowMatrices &flows, DistMatrix &distances);
    void compute_objs();
    void compute_objs(DistMatrix &dist_mat, FlowMatrices &flow_mats);

    void print(); // Not really necessary, used just for debug
    
};

/*---------------------------------------------*/
/*------------ DOMINANCE FUNCTIONS ------------*/
/*---------------------------------------------*/
bool operator<=(Individual &ind1, Individual &ind2);
bool operator==(Individual &ind1, Individual &ind2);
bool incomparable(Individual &ind1, Individual &ind2);

#endif