#include "HybridTS.h"

#include <vector>

#include <random> // for random devices
#include <chrono> // for measuring time

using std::vector;

float time_limit = 1000;
bool time_limit_crit = true;
int max_iter = 1000000; // 1 million
bool max_iter_crit = false;

/*--------------------------------------------------------------*/
/*       compute the cost difference if elements i and j        */
/*         are transposed in permutation (solution) p           */
/*            Based on RoTS code by Eric Taillard               */
/*--------------------------------------------------------------*/
long HybridTS::compute_delta(int i, int j, solution &sol)
{
    long d;
    d = (flows[i][i] - flows[j][j]) *
        (distances[ sol.p[j] ][ sol.p[j] ] - distances[ sol.p[i] ][ sol.p[i] ])
        +
        (flows[i][j] - flows[j][i]) *
        (distances[ sol.p[j] ][ sol.p[i] ] - distances[ sol.p[i] ][ sol.p[j] ]);

    for(int k = 0; k < n_facs; k++)
    {
        if(k == i or k == j) continue;

        d += (flows[k][i] - flows[k][j]) *
             (distances[ sol.p[k] ][ sol.p[j] ] - distances[ sol.p[k] ][ sol.p[i] ])
             +
             (flows[i][k] - flows[j][k]) *
             (distances[ sol.p[j] ][ sol.p[k] ] - distances[ sol.p[i] ][ sol.p[k] ]);
    }

    return d;
}

/*--------------------------------------------------------------*/
/*      Idem, but the value of delta[i][j] is supposed to       */
/*    be known before the transposition of elements r and s     */
/*            Based on RoTS code by Eric Taillard               */
/*--------------------------------------------------------------*/
long HybridTS::compute_delta(int i, int j, int r, int s, solution &sol)
{
    long d;
    d = delta[i][j]
        +
        (flows[r][i] - flows[r][j] + flows[s][j] - flows[s][i]) *
        (distances[ sol.p[s] ][ sol.p[i] ] - distances[ sol.p[s] ][ sol.p[j] ] + 
         distances[ sol.p[r] ][ sol.p[j] ] - distances[ sol.p[r] ][ sol.p[i] ])
        +
        (flows[i][r] - flows[j][r] + flows[j][s] - flows[i][s]) * 
        (distances[ sol.p[i] ][ sol.p[s] ] - distances[ sol.p[j] ][ sol.p[s] ] + 
         distances[ sol.p[j] ][ sol.p[r] ] - distances[ sol.p[i] ][ sol.p[r] ] );
    
    return d;
}

bool HybridTS::isTabu(int i, int j, solution &sol, int curr_tabu, int it)
{
    if( tabu_list[i][sol.p[j]] + curr_tabu >= it ) return true;
    else return false;
}

void HybridTS::make_tabu(int i, int j, solution &sol, int it)
{
    tabu_list[i][sol.p[i]] = it;
    tabu_list[j][sol.p[j]] = it;
}

int HybridTS::init()
{
    time_t begin, now;
    time(&begin);
    
    /* -------------- Generate a random initial solution -------------- */
    solution best(n_facs); best.shuffle();
    for(int i = 0; i < n_facs; i++)
    {
        for(int j = 0; j < n_facs; j++)
        {
            best.cost += flows[i][j] * distances[ best.p[i] ][ best.p[j] ];

            // Initialize delta matrix (based on code by Eric Taillard)
            if(i < j) { delta[i][j] = compute_delta(i, j, best); }
        }
    }
    /* -------------- Generate a random initial solution -------------- */

    int num_iter = 1;
    /* 
       Checks if maximum number of iterations is adopted.
       If it is, checks whether that number was reached.
       If it's not, checks whether the time limit for main loop was reached.
    */
    while( max_iter_crit ? num_iter <= max_iter : difftime(time(&now),begin) < time_limit )
    {
        
    }

}