//============================================================================
// Name        : TravelingTournament.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "CPSolver.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

int main()
{
    fstream fs("instances/data8.txt");
    if (fs.fail())
    {
        cerr << "failed to open instance" << endl;
        return 1;
    }

    vector<vector<int>> distances;
    string line;
    while (getline(fs, line))
    {
        vector<int> row;
        stringstream ss(line);
        while (true)
        {
            int t;
            ss >> t;
            if (ss.fail())
                break;
            row.push_back(t);
        }
        if (!row.empty())
            distances.push_back(row);
    }
    std::cout << "Distances: " << std::endl;
    CPSolver::printMatrix(distances);
    std::cout << "-----------" << std::endl;

    CPSolver solver(distances);
    if (!solver.solve())
    {
        cerr << "No solution found" << endl;
    } else
    {
        CPSolver::printMatrix(solver.getSolution());
    }
    return 0;
}
