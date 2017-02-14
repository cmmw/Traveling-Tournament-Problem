/*
 * DestroyTeams.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#include "DestroyTeams.h"
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <random>

DestroyTeams::DestroyTeams(const mat2i& distance, IPSolver& solver) :
        IDestroy(distance, solver)
{
}

DestroyTeams::~DestroyTeams()
{
}

mat2i DestroyTeams::destroy(const mat2i& solution, int size)
{
    mat2i destroyed = solution;
    std::vector<int> teams(m_teams);
    std::vector<int> homeCosts = homeDistance(solution);
    std::iota(teams.begin(), teams.end(), 1);

    std::default_random_engine engine(rand());
    std::discrete_distribution<> dist;

    //One team occurs exactly two times in (m_teams - 1) rows
    float s = m_teams * m_rounds * size / 100.0f;
    s = s / (2.0f * (m_teams - 1));
    size = std::round(s);
    if (size < 2)
        size = 2;

//    std::cout << "destroy " << size << " teams" << std::endl;
    std::vector<int> selected;
    for (int i = 0; i < size; i++)
    {
        std::discrete_distribution<>::param_type param(homeCosts.begin(), homeCosts.end());
        dist.param(param);
        int tIdx = dist(engine);
        selected.push_back(teams[tIdx]);
        homeCosts.erase(homeCosts.begin() + tIdx);
        teams.erase(teams.begin() + tIdx);
    }

    //swap homes: for >= 1 teams remove the entries containing t1, t2, ... in the whole schedule
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int absEntry = std::abs(destroyed[t][r]);
            if (std::find(selected.begin(), selected.end(), absEntry) != selected.end())
                destroyed[t][r] = 0;
        }
    }
    m_solver.populatePartialSolution(destroyed);
    return destroyed;
}

std::vector<int> DestroyTeams::homeDistance(const mat2i& solution)
{
    std::vector<int> homeDistance(m_teams);
    for (int t = 0; t < m_teams; t++)
    {
        int opponent = solution[t][0];
        if (opponent < 0)
            homeDistance[std::abs(opponent) - 1] += m_distance[t][std::abs(opponent) - 1];

        for (int r = 1; r < m_rounds - 1; r++)
        {
            int start = solution[t][r - 1];
            opponent = solution[t][r];
            if (start < 0 && opponent > 0)
            {
                //Away -> Home
                homeDistance[std::abs(start) - 1] += m_distance[std::abs(start) - 1][t];
                homeDistance[std::abs(opponent) - 1] += m_distance[std::abs(start) - 1][t];
            } else if (start > 0 && opponent < 0)
            {
                //Home -> Away
                homeDistance[std::abs(start) - 1] += m_distance[t][std::abs(opponent) - 1];
                homeDistance[std::abs(opponent) - 1] += m_distance[t][std::abs(opponent) - 1];
            } else if (start < 0 && opponent < 0)
            {
                //Away -> Away
                homeDistance[std::abs(start) - 1] += m_distance[std::abs(start) - 1][std::abs(opponent) - 1];
                homeDistance[std::abs(opponent) - 1] += m_distance[std::abs(start) - 1][std::abs(opponent) - 1];
            }
        }

        opponent = solution[t][m_rounds - 1];
        if (opponent < 0)
            homeDistance[std::abs(opponent) - 1] += m_distance[std::abs(opponent) - 1][t];
    }
    return homeDistance;
}
