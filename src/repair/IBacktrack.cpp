/*
 * IBacktrack.cpp
 *
 *  Created on: 30.11.2016
 *      Author: Christian
 */

#include "IBacktrack.h"
#include <cmath>
#include <cstdlib>

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
    int distance = Common::eval(sol, m_distance);
    while (k <= kMax && !backTrack(sol, distance, k++, vars))
        ;

    return m_bestSolution;
}

//limited discrepancy search
bool IBacktrack::backTrack(mat2i& solution, int distance, int k, int variablesLeft)
{
    //we have to follow the non-heuristic path 'k' times, but only 'nodesLeft' choices left -> prune this path
    if (variablesLeft < k)
        return false;

    if (calcLowerBound(solution, distance) > m_upperBound)
        return false;

    int team, round;
    if (!getNextVariable(team, round, solution))
    {
        //k has to be 0 at this point, otherwise we did not apply the limited discrepancy search correctly
//        assert(k == 0);

        int value = Common::eval(solution, m_distance);
        if (value < m_threshold)
        {
            m_bestSolution = solution;
            return true;
        }
        //Keep track of the best solution found so far
        if (value < m_bestSolutionValue)
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
            int newDistance = distance + Common::deltaDistance(team, round, d, solution, m_distance);
            if (setOpponent)
                newDistance += Common::deltaDistance(std::abs(d) - 1, round, solution[std::abs(d) - 1][round], solution, m_distance);
            if (backTrack(solution, newDistance, (i == 0) ? k : k - 1, (setOpponent) ? variablesLeft - 2 : variablesLeft - 1))
                return true;
        }

        if (setOpponent)
            solution[std::abs(d) - 1][round] = 0;
        resetDoamin(domainBackup);
    }
    solution[team][round] = 0;
    return false;
}
