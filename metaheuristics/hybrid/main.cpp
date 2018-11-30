#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm> // For std::sort
#include <cmath> // For std::round

#include "../lib/readdata.h"
#include "HybridTS.h"

using std::string;
using std::vector;
using std::sort;

bool read_parameters(char const *argv[], prob_params &params);

/*
    PARAMETERS OF THE ALGORITHM:
    1 - Instance name
    2 - Minimum tabu list size (values: 5, 10, 15, 20, 30)
    3 - Variation of tabu list size (values: 0, 5, 10, 15, 20)
    4 - Maximum number of failures (values: 5, 6, 7, 8, 9, 10)
    5 - Threshold for using second aspiration function (values: 30, 40, 50, 60, 70)
    6 - Extra iterations for second aspiration function(values: 10, 15, 20, 30, 40)
*/
int main(int argc, char const *argv[])
{
    prob_params params;
    Matrix dist_mat;
    Matrix flow_mat;

    string file;

    if (argc > 1)
    {
        file = string(argv[1]);
        std::cout << file << "\n";
        string sfile = file;
        size_t index = 0; index = sfile.find("instances", index);
        sfile.replace(index, 9, "solutions");
        index = sfile.find("dat", index);
        sfile.replace(index, 3, "sln");
        
        //string rfile = "results/"+file+".out";

        int n_facs;

        if (file.find("chr") != std::string::npos or file.find("scr") != std::string::npos)
        {
            int read = read_data(file, params.n_facs, flow_mat, dist_mat);
            if (read != READING_OK)
            {
                std::cout << "Error in reading instance " << file << ".\n";
                return -1;
            }
        }
        else{
            int read = read_data(file, params.n_facs, dist_mat, flow_mat);
            if (read != READING_OK)
            {
                std::cout << "Error in reading instance " << file << ".\n";
                return -1;
            }
        }

        std::cout << "read_solution\n";
        long sol = read_solution(sfile);
        std::cout << "read_solution\n";
        if (sol == ERROR_READING_DATA)
        {
            std::cout << "Error in reading solution of " << file << ".\n";
            return -1;
        }
        params.qaplib_sol = sol;

        if(not read_parameters(argv, params))
        {
            std::cout << "Error in reading parameters. Proper formatting is:\n";
            std::cout << "./exec instance_name min_tabu_list delta\n";
            std::cout << "min_tabu_list: minimum tabu list size\n";
            std::cout << "delta: Variation of tabu list size\n";

            return -1;
        }
    }
    else
    {
        std::cout << "Error in reading parameters. Proper formatting is:\n";
        std::cout << "./exec instance_name min_tabu_list delta\n";
        std::cout << "min_tabu_list: minimum tabu list size\n";
        std::cout << "delta: Variation of tabu list size\n";

        return -1;
    }

    

    HybridTS ts(params, dist_mat, flow_mat);
    ts.init();
    ts.write_results(file);

    return 0;
}

bool read_parameters(char const *argv[], prob_params &params)
{
    std::stringstream strv2, strv3, strv4, strv5, strv6, strv7;

    strv2 << argv[2];
    strv2 >> params.min_tabu_list; //std::cout << "min_tabu: " << params.min_tabu_list << "\n";

    strv3 << argv[3];
    strv3 >> params.delta; //std::cout << "delta: " << params.delta << "\n";

    strv4 << argv[4];
    strv4 >> params.max_fails; //std::cout << "max_fails: " << params.max_fails << "\n";

    strv5 << argv[5];
    strv5 >> params.threshold; //std::cout << "threshold: " << params.threshold << "\n";

    strv6 << argv[6];
    strv6 >> params.aspiration; //std::cout << "aspiration: " << params.aspiration << "\n";

    strv7 << argv[7];
    strv6 >> params.max_iter;

    return true;
}
