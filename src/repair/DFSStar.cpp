/*
 * DFSStar.cpp
 *
 *  Created on: 21.11.2016
 *      Author: Christian
 */

#include "DFSStar.h"
#include <cmath>
#include <limits>

DFSStar::DFSStar(const mat2i& distance) :
        IRepair(distance), m_nodes(0), m_bestValue(std::numeric_limits<int>::max())
{
}

DFSStar::~DFSStar()
{
}

mat2i DFSStar::solve(const mat2i& solution, bool optimal)
{
    mat2i sol = solution;
    init(sol);
    m_nodes = 0;
    m_bestSolution.clear();
    m_bestValue = std::numeric_limits<int>::max();
    backTrack(sol);
    return m_bestSolution;
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

    for (auto d : m_domain[team][round])
    {
        m_nodes++;
        std::vector<DomainBackupEntry> domainBackup;
        solution[team][round] = d;

        if (d < 0)
            solution[-d - 1][round] = (team + 1);
        else
            solution[d - 1][round] = -(team + 1);

        if (lowerBound < upperBound && forwardCheck(team, round, solution, domainBackup) && forwardCheck(std::abs(d) - 1, round, solution, domainBackup))
        {
            if (backTrack(solution))
                return true;
        }
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
