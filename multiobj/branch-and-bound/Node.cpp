#include "Node.h"
#include "../lib/libhungarian/hungarian.h"

#include <vector>
#include <set>
#include <stdlib.h>
#include <algorithm>
#include <iostream>

using std::vector;
using std::set;
using std::sort;

int** alloc_table(int size)
{
    int** table;

    table = (int**)calloc(size,sizeof(int*));

    for (int i = 0; i < size; ++i)
    {
        table[i] =  (int*)calloc(size,sizeof(int));
        for (int j = 0; j < size; ++j)
        {
            table[i][j] = 0;
        }
    }

    return table;
}

int scalar(int i, int j, vector< vector<int> > &A, vector< vector<int> > &B)
{
    int sp = 0;

    for (int k = 0; k < A[i].size(); ++k)
    {
        //std::cout << A[i][k] << "*" << B[j][last(B[j])-k] << " + ";
        sp += A[i][k] * B[j][last(B[j])-k];
    }
    ////std::cout << "// ";
    return sp;
}

bool operator<=(Solution &s, Node &n)
{
    for (int i = 0; i < n.n_objs; ++i)
    {
        if(s.objs[i] > n.lower_bound[i])
            return false;
    }
    return true;
}

void Node::compute_lowerbound(DistMatrix &dist_mat, FlowMatrices &flow_mats)
{
    int re_size = remaining_locations.size();

    //print();
    // We compute the lower bound for each objective
    // In the end, the lb will be the ideial point formed by each objective value
    for (int k = 0; k < n_objs; ++k)
    {
        // First, we compute the cost of the pre-assigned items
        lower_bound[k] = 0;
        for (int i = 0; i <= last_item; ++i)
        {
            for (int j = 0; j <= last_item; ++j)
            {
                int pi = partial_assignment[i];
                int pj = partial_assignment[j];
                lower_bound[k] += flow_mats[k][i][j] * dist_mat[pi][pj];
            }
        }
        //std::cout << "l_b["<<k<<"]="<<lower_bound[k]<<"--LAP--";

        // Then, we build a matrix L, and use it for computing Gilmore-Lawler bound

        if(re_size == 0) continue;

        int** L;
        L = alloc_table(re_size);

        vector< vector<int> > D; D.resize(re_size);
        vector< vector<int> > F; F.resize(re_size);

        // Here, we consider only the items not assigned and the locations not used
        int a = 0;
        for (int i = last_item+1; i < n_facs; ++i)
        {
            D[a].resize(re_size); F[a].resize(re_size);

            int b = 0;
            for (auto j : remaining_locations)
            {
                F[a][b] = flow_mats[k][i][j];
                D[a][b] = dist_mat[i][j];

                b += 1;
            }
            a += 1;
        }

        for (a = 0; a < re_size; a++)
        {
            // The elements D/F[a][a] are not considered in the min scalar product, sO we remove it efficiently
            int aux = F[a][a]; F[a][a] = F[a][last(F[a])]; F[a][last(F[a])] = aux; F[a].pop_back();
            aux = D[a][a]; D[a][a] = D[a][last(D[a])]; D[a][last(D[a])] = aux; D[a].pop_back();

            sort(D[a].begin(), D[a].end());
            sort(F[a].begin(), F[a].end());
        }

        for (a = 0; a < re_size; a++)
        {
            for (int b = 0; b < re_size; b++)
            {
                int lab = F[a][a] * D[b][b];
                lab += scalar(a, b, F, D);
                L[a][b] = lab;
            }
        }

        // After computing the L matriz for the k-th objective, we solve the corresponding LAP problem
        hungarian_problem_t p;
        int matrix_size = hungarian_init(&p, L, re_size, re_size, HUNGARIAN_MODE_MINIMIZE_COST);

        hungarian_solve(&p);

        int lb = 0;
        for (int i = 0; i < re_size; ++i)
        {
            for (int j = 0; j < re_size; ++j)
            {
                if (p.assignment[i][j] == 1)
                {
                    //assignment[i] = j;
                    lb += L[i][j];
                    break;
                }
            }
        }

        lower_bound[k] += lb;
        //std::cout << "l_b["<<k<<"]="<<lower_bound[k]<<"\n";

        /* ------- FREE THE USED MEMORY ------- */
        hungarian_free(&p);

          int idx;
          for (idx=0; idx < re_size; idx+=1) {
            free(L[idx]);
          }
          free(L);
        /* ------- FREE THE USED MEMORY ------- */

    }
    //std::cout << "\n-----------------------------------------------------------\n";   
}

void Node::print()
{
    std::cout << "-----------------------------------------------------------\n";
    std::cout << "Last item assigned: " << last_item << "\n";
    
    std::cout << "Partial assignment: ";
    for(int i=0; i <= last_item; i++){
        std::cout << i << "-" << partial_assignment[i] << ", ";
    } std::cout << "\n";

    std::cout << "Remaining locations: ";
    for(auto it : remaining_locations){
        std::cout << it << ", ";
    } std::cout << "\n";

    /*std::cout << "Lower bound: ";
    for(auto i : lower_bound){
        std::cout << i << ", ";
    }*/

    //std::cout << "\n-----------------------------------------------------------\n";
}