/*
 * NNRepair.cpp
 *
 *  Created on: 22.11.2016
 *      Author: Christian
 */

#include "NNLRepair.h"

#include <cmath>
#include <algorithm>
#include <iostream>

NNLRepair::NNLRepair(const mat2i& distance) :
        IBacktrack(distance)
{
}

NNLRepair::~NNLRepair()
{
}

bool NNLRepair::getNextVariable(int& team, int& round, const mat2i& solution)
{
    //return the variable which will increase the costs by a minimum (from left to right)
    bool found = false;
    int costs = std::numeric_limits<decltype(costs)>::max();
    for (int r = 0; r < m_rounds; r++)
    {
        for (int t = 0; t < m_teams; t++)
        {
            if (solution[t][r] == 0 && (r == 0 || solution[t][r - 1] != 0))
            {
                int from;
                if (r == 0)
                    from = t + 1;
                else
                    from = solution[t][r - 1];

                for (auto d : m_domain[t][r])
                {
                    int c = Common::getDistance(m_distance, t, from, d);
                    if (c < costs)
                    {
                        costs = c;
                        team = t;
                        round = r;
                        found = true;
                    }
                }
            }
        }
    }
    return found;
}

std::vector<int> NNLRepair::valueOrderHeuristic(const mat2i& solution, int team, int round)
{
    std::vector<int> domain = m_domain[team][round];
    std::sort(domain.begin(), domain.end(), [&] (int v1, int v2)
    {
        if (round == 0)
        {
            int dist1 = Common::getDistance(m_distance, team, team + 1, v1);
            int dist2 = Common::getDistance(m_distance, team, team + 1, v2);
            return dist1 < dist2;
        } else if (round == m_rounds - 1)
        {
            int dist1 = Common::getDistance(m_distance, team, solution[team][round-1], v1) + Common::getDistance(m_distance, team, v1, team + 1);
            int dist2 = Common::getDistance(m_distance, team, solution[team][round-1], v2) + Common::getDistance(m_distance, team, v2, team + 1);
            return dist1 < dist2;
        } else
        {
            int dist1 = Common::getDistance(m_distance, team, solution[team][round-1], v1);
            int dist2 = Common::getDistance(m_distance, team, solution[team][round-1], v2);
            return dist1 < dist2;
        }
    });
    return domain;
}
