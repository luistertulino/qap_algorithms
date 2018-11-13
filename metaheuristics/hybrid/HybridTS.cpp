#include "HybridTS.h"

#include <vector>
#include <random> // for random devices
#include <chrono> // for measuring time
#include <climits>
#include <algorithm> // std::swap
#include <cmath> // exp 

using std::vector;

std::random_device rd;
std::mt19937 gen(rd());

float time_limit = 1000;
int max_iter = 200; // 1 million
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

bool HybridTS::isTabu(int i, int j, solution &sol, int it)
{
    if( tabu_list[i][sol.p[j]] < it ) return false;
    else return true;
}

void HybridTS::make_tabu(int i, int j, solution &sol, int curr_tabu, int it)
{
    tabu_list[i][sol.p[i]] = it + curr_tabu;
    tabu_list[j][sol.p[j]] = it + curr_tabu;
}

solution HybridTS::init()
{
    time_t begin, now;
    time(&begin);
    
    /* -------------- Generate a random initial solution -------------- */
    solution best(n_facs); best.shuffle(gen);
    for(int i = 0; i < n_facs; i++)
    {
        for(int j = 0; j < n_facs; j++)
        {
            best.cost += flows[i][j] * distances[ best.p[i] ][ best.p[j] ];

            // Initialize delta matrix (based on code by Eric Taillard)
            if(i < j) { delta[i][j] = compute_delta(i, j, best); }
        }
    }
    solution curr; curr = best;
    /* -------------- Generate a random initial solution -------------- */

    /* -------------- Matrix of allocation couting -------------- */
    Matrix alloc_count; alloc_count.resize(n_facs);
    for(int i = 0; i < n_facs; i++)
        alloc_count[i].resize(n_facs, 0);

    for(int i = 0; i < n_facs; i++)
        alloc_count[i][curr.p[i]] += 1;
    /* -------------- Matrix of allocation couting -------------- */

    std::uniform_int_distribution<int> distribution(min_tabu_list, min_tabu_list+delta_tabu);
    int curr_tabu = distribution(gen); // Current size of tabu list

    std::uniform_real_distribution<double> annealing(0.0,1.0); // For annealing random selection

    /* -------------- Initialize tabu list -------------- */
    for(int i = 0; i < n_facs; i++)
    {
        for(int j = 0; j < n_facs; j++)
        {
            tabu_list[i][j] = 0; // Indicates until which iteration the allocation of j in i will be tabu
        }
    }
    /* -------------- Initialize tabu list -------------- */

    bool use_second = (n_facs >= threshold); // Whether to use or not the second aspiration function. If that's the case, this functions takes priority over the classical one
    
    int num_fails = 0;
    /*  
        When a degrading movement is accepted by simulated annealing, this counter is incremented
        When a movement improves the best solution, it is restarted
        When it reaches a threshold, the current solution is restarted, together with the tabu list
    */

    int iteration_found = -1; // Iteration when the solution from qaplib (or better) was found
    int num_iter = 0;
    /* 
       Checks if maximum number of iterations criterium is adopted.
       If it is, checks whether that number was reached.
       If it's not, checks whether the time limit for main loop was reached.
    */

    /* --------------- Initialization of temperature --------------- */
    long dmin = LONG_MAX; long dmax = 0;
    std::uniform_int_distribution<int> rand_item(0, n_facs); // The swap will be random
    while(num_iter < max_iter/100)
    {
        num_iter++;        
        if(num_iter % (2*(min_tabu_list+delta_tabu)) == 0)
        {
            curr_tabu = distribution(gen);
            std::cout << "------curr tabu: " << curr_tabu << "---------\n";
        }

        int it1 = rand_item(gen);
        int it2 = rand_item(gen); while(it2 == it1) it2 = rand_item(gen);

        if(delta[it1][it2] > 0){ dmin = std::min(dmin, delta[it1][it2]); dmax = std::max(dmax, delta[it1][it2]); }

        bool tabu = isTabu(it1, it2, curr, num_iter);
        bool aspired = (use_second and // Second aspiration function by Taillard. It is only used for "big" problems
                        (tabu_list[it1][ curr.p[it2] ] < num_iter - aspiration) and
                        (tabu_list[it2][ curr.p[it1] ] < num_iter - aspiration) 
                        or
                        curr.cost + delta[it1][it2] < best.cost);

        if(aspired or not tabu)
        {            
            curr.cost += delta[it1][it2];
            std::swap(curr.p[it1], curr.p[it2]);
            make_tabu(it1, it2, curr, curr_tabu, num_iter);
        }
        else continue; // if the move was not valid, there is nothing else to do
        if(curr.cost < best.cost) best = curr;

        // Update delta table, but only if a movement as applied
        for(int i = 0; i < n_facs; i++)
        {
            for(int j = i+1; j < n_facs; j++)
            {                
                if(i != it1 and i != it2 and j != it1 and j != it2)
                    delta[i][j] = compute_delta(i, j, it1, it2, curr);
                else delta[i][j] = compute_delta(i, j, curr);
            }
        }

    }

    double t_begin = dmin + (dmax - dmin)/10.0; // Initial temperature
    double t_end = dmin; // Final temperature
    double beta = (t_begin - t_end) / (max_iter * t_begin * t_end );

    double temperature = t_begin;
    /* --------------- Initialization of temperature --------------- */


    while( max_iter_crit ? num_iter <= max_iter : difftime(time(&now),begin) < time_limit )
    {
    	std::cout << "---------------------------------------\n";
        std::cout << "Iteration: " << num_iter << "\n";

    	if(best.cost <= qaplib_sol) iteration_found = num_iter;

        if(num_iter % (2*(min_tabu_list+delta_tabu)) == 0)
        {
            curr_tabu = distribution(gen);
            std::cout << "------curr tabu: " << curr_tabu << "---------\n";
        }
            // Taillard's random update of tabu list size at each 2*s_max iterations
        
        int i_retained = -1, j_retained = -1;
        long min_delta = LONG_MAX;
        bool aspired = false;
        bool already_aspired = false;

        /* ------------------ Exploration of neighborhood ------------------ */
        for(int i = 0; i < n_facs; i++)
        {
            for(int j = i+1; j < n_facs; j++)
            {
                bool tabu = isTabu(i, j, curr, num_iter);
                aspired = (use_second and // Second aspiration function by Taillard. It is only used for "big" problems
                           (tabu_list[i][ curr.p[j] ] < num_iter - aspiration) and
                           (tabu_list[j][ curr.p[i] ] < num_iter - aspiration) 
                           or
                           curr.cost + delta[i][j] < best.cost);

                bool lesser_delta = (delta[i][j] < min_delta);
                if( (aspired and not already_aspired) // First move aspired
                    or
                    (aspired and already_aspired and lesser_delta) // More than one move was aspired and take the best one
                    or
                    (not aspired and not already_aspired and lesser_delta and not tabu) )
                    	// The move is not an aspiration one but is a better one and is not tabu: it can be applied to curr
                {
                    i_retained = i; j_retained = j;
                    min_delta = delta[i][j];

                    if(aspired) already_aspired = true;
                }
            }
        }
        /* ------------------ Exploration of neighborhood ------------------ */
        
        //std::cout << "i_retained: " << i_retained << " j_retained: " << j_retained << " ";

        bool new_sol = false;

        if(i_retained == -1) // No move was retained
        {
            curr.random_restart(alloc_count, gen);
            curr.comp_cost(flows, distances);
            new_sol = true;
            num_fails = 0;
        }
        else
        {
            //std::cout << "delta[i_retained][j_retained]: " << delta[i_retained][j_retained] << "\n";
            
            long new_cost = curr.cost + delta[i_retained][j_retained];
            std::cout << "best.cost = " << best.cost << " new_cost = " << new_cost << "\n";
            if(new_cost < best.cost // Improves best solution
               or
               (num_fails < max_fails and annealing(gen) < exp((best.cost-new_cost)/temperature) ) )
                  // Does not improve best solution but is selected to be applied by the simulated annealing criterium
            {
            	if(best.cost < new_cost) num_fails++; // If that's the case, the second condition was true
                // Apply move
                std::swap(curr.p[i_retained], curr.p[j_retained]);
                // Update solution value
                curr.cost += delta[i_retained][j_retained];
                // Update tabu list with the move made
                make_tabu(i_retained, j_retained, curr, curr_tabu, num_iter);

                std::cout << "Apply movement\n";
            }
            else{ // The move was not selected. Do a random restart on the solution
                curr.random_restart(alloc_count, gen);
                curr.comp_cost(flows, distances);

                new_sol = true;
                num_fails = 0;
            }            
        }

        std::cout << "num_fails: " << num_fails << "\n";

        if (new_sol) // A random solution was created, so the tabu list must be restarted
        {
            std::cout << "new solution cost: " << curr.cost << "\n";
        	std::cout << "new random solution made.\n";
        	for(int i = 0; i < n_facs; i++)
		    {
		        for(int j = 0; j < n_facs; j++)
		        {
		            tabu_list[i][j] = 0; // Indicates until which iteration the allocation of j in i will be tabu
		        }
		    }
        }

        for(int i = 0; i < n_facs; i++)
            alloc_count[i][curr.p[i]] += 1;

        if(curr.cost < best.cost) best = curr;

        // Update delta table
        for(int i = 0; i < n_facs; i++)
        {
            for(int j = i+1; j < n_facs; j++)
            {
                if(not new_sol and i != i_retained and i != j_retained and j != i_retained and j != j_retained)
                    delta[i][j] = compute_delta(i, j, i_retained, j_retained, curr);
                    
                else delta[i][j] = compute_delta(i, j, curr);
            }
        }

        num_iter++;
        std::cout << "best cost = " << best.cost << "\n";
        temperature = temperature / (1.0 + beta*temperature);
    }

    return best;
}