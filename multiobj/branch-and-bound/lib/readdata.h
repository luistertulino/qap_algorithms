#ifndef READDATA_H_
#define READDATA_H_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using std::string;
using std::vector;

typedef vector< vector<int> > Dist_Matrix;
typedef vector< vector< vector<int> > > Flow_Matrices;

void split_string(std::string &str, std::string &delimiter, std::vector< std::string > &words){
    // Splits a string based on a delimiter

    int i = 0;
    size_t pos = str.find(delimiter);
    std::string buff = "";

    while (pos != std::string::npos) {
        buff = str.substr(i,pos-i);

        if(!buff.empty()){
            words.push_back(buff);
        }

        i = ++pos;
        pos = str.find(delimiter, pos);

        if (pos == std::string::npos) {
            buff = str.substr(i,str.length());

            if(!buff.empty()){
                words.push_back(buff);
            }
        }
    }
}

void printdata(int &n_facs, int &n_objs, Dist_Matrix &dist_mat, Flow_Matrices &flow_mats);

int read_data(string &file, int &n_facs, int &n_objs, Dist_Matrix &dist_mat, Flow_Matrices &flow_mats)
{
    std::vector< std::string > words;
    std::string delimiter = " ";

    std::ifstream problem_file(file, std::ios::in);

    if(problem_file.is_open())
    {
        string line = "";
        if (std::getline(problem_file, line, '\n')) // read problem size and num of objectives
        {            
            split_string(line, delimiter, words);

            n_facs = std::stoi(words[0], nullptr);
            n_objs = std::stoi(words[1], nullptr);
            std::cout << n_facs << " " << n_objs << "\n";
        }
        else{
            std::cerr << "error in reading n and k of" << file << " file\n";
            return -1;
        }

        /* -------------- Read distance matrix -------------- */
        dist_mat.resize(n_facs);
        for (int i = 0; i < n_facs; ++i)
        {
            dist_mat[i].resize(n_facs);

            if (std::getline(problem_file, line, '\n')){
                words.clear();
                split_string(line, delimiter, words);
            }
            else{
                std::cerr << "error in Dist_Matrix in " << file << " file\n";
                return -1;
            }

            for (int j = 0; j < n_facs; ++j)
            {            
                int dij = std::stoi(words[j], nullptr);
                dist_mat[i][j] = dij;                
                
            }
        }

        /* -------------- Read flow matrices -------------- */
        flow_mats.resize(n_objs);
        for (int k = 0; k < n_objs; ++k)
        {
            flow_mats[k].resize(n_facs);
            for (int i = 0; i < n_facs; ++i)
            {
                flow_mats[k][i].resize(n_facs);


                if (std::getline(problem_file, line, '\n')){
                    words.clear();
                    split_string(line, delimiter, words);
                }
                else{
                    std::cerr << "error in Flow_Matrix " << k <<" in " << file << " file\n";
                    return -1;
                }

                for (int j = 0; j < n_facs; ++j)
                {            
                    int f_kij = std::stoi(words[j], nullptr);
                    flow_mats[k][i][j] = f_kij;               
                    
                }
            }
        }
    }
    else{
        std::cerr << "error in opening " << file << " file\n";
        return -1;
    }
    printdata(n_facs, n_objs, dist_mat, flow_mats);
}

void printdata(int &n_facs, int &n_objs, Dist_Matrix &dist_mat, Flow_Matrices &flow_mats){
    std::cout << n_facs << " " << n_objs << "\n";

    std::cout << "Dist_Matrix:\n";
    for (int i = 0; i < n_facs; ++i)
    {
        for (int j = 0; j < n_facs; ++j)
        {
            std::cout << dist_mat[i][j] << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\nFlow matrices:\n";

    for (int k = 0; k < n_objs; ++k)
    {
        std::cout << "k = " << k << "\n";
        for (int i = 0; i < n_facs; ++i)
        {
            for (int j = 0; j < n_facs; ++j)
            {
                std::cout << flow_mats[k][i][j] << " ";
            }
            std::cout << "\n";
        }
    }
}

#endif