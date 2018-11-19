#include "../lib/definitions.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

using std::vector;

class solution
{
  public:
    int n_facs;
    vector<int> p; // Permutation of QAP. p[i] = j indicates that item i is allocated on place j
    long cost; // Cost of that solution

    solution(){}

    solution(int n)
    {
        p.resize(n);
        n_facs = n;
        for(int i = 0; i < n; i++) p[i] = i;
        cost = 0;
    }

    solution& operator=(solution &s)
    {
        n_facs = s.n_facs;
        p = s.p;
        cost = s.cost;

        return *this;
    }

    void resize(int n)
    {
        p.resize(n);
        n_facs = n;
        for(int i = 0; i < n; i++) p[i] = i;
        cost = 0;
    }

    void shuffle(std::mt19937 &g){ std::shuffle(p.begin(), p.end(), g); }

    void comp_cost(Matrix &flows, Matrix &distances)
    {
        cost = 0;
        for(int i = 0; i < n_facs; i++)
        {
            for(int j = 0; j < n_facs; j++)
                cost += flows[i][j] * distances[ p[i] ][ p[j] ];
        }
    }

    void print()
    {
        std::cout << "Permutation: ";
        for (int i = 0; i < n_facs; ++i)
        {
            std::cout << i << "->" << p[i] << " ";
        }
        std::cout << "\n";
        std::cout << "Cost: " << cost << "\n";
    }

    void random_restart(Matrix &alloc_count, std::mt19937 &gen)
    {
        int items[n_facs]; int remaining = n_facs;
        for(int i = 0; i < n_facs; i++) items[i] = i;
        vector<int> reverse_count(remaining,0);

        for(int i = 0; i < n_facs; i++) // For each item, chooses its location
        {
            /* 
                The probability that a location j is selected to item i is reverse to the number of times
                that the allocation i->j was made.
            */
            int sum = 0;
            for(int j = 0; j < remaining; j++) sum += alloc_count[i][items[j]];

            for(int j = 0; j < remaining; j++) reverse_count[j] = sum - alloc_count[i][items[j]];

            std::discrete_distribution<int> dist(reverse_count.begin(), reverse_count.end());
            int selected = dist(gen);
            p[i] = items[selected];

            // remove that location of possible ones
            std::swap(items[selected], items[remaining-1]);
            remaining--;
            reverse_count.pop_back();
        }
    }
};