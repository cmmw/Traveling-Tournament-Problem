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

CPSolver::CPSolver(const std::vector<std::vector<int>>& distances) :
        Algorithm(distances), m_bestCosts(std::numeric_limits<int>::max()), m_solutionFound(false), m_searchOptimum(false)
{
}

CPSolver::~CPSolver()
{
}

bool CPSolver::solve(std::vector<std::vector<int>>& solution, bool optimal)
{
    init(solution);
    m_bestCosts = std::numeric_limits<int>::max();
    m_solutionFound = false;
    m_searchOptimum = optimal;
    std::vector<std::vector<int>> solutionCpy = solution;
    backTrack(1, 0, solutionCpy, m_domain, solution);
    return m_solutionFound;
}

bool CPSolver::backTrack(int team, int round, std::vector<std::vector<int>>& solution, std::vector<std::vector<std::vector<int>>>& domain, std::vector<std::vector<int>>& solutionOut)
{
    if (team > m_teams)
    {
        return backTrack(1, round+1, solution, domain, solutionOut);
    }
    if(round >= m_rounds)
    {
        if(m_searchOptimum)
        {
            int costs = eval(solution, m_distances);
            if(costs < m_bestCosts)
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

    if(solution[team - 1][round] != 0)
    {
        return backTrack(team + 1, round, solution, domain, solutionOut);
    }

    std::vector<int>& values = domain[team - 1][round];
    for(auto d : values)
    {
        auto domainCopy = domain;
        solution[team - 1][round] = d;
        if(Algorithm::forwardCheck(team, round, solution, domainCopy))
        {
            if(backTrack(team + 1, round, solution, domainCopy, solutionOut))
            {
                return true;
            }
        }
    }
    solution[team - 1][round] = 0;
    return false;
}

