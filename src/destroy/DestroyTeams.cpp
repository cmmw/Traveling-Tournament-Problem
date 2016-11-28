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
#include <iostream>

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
    std::vector<int> teamCosts = teamDistance(solution);
    std::iota(teams.begin(), teams.end(), 1);

    std::default_random_engine engine(rand());
    std::discrete_distribution<> dist;

    std::vector<int> selected;
    int size = 2;
    for (int i = 0; i < size; i++)
    {
        std::discrete_distribution<>::param_type param(teamCosts.begin(), teamCosts.end());
        dist.param(param);
        int tIdx = dist(engine);
        selected.push_back(teams[tIdx]);
        teamCosts.erase(teamCosts.begin() + tIdx);
        teams.erase(teams.begin() + tIdx);
    }

    //swap teams: remove all entries in row t1, t2 (n >= 2) expect the ones where they play against each other and in all other cells remove the entries containing t1 or t2
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int& entry = destroyed[t][r];
            int absEntry = std::abs(entry);

            if (std::find(selected.begin(), selected.end(), t + 1) != selected.end())
            {
                if (std::find(selected.begin(), selected.end(), absEntry) == selected.end())
                {
                    entry = 0;
                }
            } else if (std::find(selected.begin(), selected.end(), absEntry) != selected.end())
            {
                entry = 0;
            }
        }
    }
    return destroyed;
}

std::vector<int> DestroyTeams::teamDistance(const mat2i& solution)
{
    std::vector<int> teamDistance(m_teams);
    for (int t = 0; t < m_teams; t++)
    {
        teamDistance[t] += Common::getDistance(m_distance, t, t + 1, solution[t][0]);
        for (int r = 1; r < m_rounds; r++)
        {
            teamDistance[t] += Common::getDistance(m_distance, t, solution[t][r - 1], solution[t][r]);
        }
        teamDistance[t] += Common::getDistance(m_distance, t, solution[t][m_rounds - 1], t + 1);
    }
    return teamDistance;
}
