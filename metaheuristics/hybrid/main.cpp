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
	2 - Minimum tabu list size: 10, 20, 30
	3 - Variation of tabu list size: 0, 5, 10, 15, 20
*/
int main(int argc, char const *argv[])
{
	prob_params params;
	Matrix dist_mat;
	Matrix flow_mat;

    if (argc > 1)
	{
		string file(argv[1]);
		string pfile = "../instances/"+file+".dat";
		string sfile = "../solutions/"+file+".sln";
		string rfile = "results/"+file+".out";

		int n_facs;

		if (file.find("chr") != std::string::npos or file.find("scr") != std::string::npos)
		{
			int read = read_data(pfile, n_facs, flow_mat, dist_mat);
			if (read != READING_OK)
			{
				std::cout << "Error in reading instance " << file << ".\n";
				return -1;
			}
		}
		else{
			int read = read_data(pfile, params.n_facs, dist_mat, flow_mat);
			if (read != READING_OK)
			{
				std::cout << "Error in reading instance " << file << ".\n";
				return -1;
			}
		}

		std::cout << params.n_facs << "\n";

		int sol = read_solution(sfile);
		if (sol == ERROR_READING_DATA)
		{
			std::cout << "Error in reading solution of " << file << ".\n";
			return -1;
		}

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

	return 0;
}

bool read_parameters(char const *argv[], prob_params &params)
{
	std::stringstream strv2, strv3;

	strv2 << argv[2];
	int min_tabu; strv2 >> min_tabu; //std::cout << "min_tabu: " << min_tabu << "\n";

	strv3 << argv[3];
	int delta; strv3 >> delta; //std::cout << "delta: " << delta << "\n";

	return true;
}