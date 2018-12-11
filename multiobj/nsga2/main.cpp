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
#include "Variator.h"

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

        if(p.generated_solutions_num < p.pop_size)
        {
            std::cerr << "Number of solutions to be generated not enough to fill population.\n";
            return -1;
        }
        if(p.parents_num > p.pop_size)
        {
            std::cerr << "Number of parents bigger than population size!\n";
            return -1;
        }
        if(p.offspring_size > p.pop_size)
        {
            std::cerr << "Number of offspring bigger than population size!\n";
            return -1;
        }
        if(p.diversification_prob + p.intensification_prob != 1.0)
        {
            std::cerr << "Sum of probabilities is not 1.0. Fix probabilities parameters!\n";
            return -1;
        }

        vector<Individual*> non_dominated_set;
        Variator var(n_facs, n_objs, p, &dist_mat, &flow_mats);
        var.init(non_dominated_set);

        //std::sort(non_dominated_set.begin(), non_dominated_set.end(), order);
        //writedata(n_objs, n_facs, instance, non_dominated_set, t);
    }
    return 0;
}

void read_parameters(char const *argv[], params &p)
{
    std::stringstream strv2, strv3, strv4, strv5, strv6, strv7;

    strv2 << argv[2];
    strv2 >> p.generated_solutions_num;

    strv3 << argv[3];
    strv3 >> p.pop_size;

    strv4 << argv[4];
    strv4 >> p.parents_num;

    strv5 << argv[5];
    strv5 >> p.offspring_size;

    strv6 << argv[6];
    strv6 >> p.diversification_prob;

    strv7 << argv[7];
    strv7 >> p.intensification_prob;
}