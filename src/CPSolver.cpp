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
    backTrack(1, 0, solutionCpy, m_domain, solution);
    return m_solutionFound;
}

bool CPSolver::backTrack(int team, int round, mat2i& solution, mat3i& domain, mat2i& solutionOut)
{
    if (team > m_teams)
    {
        return backTrack(1, round + 1, solution, domain, solutionOut);
    }
    if (round >= m_rounds)
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

    if (solution[team - 1][round] != 0)
    {
        return backTrack(team + 1, round, solution, domain, solutionOut);
    }

    std::vector<int>& values = domain[team - 1][round];
    for (auto d : values)
    {
        std::vector<DomainEntry> domainBackups;
        solution[team - 1][round] = d;
        if (d < 0)
        {
            solution[-d - 1][round] = team;
        } else
        {
            solution[d - 1][round] = -team;
        }
        if (Algorithm::forwardCheck(team, round, solution, domain, domainBackups) && Algorithm::forwardCheck(std::abs(d), round, solution, domain, domainBackups))
        {
            if (backTrack(team + 1, round, solution, domain, solutionOut))
            {
                return true;
            }
        }
        solution[std::abs(d) - 1][round] = 0;
        for (auto& b : domainBackups)
        {
            domain[b.m_team][b.m_round] = b.m_backup;
        }
    }
    solution[team - 1][round] = 0;
    return false;
}

