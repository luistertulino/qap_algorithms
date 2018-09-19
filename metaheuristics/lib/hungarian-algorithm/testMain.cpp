#include <iostream>
#include "Hungarian.h"


int main(void)
{
    // please use "-std=c++11" for this initialization of vector.
	vector< vector<double> > costMatrix = { { 297, 368, 244, 156 }, 
										  { 174, 254, 131, 82 }, 
										  { 293, 353, 245, 161 }, 
										  { 152, 217, 93, 73 } };

	HungarianAlgorithm HungAlgo;
	vector<int> assignment;

	double cost = HungAlgo.Solve(costMatrix, assignment);

	for (unsigned int x = 0; x < costMatrix.size(); x++)
		std::cout << x << "," << assignment[x] << "\t";

	std::cout << "\ncost: " << cost << std::endl;

	return 0;
}
