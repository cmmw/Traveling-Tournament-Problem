/*
 * DestroyRounds.cpp
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#include "DestroyRounds.h"
#include <cstdlib>

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

    //swap rounds: remove all entries in >= 2 rounds
    int r1 = rand() % m_rounds;
    int r2 = rand() % m_rounds;
    int r3 = rand() % m_rounds;
    int r4 = rand() % m_rounds;
    while (r2 == r1)
        r2 = rand() % m_rounds;

    while (r3 == r1 || r3 == r2)
        r3 = rand() % m_rounds;

    while (r4 == r1 || r4 == r2 || r4 == r3)
        r4 = rand() % m_rounds;

    for (int t = 0; t < m_teams; t++)
    {
        destroyed[t][r1] = 0;
        destroyed[t][r2] = 0;
        destroyed[t][r3] = 0;
        destroyed[t][r4] = 0;
    }

    return destroyed;
}
