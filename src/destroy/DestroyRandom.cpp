/*
 * DestroyRandom.cpp
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#include "DestroyRandom.h"
#include <algorithm>

DestroyRandom::DestroyRandom(const mat2i& distance) :
        IDestroy(distance)
{
}

DestroyRandom::~DestroyRandom()
{
}

mat2i DestroyRandom::destroy(const mat2i& solution)
{
    mat2i destroyed = solution;

    //random
    int size = m_teams * 5;
    std::vector<std::pair<int, int>> vars;
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            vars.push_back(std::make_pair(t, r));
        }
    }
    std::random_shuffle(vars.begin(), vars.end());

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

    return destroyed;
}
