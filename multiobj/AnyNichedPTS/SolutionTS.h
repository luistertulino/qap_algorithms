#ifndef SOLUTIONTS_H_
#define SOLUTIONTS_H_

#define CASE_ASPIRED 1
#define CASE_ANYWAY 2

#include "../lib/definitions.h"

#include <algorithm>
#include <vector>
#include <iostream>

using std::swap;
using std::vector;

class SolutionTS
{
  public:
    int n_facs, n_objs;
    vector<int> p; // Permutation
    vector<long> objs; // Objective functions
    bool visited; // true if this solution was explored; false otherwise
    int last_i, last_j; // Indicates the last movement made in the solution
    vector<long> deltas; // stores the variation of objs at the previous step of the iteration
    int status; // if 1, that solution is at least non-dominated;
                // if 2, that solution enters in the archive anyway
    int dom_count;
    vector<double> dijs;
    double niche_count;

    SolutionTS()
    {
        visited = false;
        last_i = last_j = -1;
        n_facs = n_objs = 0;
        dom_count = 0;
        niche_count = 1;
    }
    SolutionTS(int n_f, int n_o)
    {
        n_facs = n_f;
        n_objs = n_o;
        p.resize(n_facs);
        objs.resize(n_objs);
        deltas.resize(n_objs);
        visited = false;
        last_i = last_j = -1;
        dom_count = 0;
        status = -1;
        niche_count = 1;
    }
    SolutionTS(SolutionTS &sol, int l_i, int l_j)
    {
        // Creates a new solution based in a solution, the movement made and the variation on objs
        n_facs = sol.n_facs;
        n_objs = sol.n_objs;
        last_i = l_i;
        last_j = l_j;
        p = sol.p;
        swap(p[l_i], p[l_j]);
        objs.resize(n_objs);
        visited = false;
        dom_count = 0;
        status = -1;
        niche_count = sol.niche_count;
    }
    SolutionTS(SolutionTS &s)
    {
        //std::cout << "make a new solution by copy\n";
        p = s.p;
        objs = s.objs;
        deltas = s.deltas;
        n_facs = s.n_facs;
        n_objs = s.n_objs;
        last_i = s.last_i;
        last_j = s.last_j;
        visited = s.visited;
        status = s.status;
        dom_count = 0;
        s.niche_count;
    }
    
    SolutionTS & operator=(SolutionTS &s)
    {
        p = s.p;
        objs = s.objs;
        deltas = s.deltas;
        n_facs = s.n_facs;
        n_objs = s.n_objs;
        last_i = s.last_i;
        last_j = s.last_j;
        visited = s.visited;
        status = s.status;
        dom_count = 0;
        s.niche_count;
        return *this;
    }

    void compute_deltas(int it1, int it2, FlowMatrices &flows, DistMatrix &distances);
    void compute_objs();
    void compute_objs(DistMatrix &dist_mat, FlowMatrices &flow_mats);
    void print();

};

bool operator<=(SolutionTS &s1, SolutionTS &s2);
bool operator==(SolutionTS &s1, SolutionTS &s2);
bool incomparable(SolutionTS &s1, SolutionTS &s2);


#endif