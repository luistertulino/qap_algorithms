#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility> // std::pair, std::make_pair
#include <fstream>
#include <ctime> // for measure elapsed time
#include <algorithm>

#include "../lib/definitions.h"
#include "../lib/readdata.h"
#include "PLS.h"

using std::string;
using std::vector;
using std::list;
using std::pair;

bool order(SolutionPLS *s1, SolutionPLS *s2)
{
    return (s1->objs < s2->objs);
}

void writedata(int n_objs, 
               int n_facs,
               string &instance, 
               vector<SolutionPLS*> &non_dominated_set,
               time_eval p)
{
    /*----------------------- PRINT NON-DOMINATED SET -----------------------*/
    string outf = "../PLSresults/";
    outf += instance;
    outf[last(outf)-1] = 'o'; outf[last(outf)] = 'u'; outf.push_back('t');

    std::ofstream outfile;
    outfile.open(outf, std::ios::app);

    if (!outfile.is_open())
    {
        std::cout << "Error in creating and opening " << outf << " file.\n";
        return;
    }

    outfile << non_dominated_set.size() << " " << p.first << " " << p.second << " " << n_facs << " " << n_objs << "\n";

    for (int i = 0; i < non_dominated_set.size(); i++)
    {
        SolutionPLS *sol = non_dominated_set[i];
        for (auto pi : sol->permutation)
            outfile << pi << " ";

        outfile << "\n";
        for (auto obj : sol->objs)
            outfile << obj << " ";

        outfile << "\n";
    }

    outfile << "--------------------\n";
    outfile.close();
    /*----------------------- PRINT NON-DOMINATED SET -----------------------*/

    /*-------------------------- PRINT STATISTICS ---------------------------
    outf = "results.txt";
    outfile.open(outf, std::ios::app);

    if (!outfile.is_open())
    {
        std::cout << "Error in  opening results.txt file.\n";
        return;
    }

    for (int i = 0; i < instance.size()-3; ++i)
    {
        outfile << instance[i];
    }
    outfile << " " << elapsed_time << " " << nodes.first << " " << nodes.second << "\n";

    outfile.close();
    /*-------------------------- PRINT STATISTICS ---------------------------*/
}

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        time_t begin;
        time(&begin);
        
        string instance(argv[1]);

        string file = "../instances/"+instance;

        int n_facs, n_objs;
        DistMatrix dist_mat;
        FlowMatrices flow_mats;

        read_data(file, n_facs, n_objs, dist_mat, flow_mats);

        PLS pls(n_facs, n_objs, &dist_mat, &flow_mats);
        vector<SolutionPLS*> non_dominated_set;

        bool num_avals_crit = true;
        int num_avals = 500000;
        float time_limit = 500;
        time_eval p = pls.init(num_avals_crit, num_avals, time_limit, non_dominated_set);

        std::sort(non_dominated_set.begin(), non_dominated_set.end(), order);

        writedata(n_objs, n_facs, instance, non_dominated_set, p);
        // difftime(now,begin)
        
        /*
        std::cout << "------------------RESULTS------------------\n";
        std::cout << "Number of non_dominated solutions: " << non_dominated_set.size();
        std::cout << "\nNON-DOMINATED SOLUTIONS\n\n";
        for(auto sol : non_dominated_set)
        {
            sol.print();
        }
        std::cout << "Number of nodes: " << count_nodes.first << "\n";
        std::cout << "Number of pruned nodes: " << count_nodes.second << "\n";
        */
    }
    return 0;
}