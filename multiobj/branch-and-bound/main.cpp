#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility> // std::pair, std::make_pair
#include <fstream>
#include <ctime> // for measure elapsed time

#include "../lib/definitions.h"
#include "../lib/readdata.h"
#include "BNB.h"

using std::string;
using std::vector;
using std::list;
using std::pair;

bool order(Solution &s1, Solution &s2)
{
    return (s1.objs < s2.objs);
}

void writedata(int n_objs, 
               int n_facs,
               string &instance, 
               double elapsed_time, 
               pair<int,int> nodes, 
               list<Solution> &non_dominated_set)
{
    /*----------------------- PRINT NON-DOMINATED SET -----------------------*/
    string outf = "../solutions/";
    outf += instance;
    outf[last(outf)-1] = 'o'; outf[last(outf)] = 'u'; outf.push_back('t');

    std::ofstream outfile;
    outfile.open(outf, std::ios::app);

    if (!outfile.is_open())
    {
        std::cout << "Error in creating and opening " << outf << " file.\n";
        return;
    }

    outfile << non_dominated_set.size() << " " << n_objs << "\n";

    for (auto sol : non_dominated_set)
    {
        for (auto pi : sol.permutation)
        {
            outfile << pi << " ";
        }
        outfile << "\n";
        for (auto obj : sol.objs)
        {
            outfile << obj << " ";
        }
        outfile << "\n";
    }

    outfile.close();
    /*----------------------- PRINT NON-DOMINATED SET -----------------------*/

    /*-------------------------- PRINT STATISTICS ---------------------------*/
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

        BNB bnb(n_facs, n_objs, &dist_mat, &flow_mats);
        list<Solution> non_dominated_set;
        pair<int,int> count_nodes = bnb.init(non_dominated_set);

        time_t now;
        time(&now);

        non_dominated_set.sort(order);

        writedata(n_objs, n_facs, instance, difftime(now,begin), count_nodes, non_dominated_set);
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