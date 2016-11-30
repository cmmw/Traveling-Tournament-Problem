/*
 * IBacktrack.cpp
 *
 *  Created on: 30.11.2016
 *      Author: Christian
 */

#include "IBacktrack.h"
#include <cmath>

IBacktrack::IBacktrack(const mat2i& distance) :
        IRepair(distance)
{
    // TODO Auto-generated constructor stub

}

IBacktrack::~IBacktrack()
{
    // TODO Auto-generated destructor stub
}

mat2i IBacktrack::solveImpl(const mat2i& solution)
{
    mat2i sol = solution;
    init(sol);

    int vars = Common::countUnsetVariables(sol);
    int kMax = 4;
    int k = 0;
    while (k <= kMax && !backTrack(sol, k++, vars))
        ;

    return m_bestSolution;
}

//limited discrepancy search
bool IBacktrack::backTrack(mat2i& solution, int k, int variablesLeft)
{
    //we have to follow the non-heuristic path 'k' times, but only 'nodesLeft' choices left -> prune this path
    if (variablesLeft < k)
        return false;

    int team, round;
    if (!getNextVariable(team, round, solution))
    {
        //k has to be 0 at this point, otherwise we did not apply the limited discrepancy search correctly
//        assert(k == 0);

        int value = Common::eval(solution, m_distance);
        if (value < m_upperBound)
        {
            m_bestSolution = solution;
            return true;
        }
        //Kepp track of the best solution found so far, even if it is not better than the upper bound
        if (m_bestSolution.empty() || value < m_bestSolutionValue)
        {
            m_bestSolution = solution;
            m_bestSolutionValue = value;
        }
        return false;
    }

    std::vector<int> domain = valueOrderHeuristic(solution, team, round);

    unsigned int i = 0;
    //Don't follow the heuristic (number of choices left equals the number of choices we have to not follow the heuristic)
    if (variablesLeft == k)
        i = 1;
    for (; i < domain.size(); i++)
    {
        int d = domain[i];
        //number of choices to not follow the heuristic is 0, only go 'left' (i = 0)
        if (i != 0 && k == 0)
            break;

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

        if (forwardCheck(team, round, solution, domainBackup) && (!setOpponent || forwardCheck(std::abs(d) - 1, round, solution, domainBackup)))
        {
            if (backTrack(solution, (i == 0) ? k : k - 1, (setOpponent) ? variablesLeft - 2 : variablesLeft - 1))
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
