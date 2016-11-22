/*
 * DFSStar.cpp
 *
 *  Created on: 21.11.2016
 *      Author: Christian
 */

#include "DFSStar.h"
#include <cmath>
#include <iostream>

DFSStar::DFSStar(const mat2i& distance) :
        IRepair(distance)
{
}

DFSStar::~DFSStar()
{
}

void DFSStar::solve(const mat2i& solution)
{
    mat2i sol = solution;
    init(sol);
    backTrack(sol);
}

bool DFSStar::backTrack(mat2i& solution)
{
    int lowerBound = 10;
    int upperBound = 20;

    int team, round;
    if (!chooseVariable(team, round, solution))
    {
        int value = Common::eval(solution, m_distance);
        if (value < m_bestValue)
        {
            m_bestSolution = solution;
            m_bestValue = value;
        }
        return false;
    }

    std::vector<int> domain = m_domain[team][round];
    for (auto d : domain)
    {
        std::vector<DomainBackupEntry> domainBackup;
        solution[team][round] = d;
        bool setOpponent = solution[std::abs(d) - 1][round] == 0;
        if (setOpponent)
        {
            if (d < 0)
                solution[-d - 1][round] = (team + 1);
            else
                solution[d - 1][round] = -(team + 1);
        }

        if (lowerBound < upperBound && forwardCheck(team, round, solution, domainBackup) && (!setOpponent || forwardCheck(std::abs(d) - 1, round, solution, domainBackup)))
        {
            if (backTrack(solution))
                return true;
        }

        if (setOpponent)
            solution[std::abs(d) - 1][round] = 0;
        for (auto& b : domainBackup)
        {
            m_domain[b.m_team][b.m_round] = b.m_backup;
        }
    }
    solution[team][round] = 0;
    return false;
}

bool DFSStar::chooseVariable(int& team, int& round, const mat2i& solution)
{
    for (int r = 0; r < m_rounds; r++)
    {
        for (int t = 0; t < m_teams; t++)
        {
            if (solution[t][r] == 0)
            {
                team = t;
                round = r;
                return true;
            }
        }
    }
    return false;
}
