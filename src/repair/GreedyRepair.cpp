/*
 * GreedyRepair.cpp
 *
 *  Created on: 22.11.2016
 *      Author: Christian
 */

#include "GreedyRepair.h"

#include <cmath>
#include <algorithm>
#include <iostream>

GreedyRepair::GreedyRepair(const mat2i& distance) :
        IBacktrack(distance)
{
}

GreedyRepair::~GreedyRepair()
{
}

bool GreedyRepair::getNextVariable(int& team, int& round, const mat2i& solution)
{
    //return MRV
    bool found = false;
    unsigned int rv = std::numeric_limits<decltype(rv)>::max();
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            if (solution[t][r] == 0)
            {
                if (rv > m_domain[t][r].size())
                {
                    team = t;
                    round = r;
                    rv = m_domain[t][r].size();
                    found = true;
                }
            }
        }
    }
    return found;
}

std::vector<int> GreedyRepair::valueOrderHeuristic(const mat2i& solution, int team, int round)
{
    //Order values by increasing distance
    std::vector<int> domain = m_domain[team][round];
    std::sort(domain.begin(), domain.end(), [&] (int v1, int v2)
    {
        return Common::deltaDistance(team, round, v1, solution, m_distance) < Common::deltaDistance(team, round, v2, solution, m_distance);
    });
    return domain;
}
