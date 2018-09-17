#ifndef READDATA_H_
#define READDATA_H_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using std::string;
using std::vector;

typedef vector< vector< vector<int> > > Flow_Matrices;

void read_data(string &file, int &n_facs, int &n_objs, int dist_matrix[], Flow_Matrices &f_mats)
{
    std::ifstream problem_file(file std::ios::in);

    if(problem_file.is_open())
    {
        string line = "";
        if (std::getline(problem_file, line, '\n'))
        {
            std::vector< std::string > words;
            std::string delimiter = " ";
            split_string(line, delimiter, words);

            n_facs = std::stoi(words[0], nullptr);
            n_objs = std::stoi(words[1], nullptr);
        }
    }
}

#endif