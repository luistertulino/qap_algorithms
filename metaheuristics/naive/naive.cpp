#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm> // For std::sort

#include "../lib/readdata.h"


#define last(vec) vec.size()-1


using std::string;
using std::vector;
using std::sort;

void computeLAPmatrix(int n_facs, Matrix &dist_mat, Matrix &flow_mat, Matrix &lapmatrix);
int scalar(int i, int j, Matrix &A, Matrix &B);

int main(int argc, char const *argv[])
{
    if (argc > 1)
	{
		string file(argv[1]);
		file = "../instances/"+file;

		int n_facs;
		Matrix dist_mat;
		Matrix flow_mat;

		read_data(file, n_facs, dist_mat, flow_mat);

		Matrix lapmatrix;
		computeLAPmatrix(n_facs, dist_mat, flow_mat, lapmatrix);
	}
	else{

	}
	return 0;
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

void computeLAPmatrix(int n_facs, Matrix &dist_mat, Matrix &flow_mat, Matrix &lapmatrix)
{
	Matrix A(flow_mat), B(dist_mat);

	std::cout << "A:\n"; print_matrix(A); std::cout << "B:\n"; print_matrix(B);
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

	std::cout << "depois de remover diagonais e ordenar\n\n"; std::cout << "A:\n"; print_matrix(A); std::cout << "B:\n"; print_matrix(B);


	for (int i = 0; i < n_facs; ++i)
	{
		for (int j = 0; j < n_facs; ++j)
		{
			int lij = flow_mat[i][i]*dist_mat[j][j];
			std::cout << "l_"<<i<<j<<": "<<lij<<" ";
			lij += scalar(i, j, A, B);
			std::cout << "l_"<<i<<j<<": "<<lij<<"\n";
			lapmatrix[i][j] = lij;
		}
		std::cout << "\n";
	}

	// print Lij matrix
	std::cout << "print Lij matrix\n\n";
	
}

int scalar(int i, int j, Matrix &A, Matrix &B){
	int sp = 0;

	for (int k = 0; k < A[i].size(); ++k)
	{
		std::cout << A[i][k] << "*" << B[j][last(B[j])-k] << " + ";
		sp += A[i][k] * B[j][last(B[j])-k];
	}
	std::cout << "// ";
	return sp;
}