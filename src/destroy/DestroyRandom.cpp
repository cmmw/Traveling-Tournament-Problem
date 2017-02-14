/*
 * DestroyRandom.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#include "DestroyRandom.h"
#include <algorithm>
#include <iostream>

DestroyRandom::DestroyRandom(const mat2i& distance, IPSolver& solver) :
        IDestroy(distance, solver)
{
}

DestroyRandom::~DestroyRandom()
{
}

mat2i DestroyRandom::destroy(const mat2i& solution, int size)
{
    mat2i destroyed = solution;

    //random
    std::vector<std::pair<int, int>> vars;
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            vars.push_back(std::make_pair(t, r));
        }
    }
    std::random_shuffle(vars.begin(), vars.end());
    float s = size * m_teams * m_rounds / 100.0f;
    size = std::round(s);
//    std::cout << "destroy " << size << " entries" << std::endl;
    for (int i = 0; i < size; i++)
    {
        int t = vars[i].first;
        int r = vars[i].second;
        if (destroyed[t][r] != 0)
        {
            destroyed[std::abs(destroyed[t][r]) - 1][r] = 0;
            destroyed[t][r] = 0;
        }
    }

    m_solver.populatePartialSolution(destroyed);
    return destroyed;
}
