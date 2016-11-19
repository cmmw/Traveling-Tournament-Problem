/*
 * DestroyHomes.cpp
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#include "DestroyHomes.h"
#include <cstdlib>

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

    //swap homes: for >= 1 games remove the entries containing t1 or t2 in the whole schedule
    int t1 = rand() % m_teams + 1;
    int t2 = rand() % m_teams + 1;
    int t3 = rand() % m_teams + 1;
    int t4 = rand() % m_teams + 1;
    int t5 = rand() % m_teams + 1;
    while (t2 == t1)
        t2 = rand() % m_teams + 1;

    while (t3 == t1 || t3 == t2)
        t3 = rand() % m_teams + 1;

    while (t4 == t1 || t4 == t2 || t4 == t3)
        t4 = rand() % m_teams + 1;

    while (t5 == t1 || t5 == t2 || t5 == t3 || t5 == t4)
        t5 = rand() % m_teams + 1;

    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int absEntry = std::abs(destroyed[t][r]);
            if (absEntry == t1 || absEntry == t2 || absEntry == t3 || absEntry == t4 || absEntry == t5)
                destroyed[t][r] = 0;
        }
    }

    return destroyed;
}
