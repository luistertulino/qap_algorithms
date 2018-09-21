#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "../lib/readdata.h"

using std::string;
using std::vector;

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        string file(argv[1]);
        file = "../instances/"+file;

        int n_facs, n_objs;
        Dist_Matrix dist_mat;
        Flow_Matrices flow_mats;

        read_data(file, n_facs, n_objs, dist_mat, flow_mats);
    }
    return 0;
}