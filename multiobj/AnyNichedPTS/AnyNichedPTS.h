#ifndef AnyNichedPTS_H_
#define AnyNichedPTS_H_

#define DISCARDED 0

#include "SolutionTS.h"

#include <list>
#include <vector>
#include <utility> // for std::pair

#include <iostream>

using std::list;
using std::vector;
using std::pair;

typedef pair<double,int> time_eval;

struct params
{
    int min_tabu_list;
    int delta_tabu;
    int size_arc_init;
    double refset_size;

    void print()
    {
        std::cout << "min_tabu_list = " << min_tabu_list << "\n";
        std::cout << "delta tabu = " << delta_tabu << "\n";
        std::cout << "size_arc_init = " << size_arc_init << "\n";
        std::cout << "refset_size = " << refset_size << "\n";
    }
};

inline bool order(SolutionTS *s1, SolutionTS *s2)
{
    return (s1->objs < s2->objs);
}

class AnyNichedPTS
{
    int dominated_solutions_in_archive;
  public:
    int n_facs, n_objs;
    DistMatrix *distances;
    FlowMatrices *flows;
    int size_arc_init;
    double refset_size; // In percentage
    int min_tabu_list; // Minimun tabu list size
    int delta_tabu;    // Maximum variation of tabu list size
    Matrix tabu_list;

  	AnyNichedPTS(int n_f, int n_o, DistMatrix *dist, FlowMatrices *_flows, params &p) /* >>>>>> Change it later to receive a struct with all parameters <<<<<< */
    {
        n_facs = n_f;
        n_objs = n_o;
        distances = dist;
        flows = _flows;

        size_arc_init = p.size_arc_init;
        refset_size = p.refset_size;
        min_tabu_list = p.min_tabu_list;
        delta_tabu = p.delta_tabu;

        tabu_list.resize(n_facs, vector<int>(n_facs, 0));
    }

    void random_solution(SolutionTS &s);

    void random_archive(vector<SolutionTS*> &archive, int size_arc, vector<int> &non_visited,
                        vector<long> &min_objs, vector<long> &max_objs);

    int select_solution(vector<SolutionTS*> &archive, vector<int> &non_visited,
                        vector<long> &min_objs, vector<long> &max_objs);

    double dij(SolutionTS &s1, SolutionTS &s2, vector<long> &min_objs, vector<long> &max_objs);

    bool isTabu(int i, int j, SolutionTS &s, int it);

    void make_tabu(int i, int j, SolutionTS &s, int curr_tabu, int it);

    time_eval init(bool num_avals_crit, int max_num_avals, float time_limit,
            vector<SolutionTS*> &non_dominated);
        // Main procedure
        /*
          Parameters:
            - num_avals_crit -> indicates if the stopping criterium is the number of obj avaliations
            - num_avals      -> number of avaliations of obj allowed before method ending. It matters only when num_avals_crit = true
            - time_limit     -> time limit for the execution of the algorithm
            - If num_avals_crit = false or time_limit = 0, 
                the stopping criterium is only the emptiness of the non-visited solutions set
        */

    bool update_nondom_set(SolutionTS *solution, vector<SolutionTS*> &non_dominated);
        // Update the non-dominated set
};

#endif