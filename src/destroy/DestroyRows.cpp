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

DestroyRows::DestroyRows(const mat2i& distance, IPSolver& solver) :
        IDestroy(distance, solver)
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
//    float c = m_teams * m_rounds * size / 100.f;       //number of entries to delete
//    float y = (2 * m_teams + m_rounds) / 4.f;
//    int x1 = std::round(y + std::sqrt(std::pow(y, 2.0f) - (c / 2.f)));
//    int x2 = std::round(y - std::sqrt(std::pow(y, 2.0f) - (c / 2.f)));
//
//    if (x1 > m_teams)
//        x1 = x2;
//
//    if (std::rand() % 2 == 0)
//        size = x1;
//    else
//        size = x2;
//
//    if (size < 2)
//        size = 2;

    size = m_teams * size / 100.f;
//    std::cout << "destroy " << size << " rows+teams" << std::endl;
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

    //swap teams: remove all entries in row t1, t2 (n >= 2)and in all other cells remove the entries containing t1 or t2
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int& entry = destroyed[t][r];
            int absEntry = std::abs(entry);

            if (std::find(selected.begin(), selected.end(), t + 1) != selected.end())
            {
                entry = 0;
            } else if (std::find(selected.begin(), selected.end(), absEntry) != selected.end())
            {
                entry = 0;
            }
        }
    }
    m_solver.populatePartialSolution(destroyed);
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
