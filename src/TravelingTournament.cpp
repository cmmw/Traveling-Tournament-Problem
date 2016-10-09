//============================================================================
// Name        : TravelingTournament.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "CPSolver.h"
#include "Algorithm.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <chrono>
using namespace std;

void test(const std::vector<std::vector<int>>& distances);

int main()
{
    srand(time(NULL));
    fstream fs("instances/nfl18.txt");
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
    Algorithm::printMatrix(distances);
    std::cout << "-----------" << std::endl;

    test(distances);
    return 0;

    CPSolver solver(distances);
    std::vector<std::vector<int>> solution;
    int bestCost = numeric_limits<int>::max();
    std::vector<std::vector<int>> bestSolution;
    solver.solve(solution);
    bestSolution = solution;
    bestCost = Algorithm::eval(bestSolution, distances);

    for (int i = 0; i < 1200; i++)
    {

        //Destroy solution
        int teams = solver.getNumTeams();
        int rounds = solver.getNumRounds();
        for (int j = 0; j < 20; j++)
        {
            int t = std::rand() % teams;
            int r = std::rand() % rounds;
            int opponent = solution[t][r];
            if (opponent != 0)
            {
                solution[std::abs(opponent) - 1][r] = 0;
                solution[t][r] = 0;
            } else
            {
                j--;
            }
        }

        if (solver.solve(solution, true))
        {
            int costs = Algorithm::eval(solution, distances);
            if (costs < bestCost)
            {
                bestSolution = solution;
                bestCost = costs;
                std::cout << bestCost << std::endl;
                i = 0;
            }
        } else
        {
            cerr << "No solution found" << endl;
        }
    }
    std::cout << Algorithm::eval(bestSolution, distances) << std::endl;
    Algorithm::printMatrix(bestSolution);
    return 0;
}

void test(const std::vector<std::vector<int>>& distances)
{
    auto start = std::chrono::high_resolution_clock::now();
    CPSolver solver(distances);
    std::vector<std::vector<int>> solution;
    solver.solve(solution, false);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    Algorithm::printMatrix(solution);
    std::cout << "Total distance: " << Algorithm::eval(solution, distances) << std::endl;
    std::cout << "Took: " << duration << "ms" << std::endl;
}
