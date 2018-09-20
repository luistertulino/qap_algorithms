#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm> // For std::sort
#include <cmath> // For std::round

#include "../lib/readdata.h"
//#include "../lib/hungarian-algorithm/Hungarian.h"
#include "../lib/libhungarian/hungarian.h"

#define last(vec) vec.size()-1


using std::string;
using std::vector;
using std::sort;

int heuristic(int n_facs, Matrix &dist_mat, Matrix &flow_mat);
int computeLAP(int n_facs, Matrix &lapmatrix, vector<int> &assignment);
void computeLAPmatrix(int n_facs, Matrix &dist_mat, Matrix &flow_mat, Matrix &lapmatrix);
int scalar(int i, int j, Matrix &A, Matrix &B);
void print_matrix(Matrix &A);
int** vector_to_matrix(Matrix &mat);

int main(int argc, char const *argv[])
{
    if (argc > 1)
	{
		string file(argv[1]);
		string pfile = "../instances/"+file+".dat";
		string sfile = "../solutions/"+file+".sln";


		int n_facs;
		Matrix dist_mat;
		Matrix flow_mat;

		if (file.find("chr") != std::string::npos or file.find("scr") != std::string::npos)
		{
			read_data(pfile, n_facs, flow_mat, dist_mat);
		}
		else{
			read_data(pfile, n_facs, dist_mat, flow_mat);
		}

		int sol = read_solution(sfile);
        
        int obj = heuristic(n_facs, dist_mat, flow_mat);

        string results_file = "results.txt";
        write_results(results_file, file, obj, sol);
	}
	else{

	}
	return 0;
}

int heuristic(int n_facs, Matrix &dist_mat, Matrix &flow_mat){
	Matrix lapmatrix;
	computeLAPmatrix(n_facs, dist_mat, flow_mat, lapmatrix);

	vector<int> assignment(n_facs);
	int lowerbound = computeLAP(n_facs, lapmatrix, assignment);

	// assignment[i] represents to where facility i was assigned

	int obj = 0;

	for (int i = 0; i < n_facs; ++i)
	{
		for (int j = 0; j < n_facs; ++j)
		{
			int pi = assignment[i];
			int pj = assignment[j];
			obj += flow_mat[i][j] * dist_mat[pi][pj];
		}
	}

	/*std::cout << "lapmatrix:\n";
	print_matrix(lapmatrix);
	std::cout << "assignment: \n";
	for (int i = 0; i < n_facs; ++i)
	{
		std::cout << "facility " << i << " to location " << assignment[i] << "\n";
	}
	std::cout << "lowerbound = " << lowerbound << "\n";
    std::cout << "quad cost: " << obj;*/

	return obj;
}

int computeLAP(int n_facs, Matrix &lapmatrix, vector<int> &assignment){
	//HungarianAlgorithm HungAlgo;
	hungarian_problem_t p;

	int** m = vector_to_matrix(lapmatrix);

	int matrix_size = hungarian_init(&p, m, n_facs, n_facs, HUNGARIAN_MODE_MINIMIZE_COST) ;
	//hungarian_print_costmatrix(&p);
	hungarian_solve(&p);
	//hungarian_print_assignment(&p);

    int lb = 0;
    for (int i = 0; i < n_facs; ++i)
    {
        for (int j = 0; j < n_facs; ++j)
        {
            if (p.assignment[i][j] == 1)
            {
                assignment[i] = j;
                lb += lapmatrix[i][j];
                break;
            }
        }
    }

    /* ------- FREE THE USED MEMORY ------- */
    hungarian_free(&p);

	  int idx;
	  for (idx=0; idx < 4; idx+=1) {
	    free(m[idx]);
	  }
	  free(m);
    /* ------- FREE THE USED MEMORY ------- */


	/*{
		vector< vector<double> > double_matrix(n_facs, vector<double>(n_facs));
		for (int i = 0; i < n_facs; ++i)
		{
			for (int j = 0; j < n_facs; ++j)
			{
				double_matrix[i][j] = lapmatrix[i][j];
			}
		}
        double lb = HungAlgo.Solve(double_matrix, assignment);
	}*/
	return std::round(lb);
}

void computeLAPmatrix(int n_facs, Matrix &dist_mat, Matrix &flow_mat, Matrix &lapmatrix)
{
	Matrix A(flow_mat), B(dist_mat);

	////std::cout << "A:\n"; print_matrix(A); //std::cout << "B:\n"; print_matrix(B);
	lapmatrix.resize(n_facs);
	for (int i = 0; i < n_facs; ++i)
		lapmatrix[i].resize(n_facs);

	for (int i = 0; i < n_facs; ++i)
	{
		/* Remove A_ii and B_ii */
			int aux = A[i][i]; A[i][i] = A[i][last(A)]; A[i][last(A)] = aux;
			A[i].pop_back();
			aux = B[i][i]; B[i][i] = B[i][last(B)]; B[i][last(B)] = aux;
			B[i].pop_back();
		/* Remove A_ii and B_ii */

		sort(B[i].begin(), B[i].end());
		sort(A[i].begin(), A[i].end());
	}

	////std::cout << "depois de remover diagonais e ordenar\n\n"; //std::cout << "A:\n"; print_matrix(A); //std::cout << "B:\n"; print_matrix(B);


	for (int i = 0; i < n_facs; ++i)
	{
		for (int j = 0; j < n_facs; ++j)
		{
			int lij = flow_mat[i][i]*dist_mat[j][j];
			////std::cout << "l_"<<i<<j<<": "<<lij<<" ";
			lij += scalar(i, j, A, B);
			////std::cout << "l_"<<i<<j<<": "<<lij<<"\n";
			lapmatrix[i][j] = lij;
		}
		////std::cout << "\n";
	}

	// print Lij matrix
	////std::cout << "print Lij matrix\n\n";
	
}

int scalar(int i, int j, Matrix &A, Matrix &B){
	int sp = 0;

	for (int k = 0; k < A[i].size(); ++k)
	{
		//std::cout << A[i][k] << "*" << B[j][last(B[j])-k] << " + ";
		sp += A[i][k] * B[j][last(B[j])-k];
	}
	////std::cout << "// ";
	return sp;
}

int** vector_to_matrix(Matrix &mat) {
  int i,j;
  int** r;
  int rows = mat.size();
  int cols = mat[0].size();
  r = (int**)calloc(rows,sizeof(int*));
  for(i=0;i<rows;i++)
  {
    r[i] = (int*)calloc(cols,sizeof(int));
    for(j=0;j<cols;j++)
      r[i][j] = mat[i][j];
  }
  return r;
}

void print_matrix(Matrix &A){
	for (int i = 0; i < A.size(); ++i)
	{
		for (int j = 0; j < A[i].size(); ++j)
		{
			std::cout << A[i][j] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}