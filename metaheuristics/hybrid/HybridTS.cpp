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
int max_iter = 1000000; // 1 million
bool max_iter_crit = true;

void random_restart(solution &s, Matrix &alloc_count);

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

    double temperature = t_begin;

    bool use_second = (n_facs >= threshold); // Whether to use or not the second aspiration function. If that's the case, this functions takes priority over the classical one
    
    int num_fails = 0;
    /*  
        When a degrading movement is accepted by simulated annealing, this counter is incremented
        When a movement improves the best solution, it is restarted
        When it reaches a threshold, the current solution is restarted, together with the tabu list
    */

    int num_iter = 1;
    /* 
       Checks if maximum number of iterations is adopted.
       If it is, checks whether that number was reached.
       If it's not, checks whether the time limit for main loop was reached.
    */
    while( max_iter_crit ? num_iter <= max_iter : difftime(time(&now),begin) < time_limit )
    {
        if(num_iter % 2*(min_tabu_list+delta_tabu) == 0) curr_tabu = distribution(gen);
            // Taillard's random update of tabu list size at each 2*s_max iterations
        
        int i_retained = -1, j_retained = -1;
        long min_delta = LONG_MAX;
        bool aspired = false;
        bool already_aspired = false;

        // Explore neighborhood
        for(int i = 0; i < n_facs; i++)
        {
            for(int j = i+1; j < n_facs; j++)
            {
                bool tabu = isTabu(i, j, s, num_iter);
                aspired = (use_second and // Second aspiration function by Taillard. It is only used for "big" problems
                           (tabu_list[i][ s.p[j] ] < num_iter - aspiration) and
                           (tabu_list[j][ s.p[i] ] < num_iter - aspiration) 
                           or
                           curr.cost + delta[i][j] < best.cost);

                bool lesser_delta = (delta[i][j] < min_delta);
                if( (aspired and not already_aspired) // First move aspired
                    or
                    (aspired and already_aspired and lesser_delta) // More than one move was aspired and take the best one
                    or
                    (not aspired and not already_aspired and lesser_delta and not tabu) )
                {
                    i_retained = i; j_retained = j;
                    min_delta = delta[i][j];

                    if(aspired) already_aspired = true;
                }
            }
        }

        bool new_sol = false; // When it's true, a complete new solution was generated

        if(i_retained == -1) // No move was retained
        {
            random_restart(curr, alloc_count);new_sol = true;
            curr.comp_cost();
        }
        else
        {
            long new_cost = curr.cost + delta[i_retained][j_retained];
            if(new_cost < best.cost // Improves best solution
               or
               (num_fails < max_fails and annealing(gen) < exp((best.cost-new_cost)/temperature) ) )
                  // Does not improve best solution but is selected to be applied by the simulated annealing criterium
            {
                // Apply move
                swap(curr.p[i_retained], curr.p[j_retained]);
                // Update solution value
                curr.cost += delta[i_retained][j_retained];
                // Update tabu list with the move made
                make_tabu(i_retained, j_retained, curr, curr_tabu, num_iter);
            }
            else{ // The move was not selected. Do a random restart on the solution
                random_restart(curr, alloc_count);new_sol = true;
                curr.comp_cost();
            }            
        }

        for(int i = 0; i < n_facs; i++)
            alloc_count[i][curr.p[i]] += 1;

        if(new_cost < best.cost) best = curr;

        // Update delta table
        for(int i = 0; i < n_facs; i++)
        {
            for(int j = i+1; j < n_facs; j++)
            {
                if(i_retained != -1 and j_retained != -1
                   and
                   i != i_retained and i != j_retained
                   and
                   j != i_retained and j != j_retained)
                { 
                    delta[i][j] = compute_delta(i, j, i_retained, j_retained, curr);
                }
                else delta[i][j] = compute_delta(i, j, curr);
            }
        }
        
    }

}

void random_restart(solution &s, Matrix &alloc_count)
{
    int items[s.n_facs]; int remaining = s.n_facs;
    for(int i = 0; i < s.n_facs; i++) items[i] = i;
    vector<int> reverse_count(remaining,0);

    for(int i = 0; i < s.n_facs; i++) // For each item, chooses its location
    {
        /* 
            The probability that a location j is selected to item i is reverse to the number of times
            that the allocation i->j was made.
        */
        int sum = 0;
        for(int j = 0; j < remaining; j++) sum += alloc_count[i][items[j]];        
        for(int j = 0; j < remaining; j++) reverse_count[j] = sum - alloc_count[i][items[j]];

        std::discrete_distribution<int> dist(reverse_count.begin(), reverse_count.end());
        int selected = dist(gen)
        s.p[i] = items[selected];

        // remove that location of possible ones
        std::swap(items[selected], items[remaining-1]);
        remaining--;
    }
}