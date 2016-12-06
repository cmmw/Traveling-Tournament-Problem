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
    while (k <= kMax && !backTrack(sol, k++, vars))
        ;

    //Try to improve the solution
    if (!m_bestSolution.empty())
        improve(m_bestSolution);

    return m_bestSolution;
}

void IBacktrack::improve(mat2i& solution)
{
    mat2i map = Common::calcTravelMap(solution);

    //Improve solution by swapping teams, find good pairs to swap
    for (int i = 0; i < 200; i++)
    {
        //teams to swap
        int t1;
        int t2;
        do
        {
            t1 = (std::rand() % m_teams) + 1;
            t2 = (std::rand() % m_teams) + 1;
        } while (t1 == t2);

        //calculate improvement
        int o1 = 0;
        int n1 = 0;
        int o2 = 0;
        int n2 = 0;
        for (int j = 0; j < m_teams; j++)
        {
            if (j == t2 - 1 || j == t1 - 1)
                continue;

            o1 += (map[t1 - 1][j] * m_distance[t1 - 1][j]);
            o2 += (map[t2 - 1][j] * m_distance[t2 - 1][j]);

            n1 += (map[t1 - 1][j] * m_distance[t2 - 1][j]);
            n2 += (map[t2 - 1][j] * m_distance[t1 - 1][j]);
        }

//        int v = Common::eval(solution, m_distance);
//        v += (n1 + n2 - o1 - o2);
        int improvement = (n1 + n2 - o1 - o2);

        //If improvement is negative (reduce costs)
        if (improvement < 0)
        {
            for (auto& r : solution)
            {
                for (auto& t : r)
                {
                    if (std::abs(t) == t1)
                        t = t2 * (t / std::abs(t));
                    else if (std::abs(t) == t2)
                        t = t1 * (t / std::abs(t));
                }
            }
            std::swap(solution[t1 - 1], solution[t2 - 1]);
            map = Common::calcTravelMap(solution);
        }
    }
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
        resetDoamin(domainBackup);
    }
    solution[team][round] = 0;
    return false;
}
