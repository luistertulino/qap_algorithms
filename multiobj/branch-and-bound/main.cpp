#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility> // std::pair, std::make_pair
#include <fstream>

#include "../lib/definitions.h"
#include "../lib/readdata.h"
#include "BNB.h"

using std::string;
using std::vector;
using std::list;
using std::pair;

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        string file(argv[1]);
        file = "../instances/"+file;

        int n_facs, n_objs;
        DistMatrix dist_mat;
        FlowMatrices flow_mats;

        read_data(file, n_facs, n_objs, dist_mat, flow_mats);

        BNB bnb(n_facs, n_objs, &dist_mat, &flow_mats);
        list<Solution> non_dominated_set;
        pair<int,int> count_nodes = bnb.init(non_dominated_set);
    }
    return 0;
}