#ifndef READDATA_H_
#define READDATA_H_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <sstream>
#include <iterator>

using std::string;
using std::vector;

#define ERROR_READING_DATA -1
#define READING_OK 0

typedef vector< vector<int> > Matrix;

void split_string(std::string &str, std::string &delimiter, std::vector< std::string > &words)
{
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

void split_string(std::string &str, std::vector< std::string > &words)
{
    std::istringstream buf(str);
    std::istream_iterator<std::string> beg(buf), end;

    words = std::vector<std::string>(beg, end);
    //std::vector<std::string> tokens(beg, end);
}

void printdata(int &n_facs, Matrix &dist_mat, Matrix &flow_mat);

int read_data(string &file, int &n_facs, Matrix &dist_mat, Matrix &flow_mat)
{
    std::vector< std::string > words;
    std::string delimiter = " ";

    std::ifstream problem_file(file, std::ios::in);

    if(problem_file.is_open())
    {
        string line = "";
        if (std::getline(problem_file, line, '\n')) // read problem size and num of objectives
        {            
            //split_string(line, delimiter, words);
            split_string(line, words);

            n_facs = std::stoi(words[0], nullptr);
        }
        else{
            std::cerr << "error in reading n of" << file << " file\n";
            return ERROR_READING_DATA;
        }
        if (std::getline(problem_file, line, '\n')){
            if (line == "") std::cout << "linha em branco\n";
        } 

        /* -------------- Read distance matrix -------------- */
        dist_mat.resize(n_facs);
        for (int i = 0; i < n_facs; ++i)
        {
            dist_mat[i].resize(n_facs);

            if (std::getline(problem_file, line, '\n')){
                words.clear();
                //split_string(line, delimiter, words);
                split_string(line, words);
            }
            else{
                std::cerr << "error in Dist_Matrix in " << file << " file\n";
                return ERROR_READING_DATA;
            }

            for (int j = 0; j < n_facs; ++j)
            {            
                int dij = std::stoi(words[j], nullptr);
                dist_mat[i][j] = dij;                
                
            }
        }

        if (std::getline(problem_file, line, '\n')){
            if (line == "") std::cout << "linha em branco\n";
        }

        /* -------------- Read flow matrix -------------- */        
        flow_mat.resize(n_facs);
        for (int i = 0; i < n_facs; ++i)
        {
            flow_mat[i].resize(n_facs);

            if (std::getline(problem_file, line, '\n')){
                words.clear();
                //split_string(line, delimiter, words);
                split_string(line, words);
            }
            else{
                std::cerr << "error in Flow_Matrix in " << file << " file\n";
                return ERROR_READING_DATA;
            }

            for (int j = 0; j < n_facs; ++j)
            {            
                int f_ij = std::stoi(words[j], nullptr);
                flow_mat[i][j] = f_ij;               
                
            }
        }

        problem_file.close();
        
    }
    else{
        std::cerr << "error in opening " << file << " file\n";
        return ERROR_READING_DATA;
    }
    //printdata(n_facs, dist_mat, flow_mat);
    return READING_OK;
}

int read_solution(string &file){
    
    std::ifstream solution_file(file, std::ios::in);

    if (solution_file.is_open())
    {
        string line = "";
        std::vector< std::string > words;
        if (std::getline(solution_file, line, '\n')) // read problem size and num of objectives
        {            
            //split_string(line, delimiter, words);
            split_string(line, words);

            int sol = std::stoi(words[1], nullptr);
            return sol;
        }

        solution_file.close();
    }
    else return ERROR_READING_DATA;

}

void write_results(string &results_file, string &instance, int obj, int sol){
    std::ofstream outfile;
    outfile.open(results_file, std::ios::app);

    if (outfile.is_open())
    {
        outfile << instance << " " << obj << " ";
        if (sol != -1)
        {
            outfile << sol << "\n";
        } 
    }
}

// Function just for testing
void printdata(int &n_facs, Matrix &dist_mat, Matrix &flow_mat)
{
    std::cout << n_facs << "\n";

    std::cout << "Dist_Matrix:\n";
    for (int i = 0; i < n_facs; ++i)
    {
        for (int j = 0; j < n_facs; ++j)
        {
            std::cout << dist_mat[i][j] << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\nFlow matrix:\n";

    for (int i = 0; i < n_facs; ++i)
    {
        for (int j = 0; j < n_facs; ++j)
        {
            std::cout << flow_mat[i][j] << " ";
        }
        std::cout << "\n";
    }
}

#endif