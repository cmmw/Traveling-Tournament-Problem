/*
 * CPSolver.cpp
 *
 *  Created on: 05.10.2016
 *      Author: Christian
 */

#include "CPSolver.h"
#include "Algorithm.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <map>

CPSolver::CPSolver(const mat2i& distances) :
        Algorithm(distances), m_bestCosts(std::numeric_limits<int>::max()), m_solutionFound(false), m_searchOptimum(false)
{
}

CPSolver::~CPSolver()
{
}

bool CPSolver::solve(mat2i& solution, bool optimal)
{
    init(solution);
    m_bestCosts = std::numeric_limits<int>::max();
    m_solutionFound = false;
    m_searchOptimum = optimal;
    mat2i solutionCpy = solution;
    backTrack(0, 0, solutionCpy, solution);
    return m_solutionFound;
}

bool CPSolver::backTrack(int team, int round, mat2i& solution, mat2i& solutionOut)
{
    if (team >= m_teams)
    {
        return backTrack(0, round + 1, solution, solutionOut);
    }

    if (!getMrv(team, round, solution, m_domain))
    //    if (round >= m_rounds)
    {
        if (m_searchOptimum)
        {
            int costs = eval(solution, m_distances);
            if (costs < m_bestCosts)
            {
                m_bestCosts = costs;
                solutionOut = solution;
                m_solutionFound = true;
            }
            return false;
        }
        m_solutionFound = true;
        solutionOut = solution;
        return true;
    }

    if (solution[team][round] != 0)
    {
        return backTrack(team + 1, round, solution, solutionOut);
    }

    std::vector<int>& values = m_domain[team][round];
//    std::random_shuffle(values.begin(), values.end());
    std::map<int, int> ruledOut;
    for (unsigned int i = 0; i < values.size(); i++)
    {
        ruledOut[values[i]] = ruledOutValues(team, round, values[i], solution, m_domain);
    }
    std::sort(values.begin(), values.end(), [&](int val1, int val2)
    {   return ruledOut[val1] < ruledOut[val2];});
    for (auto d : values)
    {
        std::vector<DomainEntry> domainBackups;
        solution[team][round] = d;
        if (d < 0)
        {
            solution[-d - 1][round] = (team + 1);
        } else
        {
            solution[d - 1][round] = -(team + 1);
        }
        if (Algorithm::forwardCheck(team, round, solution, m_domain, domainBackups) && Algorithm::forwardCheck(std::abs(d) - 1, round, solution, m_domain, domainBackups))
        {
            if (backTrack(team + 1, round, solution, solutionOut))
            {
                return true;
            }
        }
        solution[std::abs(d) - 1][round] = 0;
        for (auto& b : domainBackups)
        {
            m_domain[b.m_team][b.m_round] = b.m_backup;
        }
    }
    solution[team][round] = 0;
    return false;
}

