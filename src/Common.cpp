/*
 * Common.cpp
 *
 *  Created on: 23.10.2016
 *      Author: Christian
 */

#include "Common.h"
#include <iostream>

void Common::initSolution(int n, mat2i& solution)
{
    solution.resize(n);
    for (int i = 0; i < n; i++)
    {
        solution[i].resize(2 * n - 2);
    }
}

void Common::printMatrix(const mat2i& matrix)
{
    for (auto r : matrix)
    {
        for (auto t : r)
        {
            std::cout << t << " ";
        }
        std::cout << std::endl;
    }
}

int Common::eval(const mat2i& solution, const mat2i& distance)
{
    if (solution.empty())
        return 0;
    int costs = 0;
    int teams = distance.size();
    int rounds = 2 * distance.size() - 2;

    for (int team = 0; team < teams; team++)
    {
        //Home -> first opponent
        int opponent = solution[team][0];
        if (opponent < 0)
            costs += distance[team][std::abs(opponent) - 1];

        for (int round = 1; round < rounds; round++)
        {
            int start = solution[team][round - 1];
            opponent = solution[team][round];
            if (start < 0 && opponent > 0)
            {
                //Away -> Home
                costs += distance[std::abs(start) - 1][team];
            } else if (start > 0 && opponent < 0)
            {
                //Home -> Away
                costs += distance[team][std::abs(opponent) - 1];
            } else if (start < 0 && opponent < 0)
            {
                //Away -> Away
                costs += distance[std::abs(start) - 1][std::abs(opponent) - 1];
            }
        }

        //Last opponent -> home
        opponent = solution[team][rounds - 1];
        if (opponent < 0)
            costs += distance[std::abs(opponent) - 1][team];
    }
    return costs;
}
