#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm> // For std::sort
#include <cmath> // For std::round

#include "../lib/definitions.h"
#include "../lib/readdata.h"
//#include "../lib/hungarian-algorithm/Hungarian.h"
//#include "../lib/libhungarian/hungarian.h"

using std::string;
using std::vector;
using std::sort;

struct prob_params
{
	int pop_size;
	double evap_fac;
	int rots_its;
	int min_tabu_list;
	int delta;
};

bool read_parameters(char const *argv[], prob_params &params);

/*
	PARAMETERS OF THE ALGORITHM:
	1 - Instance name
	2 - Population size: n, 2n, 5n, 10n; n -> problem size
	3 - Evaporation factor: real value on the interval (0,1)
	4 - Initial RoTS number of iterations: 1000, 5000, 10000
	5 - Minimum tabu list size: 10, 20, 30
	6 - Variation of tabu list size: 0, 5, 10, 15, 20
*/
int main(int argc, char const *argv[])
{
    if (argc > 1)
	{
		string file(argv[1]);
		string pfile = "../instances/"+file+".dat";
		string sfile = "../solutions/"+file+".sln";
		string rfile = "results/"+file+".out";

		int n_facs;
		Matrix dist_mat;
		Matrix flow_mat;

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
			int read =read_data(pfile, n_facs, dist_mat, flow_mat);
			if (read != READING_OK)
			{
				std::cout << "Error in reading instance " << file << ".\n";
				return -1;
			}
		}

		int sol = read_solution(sfile);
		if (sol == ERROR_READING_DATA)
		{
			std::cout << "Error in reading solution of " << file << ".\n";
			return -1;
		}

		prob_params params;
		if(not read_parameters(argv, params))
		{
			std::cout << "Error in reading parameters. Proper formatting is:\n";
			std::cout << "./exec instance_name pop_size evap_factor rots_its min_tabu_list delta\n";
			std::cout << "pop_size: size of population\n";
			std::cout << "evap_factor: evaporation factor of pheromones\n";
			std::cout << "rots_its: initial RoTS number of iterations\n";
			std::cout << "min_tabu_list: minimum tabu list size\n";
			std::cout << "delta: Variation of tabu list size\n";

			return -1;
		}
        
        
	}
	else{

	}
	return 0;
}

bool read_parameters(char const *argv[], prob_params &params)
{
	stringstream strv;

	strv << argv[2];
	int psize; strv >> psize; std::cout << "psize: " << psize << "\n";

	strv << argv[3];
	double evap; strv >> evap;
	if(not (evap > 0.0 and evap < 1.0))
	{
		std::cout << "evap_fac value not valid: " << evap << "\n"
		return false;
	}

	strv << argv[4];
	int rots_its; strv >> rots_its; std::cout << "rots_its: " << rots_its << "\n";

	strv << argv[5];
	int min_tabu; strv >> min_tabu; std::cout << "min_tabu: " << min_tabu << "\n";

	strv << argv[6];
	int delta; strv >> delta; std::cout << "delta: " << delta << "\n";
}