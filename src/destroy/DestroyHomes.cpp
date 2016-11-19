/*
 * DestroyHomes.cpp
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#include "DestroyHomes.h"
#include <cstdlib>
#include <numeric>
#include <algorithm>

DestroyHomes::DestroyHomes(const mat2i& distance) :
        IDestroy(distance)
{
}

DestroyHomes::~DestroyHomes()
{
}

mat2i DestroyHomes::destroy(const mat2i& solution)
{
    mat2i destroyed = solution;
    std::vector<int> teams(m_teams);
    std::iota(teams.begin(), teams.end(), 1);
    std::random_shuffle(teams.begin(), teams.end());
    int size = 5;

    //swap homes: for >= 1 teams remove the entries containing t1, t2, ... in the whole schedule
    teams.resize(size);
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int absEntry = std::abs(destroyed[t][r]);
            if (std::find(teams.begin(), teams.end(), absEntry) != teams.end())
                destroyed[t][r] = 0;
        }
    }
    return destroyed;
}
