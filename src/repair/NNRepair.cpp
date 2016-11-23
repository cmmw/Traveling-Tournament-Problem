/*
 * NNRepair.cpp
 *
 *  Created on: 22.11.2016
 *      Author: Christian
 */

#include "NNRepair.h"
#include <cmath>
#include <algorithm>
#include <iostream>

NNRepair::NNRepair(const mat2i& distance) :
        IRepair(distance)
{
}

NNRepair::~NNRepair()
{
}

bool NNRepair::getNextVariable(int& team, int& round, const mat2i& solution)
{
    //This heuristic assumes that all games before the current round are set for the team
    bool found = false;
    unsigned int rv = std::numeric_limits<unsigned int>::max();
    for (int r = 0; r < m_rounds && !found; r++)
    {
        for (int t = 0; t < m_teams; t++)
        {
            if (solution[t][r] == 0 && m_domain[t][r].size() < rv)
            {
                rv = m_domain[t][r].size();
                team = t;
                round = r;
                found = true;
            }
        }
    }
    return found;
}

std::vector<int> NNRepair::valueOrderHeuristic(const mat2i& solution, int team, int round)
{
    std::vector<int> domain = m_domain[team][round];
    std::sort(domain.begin(), domain.end(), [&] (int v1, int v2)
    {
        if (round == 0)
        {
            int dist1 = getDistance(team, team + 1, v1);
            int dist2 = getDistance(team, team + 1, v2);
            return dist1 < dist2;
        } else if (round == m_rounds - 1)
        {
            int dist1 = getDistance(team, solution[team][round-1], v1) + getDistance(team, v1, team + 1);
            int dist2 = getDistance(team, solution[team][round-1], v2) + getDistance(team, v2, team + 1);
            return dist1 < dist2;
        } else
        {
            int dist1 = getDistance(team, solution[team][round-1], v1);
            int dist2 = getDistance(team, solution[team][round-1], v2);
            return dist1 < dist2;
        }
    });
    return domain;
}

//team with index teamIdx plays against t1 and then t2, calculate travel distance
int NNRepair::getDistance(int teamIdx, int t1, int t2)
{
    //both play at home
    if (t1 > 0 && t2 > 0)
        return 0;

    //both play away
    if (t1 < 0 && t2 < 0)
        return m_distance[std::abs(t1) - 1][std::abs(t2) - 1];

    //t1 home, t2 away
    if (t1 > 0 && t2 < 0)
        return m_distance[teamIdx][std::abs(t2) - 1];

    //t1 away, t2 home
    return m_distance[std::abs(t1) - 1][teamIdx];
}
