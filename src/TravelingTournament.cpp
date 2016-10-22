//============================================================================
// Name        : TravelingTournament.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "Algorithm.h"
#include "CPSolver.h"
#include "GraphColorHeu.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <algorithm>
#include "ConstHillClimber.h"

using namespace std;

template<class T>
void test(const mat2i& distances);
void destroy(mat2i& solution, const mat2i& distances, int size);
int nodeCosts(int team, int round, const mat2i& solution, const mat2i& distances);
int _nodeCosts(int team, int round, const mat2i& solution, const mat2i& distances);

int main()
{
    srand(time(NULL));
    fstream fs("instances/nfl24.txt");
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

    test<GraphColorHeu>(distances);
    test<CPSolver>(distances);
    test<ConstHillClimber>(distances);
    return 0;

    CPSolver solver(distances);
    mat2i solution;
    int bestCost = numeric_limits<int>::max();
    mat2i bestSolution;
    solver.solve(solution);
    bestSolution = solution;
    bestCost = Algorithm::eval(bestSolution, distances);
    std::cout << "Initial solution: " << bestCost << std::endl;
    int size = 5;
    for (int i = 0; i < 2000; i++)
    {
        //Destroy solution
        destroy(solution, distances, size);
        if (solver.solve(solution, true))
        {
            int costs = Algorithm::eval(solution, distances);
            if (costs < bestCost)
            {
                bestSolution = solution;
                bestCost = costs;
                i = 0;
                size = 10;
                if (size < 0)
                    size = 5;
            } else
            {
                if (size != -1 && size < 130)
                {
                    size++;
                } else
                {
                    size = -1;
                }
            }
        } else
        {
            cerr << "No solution found" << endl;
        }
        std::cout << "\ri: " << i + 1 << ",\tsize: " << size << ",\tbestCosts: " << bestCost;
        std::cout.flush();
    }
    std::cout << std::endl;
    std::cout << Algorithm::eval(bestSolution, distances) << std::endl;
    Algorithm::printMatrix(bestSolution);
    return 0;
}

class Entry
{
public:
    Entry(int team, int round, int dist) :
            m_team(team), m_round(round), m_dist(dist)
    {
    }
    Entry() :
            m_team(-1), m_round(-1), m_dist(-1)
    {
    }
    int m_team;
    int m_round;
    int m_dist;
};

void destroy(mat2i& solution, const mat2i& distances, int size)
{
    int teams = solution.size();
    int rounds = teams * 2 - 2;

    if (size > 0)
    {
        std::vector<Entry> entries(size);
        for (int t = 0; t < teams; t++)
        {
            for (int r = 0; r < rounds; r++)
            {
                bool skip = false;
                for (const auto e : entries)
                {
                    if (e.m_round == r)
                    {
                        if (e.m_team != -1 && e.m_team == std::abs(solution[t][r]) - 1)
                            skip = true;
                    }
                }
                if (skip)
                    continue;
                //TODO prevent duplicate deletion of nodes
                int dist = _nodeCosts(t, r, solution, distances);
                auto ub = std::upper_bound(entries.begin(), entries.end(), dist, [](int value, const Entry& e)
                {   return value < e.m_dist;});
                if (ub != entries.begin())
                {
                    std::move(entries.begin() + 1, ub, entries.begin());
                    *(ub - 1) = Entry(t, r, dist);
                }
            }
        }

        for (const auto e : entries)
        {
            if (solution[e.m_team][e.m_round] != 0)
            {
                solution[std::abs(solution[e.m_team][e.m_round]) - 1][e.m_round] = 0;
                solution[e.m_team][e.m_round] = 0;
            }
        }
    } else
    {
        //Random
        for (int j = 0; j < 120; j++)
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
    }
}

int nodeCosts(int team, int round, const mat2i& solution, const mat2i& distances)
{
    return _nodeCosts(team, round, solution, distances) + _nodeCosts(std::abs(solution[team][round]) - 1, round, solution, distances);
}

int _nodeCosts(int team, int round, const mat2i& solution, const mat2i& distances)
{
    int costs = 0;
    int from = 0;
    int to = 0;
    int opponent = solution[team][round];

    if (round > 0)
        from = solution[team][round - 1];

    if (round < ((int) solution[0].size()) - 1)
        to = solution[team][round + 1];

    if (from < 0 && opponent < 0)
        costs += distances[std::abs(from) - 1][std::abs(opponent) - 1];
    else if (from >= 0 && opponent < 0)
        costs += distances[team][std::abs(opponent) - 1];
    else if (from < 0 && opponent > 0)
        costs += distances[std::abs(from) - 1][team];

    if (to < 0 && opponent < 0)
        costs += distances[std::abs(opponent) - 1][std::abs(to) - 1];
    else if (to >= 0 && opponent < 0)
        costs += distances[std::abs(opponent) - 1][team];
    else if (to < 0 && opponent > 0)
        costs += distances[team][std::abs(to) - 1];

    return costs;
}

template<class T>
void test(const mat2i& distances)
{
    auto start = std::chrono::high_resolution_clock::now();
    T solver(distances);
    mat2i solution;
    bool found = solver.solve(solution, false);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (found)
    {
        Algorithm::printMatrix(solution);
    } else
    {
        std::cout << "no solution found" << std::endl;
    }
    std::cout << "Total distance: " << Algorithm::eval(solution, distances) << std::endl;
    std::cout << "Took: " << duration << "ms" << std::endl;
}
