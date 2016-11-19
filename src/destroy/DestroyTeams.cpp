/*
 * DestroyTeams.cpp
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#include "DestroyTeams.h"
#include <cstdlib>
#include <numeric>
#include <algorithm>

DestroyTeams::DestroyTeams(const mat2i& distance) :
        IDestroy(distance)
{
}

DestroyTeams::~DestroyTeams()
{
}

mat2i DestroyTeams::destroy(const mat2i& solution)
{
    mat2i destroyed = solution;
    std::vector<int> teams(m_teams);
    std::iota(teams.begin(), teams.end(), 1);
    std::random_shuffle(teams.begin(), teams.end());
    int size = 2;

    //swap teams: remove all entries in row t1, t2 (n >= 2) expect the ones where they play against each other and in all other cells remove the entries containing t1 or t2
    teams.resize(size);
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int& entry = destroyed[t][r];
            int absEntry = std::abs(entry);

            if (std::find(teams.begin(), teams.end(), t + 1) != teams.end())
            {
                if (std::find(teams.begin(), teams.end(), absEntry) == teams.end())
                {
                    entry = 0;
                }
            } else if (std::find(teams.begin(), teams.end(), absEntry) != teams.end())
            {
                entry = 0;
            }
        }
    }
    return destroyed;
}
