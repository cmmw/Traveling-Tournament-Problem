//============================================================================
// Name        : main.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "LNS.h"
#include "repair/CPSolver.h"
#include "Factorization.h"
#include "Common.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <sstream>

mat2i readInstance(const char* instance);
template<class T>
void test(const char* instance);
template<class T>
void benchmark();
template<class T>
void testLNS(const char* instance);

const char* instances[] =
        { "instances/data8.txt", "instances/data16.txt", "instances/nfl18.txt", "instances/nfl20.txt", "instances/nfl22.txt", "instances/nfl24.txt", "instances/nfl26.txt", "instances/nfl28.txt", "instances/nfl30.txt", "instances/nfl32.txt", "instances/galaxy34.txt", "instances/galaxy36.txt", "instances/galaxy38.txt", "instances/galaxy40.txt" };

int main()
{
    srand(time(nullptr));

    testLNS<Factorization>("instances/data10.txt");
//    test<CPSolver>("instances/data6.txt");
//    test<Factorization>("instances/data8.txt");
//    benchmark<CPSolver>();
//    benchmark<Factorization>();
    return 0;
}

mat2i readInstance(const char* instance)
{
    mat2i distance;
    std::fstream fs(instance);
    if (fs.fail())
    {
        std::cerr << "failed to open instance" << std::endl;
        return distance;
    }

    std::string line;
    while (getline(fs, line))
    {
        std::vector<int> row;
        std::stringstream ss(line);
        while (true)
        {
            int t;
            ss >> t;
            if (ss.fail())
                break;
            row.push_back(t);
        }
        if (!row.empty())
            distance.push_back(row);
    }
    return distance;
}

template<class T>
void test(const char* instance)
{
    mat2i distance = readInstance(instance);

    std::cout << "Distances: " << std::endl;
    Common::printMatrix(distance);
    std::cout << "-----------" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    T solver(distance);
    mat2i solution;
    bool found = solver.solve(solution);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (found)
    {
        Common::printMatrix(solution);
    } else
    {
        std::cout << "no solution found" << std::endl;
    }
    std::cout << "Total distance: " << Common::eval(solution, distance) << std::endl;
    std::cout << "Took: " << duration << "ms" << std::endl;
}

#include <windows.h>
template<class T>
void benchmark()
{
    for (const char* instance : instances)
    {
        mat2i distance = readInstance(instance);
        if (distance.empty())
            break;
        T solver(distance);
        mat2i solution;
        auto start = std::chrono::high_resolution_clock::now();
        bool found = solver.solve(solution);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << instance << ": " << duration << "ms, ";
        if (found)
            std::cout << Common::eval(solution, distance) << std::endl;
        else
            std::cout << "no solution found" << std::endl;
    }
}

template<class T>
void testLNS(const char* instance)
{
    mat2i distance = readInstance(instance);
    mat2i initSolution;
    T solver(distance);
    solver.solve(initSolution);

    LNS lns(distance);

    auto start = std::chrono::high_resolution_clock::now();
    mat2i solution = lns.solve(initSolution);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    Common::printMatrix(solution);
    std::cout << instance << ": " << duration << "ms, ";
    std::cout << Common::eval(solution, distance) << std::endl;
}