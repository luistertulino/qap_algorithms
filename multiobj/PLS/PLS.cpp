#include "PLS.h"

#include <list>

using std::list;

random_solution(Solution &s){
    // Just for now - CHANGE LATER!!!

    for (int i = 0; i < count; ++i)
    {
        s.permutation[i] = i+1;
    }
    s.compute_objs();
}

int PLS::init(DistMatrix &distances, FlowMatrices &flows, bool num_avals_crit, int max_num_avals, list<Solution> &non_dominated){
    Solution s(n_facs, n_objs);
    random_solution(s);

    list<Solution> non_visited;
    non_visited.push_back(s);

    non_dominated.push_back(s);

    int num_avals = 0;

    vector<int> new_objs(n_objs, 0);
    while(!non_visited.empty() or ( num_avals_crit and num_avals < max_num_avals ) )
    {
        auto solution = non_visited.begin();

        for (int i = 0; i < n_facs; ++i)
        {
            for (int j = i+1; j < n_facs; ++j)
            {
                compute_objs(*solution, i, j, new_objs, distances, flows);

                if (!(solution->objs <= new_objs) or solution->objs == new_objs)
                {
                    /* 
                        We only try to update the non-dominated set 
                        if the new "solution" is not dominated by the current solution 
                    */
                    if (try_update_nondom_set(new_objs, non_dominated))
                    {
                        // It was possible to add the new solution to the non-dominated set
                        // The new solution is added to the non-visited solutions set

                        non_visited.emplace_back(*solution, i, j, new_objs);
                    }
                }
            }
        }
    }
}

bool PLS::try_update_nondom_set(vector<int> &new_objs, list<Solution> &non_dominated){
    auto sol = non_dominated.begin();

    while(sol != non_dominated.end()){

        if (s <= (*sol)) 
        {
            if (s.objs != sol->objs)
            {
                // In this case, s dominates sol, and sol must me removed from the set
                sol = non_dominated.erase(sol);
            }
        }
        else if ((*sol) <= s)
        {
            // In this case, s is dominated by sol, and then must be discarded
            return DISCARDED;
        }
    }
}