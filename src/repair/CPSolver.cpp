/*
 * CPSolver.cpp
 *
 *  Created on: 23.10.2016
 *      Author: Christian
 */

#include "CPSolver.h"

#include <algorithm>
#include <queue>
#include <map>
#include <iostream>

CPSolver::CPSolver(const mat2i& distance) :
        IRepair(distance), m_useMRV(true), m_useLCV(true), m_nodes(0), m_optimal(false), m_bestValue(0)
{
}

CPSolver::~CPSolver()
{
}

mat2i CPSolver::solve(const mat2i& solution, bool optimal)
{
    mat2i sol = solution;
    m_optimal = optimal;
    m_bestValue = std::numeric_limits<int>::max();
    init(sol);
    m_nodes = 0;
    m_bestSolution.clear();
    backTrack(sol);
    sol = m_bestSolution;
//    std::cout << "searched nodes: " << m_nodes << std::endl;
    return sol;
}

bool CPSolver::backTrack(mat2i& solution)
{
    int team, round;

    if (!getUnassignedVar(team, round, solution))
    {
        if (m_optimal)
        {
            int value = Common::eval(solution, m_distance);
            if (value < m_bestValue)
            {
                m_bestSolution = solution;
                m_bestValue = value;
            }
            return false;
        } else
        {
            m_bestSolution = solution;
            return true;
        }
    }

    std::vector<int> domain = m_domain[team][round];
    if (m_useLCV)
    {
        std::map<int, int> ruledOut;
        std::vector<int>& values = domain;
        for (unsigned int i = 0; i < values.size(); i++)
        {
            ruledOut[values[i]] = ruledOutValues(team, round, values[i], solution);
        }
//            std::random_shuffle(values.begin(), values.end());
        std::sort(values.begin(), values.end(), [&](int val1, int val2)
        {   return ruledOut[val1] < ruledOut[val2];});
    }

    for (auto d : domain)
    {
        m_nodes++;
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

bool CPSolver::getUnassignedVar(int& team, int& round, const mat2i& solution)
{
    if (m_useMRV)
    {
        return getMRV(team, round, solution);
    } else
    {
        return getNextUnassignedVar(team, round, solution);
    }
}

bool CPSolver::getNextUnassignedVar(int& team, int& round, const mat2i& solution)
{
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
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

bool CPSolver::getMRV(int& team, int& round, const mat2i& solution)
{
    bool found = false;
    unsigned int rv = std::numeric_limits<unsigned int>::max();
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            if (solution[t][r] == 0)
            {
                if (rv > m_domain[t][r].size())
                {
                    team = t;
                    round = r;
                    rv = m_domain[t][r].size();
                    found = true;
                }
            }
        }
    }
    return found;
}

int CPSolver::ruledOutValues(int team, int round, int value, const mat2i& solution)
{
    int ruledOut = 0;
    ruledOut += ruledOutHorizontal(team, round, value, solution);
    ruledOut += ruledOutVertical(team, round, value, solution);
    ruledOut += ruledOutOpponent(team, round, value, solution);
    ruledOut += ruledOutNoRepeat(team, round, value, solution);
    ruledOut += ruledOutAtMost(team, round, value, solution);

    //Opponent is also set and will restrict values
    int tmp = team;
    if (value < 0)
    {
        team = -value - 1;
        value = tmp + 1;
    } else
    {
        team = value - 1;
        value = -(tmp + 1);
    }
    ruledOut += ruledOutHorizontal(team, round, value, solution);
    ruledOut += ruledOutVertical(team, round, value, solution);
    ruledOut += ruledOutNoRepeat(team, round, value, solution);
    ruledOut += ruledOutAtMost(team, round, value, solution);
    return ruledOut;
}

int CPSolver::ruledOutHorizontal(int team, int round, int value, const mat2i& solution)
{
    int cnt = 0;
    /*
     * Every team plays exactly once against each other team at home and away
     */
    for (int i = 0; i < m_rounds; i++)
    {
        if (i != round && solution[team][i] == 0)
        {
            const std::vector<int>& _domain = m_domain[team][i];
            if (std::find(_domain.begin(), _domain.end(), value) != _domain.end())
                cnt++;
        }
    }
    return cnt;
}

int CPSolver::ruledOutVertical(int team, int round, int value, const mat2i& solution)
{
    int cnt = 0;
    /*
     * Every team plays only 1 game each round
     */
    for (int i = 0; i < m_teams; i++)
    {
        if (i != team && solution[i][round] == 0)
        {
            const std::vector<int>& _domain = m_domain[i][round];
            if (std::find(_domain.begin(), _domain.end(), value) != _domain.end())
                cnt++;
            if (std::find(_domain.begin(), _domain.end(), -value) != _domain.end())
                cnt++;
        }
    }
    return cnt;
}

int CPSolver::ruledOutOpponent(int team, int round, int value, const mat2i& solution)
{
    int cnt = 0;
    /*
     * Opponent team has to play against this team: Set domain of the variable of the opponent game to -X[team][round]
     */
    if (solution[std::abs(value) - 1][round] == 0)
    {
        const std::vector<int>& opponentDomain = m_domain[std::abs(value) - 1][round];
        cnt += (opponentDomain.size() - 1);
    }

    return cnt;
}

int CPSolver::ruledOutNoRepeat(int team, int round, int value, const mat2i& solution)
{
    int cnt = 0;
    /*
     * Repeated games not allowed, remove -X[team][round] from domain of variable X[team][round+1] and X[team][round-1]
     */
    if (round > 0)
    {
        if (solution[team][round - 1] == 0)
        {
            const std::vector<int>& _domain = m_domain[team][round - 1];
            if (std::find(_domain.begin(), _domain.end(), -value) != _domain.end())
                cnt++;
        }
    }
    if (round < m_rounds - 1)
    {
        if (solution[team][round + 1] == 0)
        {
            const std::vector<int>& _domain = m_domain[team][round + 1];
            if (std::find(_domain.begin(), _domain.end(), -value) != _domain.end())
                cnt++;
        }
    }
    return cnt;
}

int CPSolver::ruledOutAtMost(int team, int round, int value, const mat2i& solution)
{
    int cnt = 0;
    /*
     * At most U (fixed to 3 for now) consecutive games home or away: take all subsets of consecutive variables with length U+1 and count number of
     * home or away games, if number is U -> remove all negative or positive values from domain of the unassigned variables domain
     */
    int u = 3;
    for (int i = round - u; i < round + 1; i++)
    {
        if (i >= 0 && i < m_rounds)
        {
            int countAway = 0;
            int countHome = 0;
            if (value < 0)
                countAway++;
            else
                countHome++;
            std::vector<const std::vector<int>*> domainsOfVars;
            for (int j = 0; j < u + 1; j++)
            {
                if (i + j < m_rounds)
                {
                    if (i + j != round && solution[team][i + j] < 0)
                    {
                        countAway++;
                    } else if (i + j != round && solution[team][i + j] > 0)
                    {
                        countHome++;
                    } else if (i + j != round)
                    {
                        domainsOfVars.push_back(&m_domain[team][i + j]);
                    }
                }
            }
            if (countAway == u)
            {
                for (auto d : domainsOfVars)
                {
                    cnt += std::count_if(d->begin(), d->end(), [](int x)
                    {   return x < 0;});
                }
            } else if (countHome == u)
            {
                for (auto d : domainsOfVars)
                {
                    cnt += std::count_if(d->begin(), d->end(), [](int x)
                    {   return x > 0;});
                }
            }
        }
    }
    return cnt;
}

