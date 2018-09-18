#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <vector>

class Solution
{
  public:
  	std::vector<int> permutation;
  	std::vector<int> objs;

  public:
	Solution();
	Solution(int n_facs, int n_objs){
		permutation.resize(n_facs);
		objs.resize(n_objs);
	}
	Solution(int perm[], int _objs[], int n_facs, int n_objs){
		permutation = std::vector<int>(perm, perm + n_facs);
		objs = std::vector<int>(_objs, _objs + n_objs);
	}
	~Solution();
};

bool operator<=(Solution &s1, Solution &s2){
	for (int i = 0; i < objs.size(); ++i)
	{
		if(s1.objs[i] > s2.objs[i])
			return false;
	}
	return true
}

bool incomparable(Solution &s1, Solution &s2){
	return !(s1 <= s2) and !(s2 <= s1);
}

#endif