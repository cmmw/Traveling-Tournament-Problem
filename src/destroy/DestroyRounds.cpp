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
    std::iota(rounds.begin(), rounds.end(), 0);
    std::random_shuffle(rounds.begin(), rounds.end());
    int size = 5;

    //swap rounds: remove all entries in >= 2 rounds
    for (int t = 0; t < m_teams; t++)
    {
        for (int i = 0; i < size; i++)
            destroyed[t][rounds[i]] = 0;
    }
    return destroyed;
}
