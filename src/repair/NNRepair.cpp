/*
 * NNRepair.cpp
 *
 *  Created on: 22.11.2016
 *      Author: Christian
 */

#include "NNRepair.h"
#include <cmath>
#include <algorithm>
#include <iostream>

NNRepair::NNRepair(const mat2i& distance) :
        IRepair(distance)
{
}

NNRepair::~NNRepair()
{
}

void NNRepair::solve(const mat2i& solution)
{
    //TODO LDS!!!
    mat2i sol = solution;
    init(sol);
    backTrack(sol, 5);
}

bool NNRepair::backTrack(mat2i& solution, int lds)
{
    int team, round;
    if (!getNextVariable(team, round, solution))
    {
        int value = Common::eval(solution, m_distance);
        if (value < m_bestValue)
        {
            m_bestSolution = solution;
            m_bestValue = value;
            return true;
        }
        return false;
    }

    std::vector<int> domain = m_domain[team][round];
    std::sort(domain.begin(), domain.end(), [&] (int v1, int v2)
    {
        if (round == 0)
        {
            int dist1 = getDistance(team, team + 1, v1);
            int dist2 = getDistance(team, team + 1, v2);
            return dist1 < dist2;
        } else if (round == m_rounds - 1)
        {
            int dist1 = getDistance(team, solution[team][round-1], v1) + getDistance(team, v1, team + 1);
            int dist2 = getDistance(team, solution[team][round-1], v2) + getDistance(team, v2, team + 1);
            return dist1 < dist2;
        } else
        {
            int dist1 = getDistance(team, solution[team][round-1], v1);
            int dist2 = getDistance(team, solution[team][round-1], v2);
            return dist1 < dist2;
        }
    });

    int i = 0;
    for (auto d : domain)
    {
        //if i = 0 we follow the heuristic. only a number of lds choices allowed to not follow the heuristic
        if (i != 0 && lds == 0)
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
            if (backTrack(solution, (i == 0) ? lds : lds - 1))
                return true;
        }

        if (setOpponent)
            solution[std::abs(d) - 1][round] = 0;
        for (auto& b : domainBackup)
        {
            m_domain[b.m_team][b.m_round] = b.m_backup;
        }
        i++;
    }
    solution[team][round] = 0;
    return false;
}

bool NNRepair::getNextVariable(int& team, int& round, const mat2i& solution)
{
    //This heuristic assumes that all games before the current round are set
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

//team with index teamIdx plays against t1 and then t2, calculate travel distance
int NNRepair::getDistance(int teamIdx, int t1, int t2)
{
    //both play at home
    if (t1 > 0 && t2 > 0)
        return 0;

    //both play away
    if (t1 < 0 && t2 < 0)
        return m_distance[std::abs(t1) - 1][std::abs(t2) - 1];

    //t1 home, t2 away
    if (t1 > 0 && t2 < 0)
        return m_distance[teamIdx][std::abs(t2) - 1];

    //t1 away, t2 home
    return m_distance[std::abs(t1) - 1][teamIdx];
}
