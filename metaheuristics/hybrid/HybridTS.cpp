#include "HybridTS.h"

#include <vector>

using std::vector;

/*--------------------------------------------------------------*/
/*       compute the cost difference if elements i and j        */
/*         are transposed in permutation (solution) p           */
/*            based on RoTS code by Eric Taillard               */
/*--------------------------------------------------------------*/
long HybridTS::compute_delta(int i, int j, solution &s)
{
    long d;
    d = (flows[i][i] - flows[j][j]) *
        (distances[ s.p[j] ][ s.p[j] ] - distances[ s.p[i] ][ s.p[i] ])
        +
        (flows[i][j] - flows[j][i]) *
        (distances[ s.p[j] ][ s.p[i] ] - distances[ s.p[i] ][ s.p[j] ]);

    for(int k = 0; k < n_facs; k++)
    {
        if(k == i or k == j) continue;

        d += (flows[k][i] - flows[k][j]) *
             (distances[ s.p[k] ][ s.p[j] ] - distances[ s.p[k] ][ s.p[i] ])
             +
             (flows[i][k] - flows[j][k]) *
             (distances[ s.p[j] ][ s.p[k] ] - distances[ s.p[i] ][ s.p[k] ]);
    }

    return d;
}

/*--------------------------------------------------------------*/
/*      Idem, but the value of delta[i][j] is supposed to       */
/*    be known before the transposition of elements r and s     */
/*            based on RoTS code by Eric Taillard               */
/*--------------------------------------------------------------*/
long HybridTS::compute_delta(int i, int j, int r, int s, solution &s)
{
    long d;
    d = delta[i][j]
        +
        (flows[r][i] - flows[r][j] + flows[s][j] - flows[s][i]) *
        (distances[ s.p[s] ][ s.p[i] ] - distances[ s.p[s] ][ s.p[j] ] + 
         distances[ s.p[r] ][ s.p[j] ] - distances[ s.p[r] ][ s.p[i] ])
        +
        (flows[i][r] - flows[j][r] + flows[j][s] - flows[i][s]) * 
        (distances[ s.p[i] ][ s.p[s] ] - distances[ s.p[j] ][ s.p[s] ] + 
         distances[ s.p[j] ][ s.p[r] ] - distances[ s.p[i] ][ s.p[r] ] );
    
    return d;
}