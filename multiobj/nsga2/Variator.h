#ifndef VARIATOR_H_
#define VARIATOR_H_

#include "Individual.h"

#include <algorithm>
#include <vector>
#include <iostream>

using std::swap;
using std::vector;

struct params
{
    int generated_solutions_num; // Number of initial random solutions to be generated
    int pop_size;                // Size of population
    int parents_num;             // Number of parents to be selected for crossover
    int offspring_size;          // Number of offspring to be produced
    double diversification_prob; // Probability of diversification in crossover: in the [0.0, 1.0) interval
    double intensification_prob; // Probability of intensification in crossover: in the [0.0, 1.0) interval

    params(int gen, int pop, int p, int o, double dprob, double iprob)
    {
        generated_solutions_num = gen;
        pop_size = pop;
        parents_num = p;
        offspring_size = o;
        diversification_prob = dprob;
        intensification_prob = iprob;
    }
};


class Variator
{
  public:
    int n_facs, n_objs;
    DistMatrix *distances;
    FlowMatrices *flows;

    int generated_solutions_num; // Number of initial random solutions to be generated
    int pop_size;                // Size of population
    int num_parents;             // Number of parents to be selected for crossover
    int offspring_size;          // Number of offspring to be produced
    double diversification_prob; // Probability of diversification in crossover: in the [0.0, 1.0) interval
    double intensification_prob; // Probability of intensification in crossover: in the [0.0, 1.0) interval

    Variator(params &p, DistMatrix *dist, FlowMatrices *f)
    {
        generated_solutions_num = p.generated_solutions_num;
        pop_size = p.pop_size;
        num_parents_num = p.parents_num;
        offspring_size = p.offspring_size;
        diversification_prob = p.diversification_prob;
        intensification_prob = p.intensification_prob;

        distances = dist;
        flows = f;
    }

    void init(vector<Individual*> &non_dominated);
    void random_pop(vector<Individual*> &pop, vector<Individual*> &external_archive);
    bool update_nondom_set(Individual *ind, vector<Individual*> &non_dominated);
    bool update_nondom_set(Individual *ind, vector<Individual*> &non_dominated);
    
};

#endif