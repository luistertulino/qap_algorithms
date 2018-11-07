#include "HybridTS.h"

#include <vector>
#include <random> // for random devices
#include <chrono> // for measuring time
#include <climits>

using std::vector;

std::random_device rd;
std::mt19937 gen(rd());

float time_limit = 1000;
int max_iter = 1000000; // 1 million
bool max_iter_crit = true;

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

    /* -------------- Simulated annealing variables -------------- */
    double t_begin = 2; // Initial temperature
    double t_end = 1; // Final temperature
    double beta = (t_begin - t_end) / (max_iter * t_begin * t_end );
    /* -------------- Simulated annealing variables -------------- */
    
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
    solution s; s = best;
    /* -------------- Generate a random initial solution -------------- */


    int num_iter = 1;
    /* 
       Checks if maximum number of iterations is adopted.
       If it is, checks whether that number was reached.
       If it's not, checks whether the time limit for main loop was reached.
    */

    int items[n_facs]; // Available items for swapping
    double temperature = t_begin;

    std::uniform_int_distribution<int> distribution(min_tabu_list, min_tabu_list+delta_tabu);
    int curr_tabu = distribution(gen); // Current size of tabu "list"
    
    while( max_iter_crit ? num_iter <= max_iter : difftime(time(&now),begin) < time_limit )
    {
        for (int i = 0; i < n_facs; ++i) items[i] = i;
        int item_begin = 0;

    	int num_fails = 0;

        MatrixLong delta_temp = delta;

        if(num_iter % 2*(min_tabu_list+delta_tabu) == 0) curr_tabu = distribution(gen);
            // Taillard's random update of tabu list size at each 2*s_max iterations

        
    	/* ------------------------ "EJECTION CHAIN" LOOP ------------------------ */
    	while(num_fails < max_fails)
    	{
    		int i_retained = j_retained = -1;
    		bool improv = false;
    		long f_s1 = LONG_MAX;

    		for (int i = item_begin; i < n_facs and not improv; ++i)
    		{
                int ri = items[i];
    			for (int j = i+1; j < n_facs and not improv; ++j)
    			{
                    int rj = items[j];

    				if(s.cost + delta[ri][rj] < best.cost // Aspiration criterium
                       or
                       s.cost + delta[ri][rj] < s.cost and not isTabu(ri, rj, s, curr_tabu, num_iter)
                       // Movement does not improve best solution but improves current and is not tabu
                    {
                        i_retained = i; j_retained = j;
                        improv = true;

                    }
                    else if(not isTabu(ri, rj, s, curr_tabu, num_iter)
                            and
                            s.cost + delta[ri][rj] < f_s1)
                    {
                        i_retained = i; j_retained = j;
                    }

    			}
    		}

            f_s1 = s.cost + delta[i_retained][j_retained]; // Modification on current solution

            if(improv) num_fails = 0;
            else num_fails++;

            /* -----------------------------------
                TODO: guardar última modificação que melhorou a melhor solução
                Como: guardar lista de swaps realizados

                TODO: atualizar tabela local de deltas

                TODO: "remover" i_ret e j_ret da lista de permitidos
            ----------------------------------- */
    	}
    	/* ------------------------ "EJECTION CHAIN" LOOP ------------------------ */

    }

}