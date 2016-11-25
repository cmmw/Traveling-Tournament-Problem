/*
 * DestroyRounds.cpp
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#include "DestroyRounds.h"
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <random>

DestroyRounds::DestroyRounds(const mat2i& distance) :
        IDestroy(distance)
{
}

DestroyRounds::~DestroyRounds()
{
}

mat2i DestroyRounds::destroy(const mat2i& solution)
{
    mat2i destroyed = solution;
    std::vector<int> rounds(m_rounds);
    std::vector<int> roundCosts(m_rounds);
    for (int r = 0; r < m_rounds; r++)
    {
        roundCosts[r] = roundDistance(solution, r);
    }
    std::iota(rounds.begin(), rounds.end(), 0);
    std::cout << "rand: " << rand() << std::endl;
    std::default_random_engine engine(rand());
    std::discrete_distribution<> dist;

    std::vector<int> selected;
    int size = 5;
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
    return destroyed;
}

int DestroyRounds::roundDistance(const mat2i& solution, int r)
{
    int costs = 0;
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

        costs += Common::getDistance(m_distance, t, prevOpponent, opponent) + Common::getDistance(m_distance, t, opponent, nextOpponent);
    }
    return costs;
}
