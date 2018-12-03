#include "Individual.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

using std::vector;

/*---------------------------------------------*/
/*------------ DOMINANCE FUNCTIONS ------------*/
/*---------------------------------------------*/
bool operator<=(Individual &ind1, Individual &ind2)
{
    bool is_equal = true;
    for (int i = 0; i < ind1.objs.size(); ++i)
    {
        if(ind1.objs[i] > ind2.objs[i]) return false;
        is_equal = (ind1.objs[i] == ind2.objs[i]) and is_equal;
    }
    return (not is_equal);
}

bool operator==(Individual &ind1, Individual &ind2)
{
    if (ind1.objs != ind2.objs) return false;
    return (ind1.p == ind2.p);
}

bool incomparable(Individual &ind1, Individual &ind2)
{
    return ( not(ind1 <= ind2) and not(ind2 <= ind1) );
}


void Individual::randomize(std::mt19937 &gen, DistMatrix &dist_mat, FlowMatrices &flow_mats)
{
    for (int i = 0; i < n_facs; ++i)
        p[i] = i;

    std::shuffle(p.begin(), p.end(), gen);
    compute_objs(distances, flows);
}


/*------------------------------------------------------------*/
/*---------------- OBJECTIVE FUNCTIONS METHODS ---------------*/
/*------------------------------------------------------------*/
void Individual::compute_deltas(int it1, int it2, FlowMatrices &flows, DistMatrix &distances)
{   // Compute variation in objective functions
    for(int k = 0; k < n_objs; k++)
    {
        deltas[k] = (flows[k][it1][it1] - flows[k][it2][it2]) *
                    (distances[ p[it2] ][ p[it2] ] - distances[ p[it1] ][ p[it1] ])
                    +
                    (flows[k][it1][it2] - flows[k][it2][it1]) *
                    (distances[ p[it2] ][ p[it1] ] - distances[ p[it1] ][ p[it2] ]);

        
        for(int i = 0; i < n_facs; i++)
        {
            if(i == it1 or i == it2) continue;

            deltas[k] += (flows[k][i][it1] - flows[k][i][it2]) *
                         (distances[ p[i] ][ p[it2] ] - distances[ p[i] ][ p[it1] ])
                         +
                         (flows[k][it1][i] - flows[k][it2][i]) *
                         (distances[ p[it2] ][ p[i] ] - distances[ p[it1] ][ p[i] ]);
        }            
    }
    last_i = it1; last_j = it2;
    std::swap(p[it1], p[it2]);

}

void Individual::compute_objs()
{   // Compute objective functions given delta values
    for(int k = 0; k < n_objs; k++) objs[k] += deltas[k];
}

void Individual::compute_objs(DistMatrix &dist_mat, FlowMatrices &flow_mats)
{   // Compute objective functions from scratch
    for (int k = 0; k < n_objs; ++k)
    {
        objs[k] = 0;
        for (int i = 0; i < n_facs; ++i)
            for (int j = 0; j < n_facs; ++j)
                objs[k] += flow_mats[k][i][j] * dist_mat[p[i]][p[j]];
    }
}

void Individual::print()
{
    std::cout << "Permutation: ";
    for (int i = 0; i < p.size(); ++i)
    {
        std::cout << i << "-->" << p[i] << ", ";
    }
    std::cout << "\nObjs: ";
    for (int i = 0; i < objs.size(); ++i)
    {
        std::cout << objs[i] << " ";
    }
    std::cout << "\nlast_i = " << last_i << " last_j = " << last_j;
    std::cout << "\ndeltas: ";
    for (int i = 0; i < deltas.size(); ++i)
    {
        std::cout << deltas[i] << " ";
    }
    std::cout << "\n\n";
    std::cout << "\n-----------------------------------------------------------\n";
}