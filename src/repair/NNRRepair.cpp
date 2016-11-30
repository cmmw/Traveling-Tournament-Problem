/*
 * NNRRepair.cpp
 *
 *  Created on: 22.11.2016
 *      Author: Christian
 */

#include "NNRRepair.h"

#include <cmath>
#include <algorithm>
#include <iostream>

NNRRepair::NNRRepair(const mat2i& distance) :
        IBacktrack(distance)
{
}

NNRRepair::~NNRRepair()
{
}

bool NNRRepair::getNextVariable(int& team, int& round, const mat2i& solution)
{
    //return the variable which will increase the costs by a minimum (from right to left)
    bool found = false;
    int costs = std::numeric_limits<int>::max();
    for (int r = 0; r < m_rounds; r++)
    {
        for (int t = 0; t < m_teams; t++)
        {
            if (solution[t][r] == 0 && (r == m_rounds - 1 || solution[t][r + 1] != 0))
            {
                int to;
                if (r == m_rounds - 1)
                    to = t + 1;
                else
                    to = solution[t][r + 1];

                for (auto d : m_domain[t][r])
                {
                    int c = Common::getDistance(m_distance, t, d, to);
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

std::vector<int> NNRRepair::valueOrderHeuristic(const mat2i& solution, int team, int round)
{
    std::vector<int> domain = m_domain[team][round];
    std::sort(domain.begin(), domain.end(), [&] (int v1, int v2)
    {
        if (round == 0)
        {
            int dist1 = Common::getDistance(m_distance, team, team + 1, v1) + Common::getDistance(m_distance, team, v1, solution[team][1]);
            int dist2 = Common::getDistance(m_distance, team, team + 1, v2) + Common::getDistance(m_distance, team, v2, solution[team][1]);
            return dist1 < dist2;
        } else if (round == m_rounds - 1)
        {
            int dist1 = Common::getDistance(m_distance, team, v1, team + 1);
            int dist2 = Common::getDistance(m_distance, team, v2, team + 1);
            return dist1 < dist2;
        } else
        {
            int dist1 = Common::getDistance(m_distance, team, v1, solution[team][round+1]);
            int dist2 = Common::getDistance(m_distance, team, v2, solution[team][round+1]);
            return dist1 < dist2;
        }
    });
    return domain;
}
