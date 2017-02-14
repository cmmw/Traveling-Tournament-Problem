/*
 * DestroyColumn.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#include "DestroyColumns.h"
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include <random>
#include <iostream>
DestroyColumns::DestroyColumns(const mat2i& distance, IPSolver& solver) :
        IDestroy(distance, solver)
{
}

DestroyColumns::~DestroyColumns()
{
}

mat2i DestroyColumns::destroy(const mat2i& solution, int size)
{
    mat2i destroyed = solution;
    std::vector<int> rounds(m_rounds);
    std::vector<int> roundCosts = roundDistances(solution);
    std::iota(rounds.begin(), rounds.end(), 0);

    std::default_random_engine engine(rand());
    std::discrete_distribution<> dist;

    float s = m_rounds * size / 100.0f;
    size = std::round(s);
    if (size < 2)
        size = 2;
//    std::cout << "destroy " << size << " columns" << std::endl;
    std::vector<int> selected;
    for (int i = 0; i < size; i++)
    {
        std::discrete_distribution<>::param_type param(roundCosts.begin(), roundCosts.end());
        dist.param(param);
        int rIdx = dist(engine);
        selected.push_back(rounds[rIdx]);
        roundCosts.erase(roundCosts.begin() + rIdx);
        rounds.erase(rounds.begin() + rIdx);
    }

    //swap rounds: remove all entries in >= 2 rounds
    for (int t = 0; t < m_teams; t++)
    {
        for (int r : selected)
            destroyed[t][r] = 0;
    }
    m_solver.populatePartialSolution(destroyed);
    return destroyed;
}

std::vector<int> DestroyColumns::roundDistances(const mat2i& solution)
{
    std::vector<int> roundDistance(m_rounds);
    for (int r = 0; r < m_rounds; r++)
    {
        for (int t = 0; t < m_teams; t++)
        {
            int prevOpponent;
            int opponent = solution[t][r];
            int nextOpponent;

            if (r == 0)
                prevOpponent = t + 1;
            else
                prevOpponent = solution[t][r - 1];

            if (r == m_rounds - 1)
                nextOpponent = t + 1;
            else
                nextOpponent = solution[t][r + 1];

            roundDistance[r] += Common::getDistance(m_distance, t, prevOpponent, opponent) + Common::getDistance(m_distance, t, opponent, nextOpponent);
        }
    }
    return roundDistance;
}
