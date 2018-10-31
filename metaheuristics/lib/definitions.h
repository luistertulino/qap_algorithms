#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <vector>

using std::vector;

#define last(vec) vec.size()-1

typedef vector< vector<int> > Matrix;
typedef vector< vector<long> > MatrixLong;

struct prob_params
{
	int n_facs;
	int min_tabu_list;
	int delta;
};

#endif