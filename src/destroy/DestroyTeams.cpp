/*
 * DestroyTeams.cpp
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#include "DestroyTeams.h"
#include <cstdlib>

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
    //swap teams: remove all entries in row t1, t2 (n >= 2) expect the ones where they play against each other and in all other cells remove the entries containing t1 or t2
    int t1 = rand() % m_teams + 1;
    int t2 = rand() % m_teams + 1;
    while (t2 == t1)
        t2 = rand() % m_teams + 1;

    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int& entry = destroyed[t][r];
            int absEntry = std::abs(entry);
            if (t + 1 == t1 || t + 1 == t2)
            {
                if (absEntry != t2 && absEntry != t1)
                {
                    entry = 0;
                }
            } else if (absEntry == t2 || absEntry == t1)
            {
                entry = 0;
            }
        }
    }
    return destroyed;
}
