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
                         (distances[ sol.p[last_j] ][ sol.p[it1] ] - distances[ sol.p[last_j] ][ sol.p[it2] ] + 
                         distances[ sol.p[last_i] ][ sol.p[it2] ] - distances[ sol.p[last_i] ][ sol.p[it1] ])
                         +
                         (flows[k][it1][last_i] - flows[k][it2][last_i] + flows[k][it2][last_j] - flows[k][it1][last_j]) * 
                         (distances[ sol.p[it1] ][ sol.p[last_j] ] - distances[ sol.p[it2] ][ sol.p[last_j] ] + 
                         distances[ sol.p[it2] ][ sol.p[last_i] ] - distances[ sol.p[it1] ][ sol.p[last_i] ] );
        }        
    }    
    else
    {        
        for(int k = 0; k < n_objs; k++)
        {
            deltas[k] = (flows[k][it1][it1] - flows[k][it2][it2]) *
                        (distances[ sol.p[it2] ][ sol.p[it2] ] - distances[ sol.p[it1] ][ sol.p[it1] ])
                        +
                        (flows[k][it1][it2] - flows[k][it2][it1]) *
                        (distances[ sol.p[it2] ][ sol.p[it1] ] - distances[ sol.p[it1] ][ sol.p[it2] ]);

            
            for(int i = 0; i < n_facs; i++)
            {
                if(i == it1 or i == it2) continue;

                deltas[k] += (flows[k][i][it1] - flows[k][i][it2]) *
                             (distances[ sol.p[k] ][ sol.p[it2] ] - distances[ sol.p[i] ][ sol.p[it1] ])
                             +
                             (flows[k][it1][i] - flows[k][it2][i]) *
                             (distances[ sol.p[it2] ][ sol.p[i] ] - distances[ sol.p[it1] ][ sol.p[i] ]);
            }
            
        }        
    }
    last_i = it1; last_j = it2;    
}

void SolutionTS::compute_objs()
{
    for(int k = 0; k < n_objs; k++) objs[k] += deltas[k];
}