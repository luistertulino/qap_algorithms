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
            deltas[k] = 
        }        
    }
    
    
}