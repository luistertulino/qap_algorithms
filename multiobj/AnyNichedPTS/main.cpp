#include <string>
#include <vector>
#include <list>
#include <utility> // std::pair, std::make_pair
#include <ctime> // for measure elapsed time
#include <algorithm>

#include <iostream>
#include <sstream>
#include <fstream>

#include "../lib/readdata.h"
#include "AnyNichedPTS.h"

using std::string;
using std::vector;
using std::list;
using std::pair;

void writedata(int n_objs, 
               int n_facs,
               string &instance, 
               vector<SolutionTS*> &non_dominated_set,
               time_eval &p);

void read_parameters(char const *argv[], params &p);

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

        params p;
        read_parameters(argv, p);
        return 0;
        //p.print();

        AnyNichedPTS pts(n_facs, n_objs, &dist_mat, &flow_mats, p);
        vector<SolutionTS*> non_dominated_set;

        bool num_avals_crit = true;
        int num_avals = 50;
        float time_limit = 500;
        time_eval t = pts.init(num_avals_crit, num_avals, time_limit, non_dominated_set);

        std::sort(non_dominated_set.begin(), non_dominated_set.end(), order);

        writedata(n_objs, n_facs, instance, non_dominated_set, t);
    }
    return 0;
}

void read_parameters(char const *argv[], params &p)
{
    std::stringstream strv2, strv3, strv4, strv5;

    strv2 << argv[2];
    strv2 >> p.min_tabu_list; //std::cout << "min_tabu_list = " << p.min_tabu_list << "\n";

    strv3 << argv[3];
    strv3 >> p.delta_tabu; //std::cout << "delta_tabu = " << p.delta_tabu << "\n";

    strv4 << argv[4];
    strv4 >> p.size_arc_init; //std::cout << "size_arc_init = " << p.size_arc_init << "\n";

    strv5 << argv[5];
    strv5 >> p.refset_size; //std::cout << "refset_size = " << p.refset_size << "\n";
}

void writedata(int n_objs, 
               int n_facs,
               string &instance, 
               vector<SolutionTS*> &non_dominated_set,
               time_eval &p)
{
    /*----------------------- PRINT NON-DOMINATED SET -----------------------*/
    string outf = "../ANPTSresults/";
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
        SolutionTS *sol = non_dominated_set[i];
        for (auto pi : sol->p)
        {
            outfile << pi << " ";
        }
        outfile << "\n";
        for (auto obj : sol->objs)
        {
            outfile << obj << " ";
        }
        outfile << "\n";
    }

    outfile << "--------------------\n";

    outfile.close();
    /*----------------------- PRINT NON-DOMINATED SET -----------------------*/
}