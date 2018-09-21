#include "PLS.h"

#include <list>

using std::list;

int PLS::non_dominance_set(Solution &s, list<Solution> &non_dominated){
    auto sol = non_dominated.begin();

    while(sol != non_dominated.end()){

        if (s <= (*sol)) 
        {
            /* s dominates  */
        }
    }
}