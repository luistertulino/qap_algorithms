#include "SolutionTS.h"

#include <vector>

using std::vector;

void SolutionTS::compute_deltas(int it1, int it2, FlowMatrices &flows, DistMatrix &distances)
{    
    if (last_i != -1 and last_j != -1
        and
        it1 != last_i and it1 != last_j
        and
        it2 != last_i and it2 != last_j)
    {        
        for(int k = 0; k < n_objs; k++)
        {
            deltas[k] += (flows[k][last_i][it1] - flows[k][last_i][it2] + flows[k][last_j][it2] - flows[k][last_j][it1]) *
                         (distances[ p[last_j] ][ p[it1] ] - distances[ p[last_j] ][ p[it2] ] + 
                         distances[ p[last_i] ][ p[it2] ] - distances[ p[last_i] ][ p[it1] ])
                         +
                         (flows[k][it1][last_i] - flows[k][it2][last_i] + flows[k][it2][last_j] - flows[k][it1][last_j]) * 
                         (distances[ p[it1] ][ p[last_j] ] - distances[ p[it2] ][ p[last_j] ] + 
                         distances[ p[it2] ][ p[last_i] ] - distances[ p[it1] ][ p[last_i] ] );
        }        
    }    
    else
    {        
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
                             (distances[ p[k] ][ p[it2] ] - distances[ p[i] ][ p[it1] ])
                             +
                             (flows[k][it1][i] - flows[k][it2][i]) *
                             (distances[ p[it2] ][ p[i] ] - distances[ p[it1] ][ p[i] ]);
            }
            
        }        
    }
    last_i = it1; last_j = it2;    
}

void SolutionTS::compute_objs()
{
    for(int k = 0; k < n_objs; k++) objs[k] += deltas[k];
}

void SolutionTS::compute_objs(DistMatrix &dist_mat, FlowMatrices &flow_mats)
{
    for (int k = 0; k < n_objs; ++k)
    {
        objs[k] = 0;
        for (int i = 0; i < n_facs; ++i)
            for (int j = 0; j < n_facs; ++j)
                objs[k] += flow_mats[k][i][j] * dist_mat[p[i]][p[j]];
    }
}