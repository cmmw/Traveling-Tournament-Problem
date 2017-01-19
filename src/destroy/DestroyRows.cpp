/*
 * DestroyRow.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#include "DestroyRows.h"
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <random>

DestroyRows::DestroyRows(const mat2i& distance) :
        IDestroy(distance)
{
}

DestroyRows::~DestroyRows()
{
}

mat2i DestroyRows::destroy(const mat2i& solution, int size)
{
    mat2i destroyed = solution;
    std::vector<int> teams(m_teams);
    std::vector<int> teamCosts = teamDistance(solution);
    std::iota(teams.begin(), teams.end(), 1);

    std::default_random_engine engine(rand());
    std::discrete_distribution<> dist;

    //calculate the number of rows and teams to delete so that 'size'% of the entries are delete
    //there are two solutions for that equation, choose one randomly
    float s = m_teams * m_rounds * size / 100.0;       //number of entries to delete
    float y = m_teams + m_rounds / 2.0f + 1;
    float x1 = ((y / 2.0f) + std::sqrt(std::pow(y, 2.0f) / 4.0f - s)) / 2.0f;
    float x2 = ((y / 2.0f) - std::sqrt(std::pow(y, 2.0f) / 4.0f - s)) / 2.0f;

    if (std::rand() % 2 == 0)
        s = x1;
    else
        s = x2;

    size = std::round(s);
    if (size < 2)
        size = 2;

    std::cout << "destroy " << size << " rows+teams" << std::endl;
    std::vector<int> selected;
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

std::vector<int> DestroyRows::teamDistance(const mat2i& solution)
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
