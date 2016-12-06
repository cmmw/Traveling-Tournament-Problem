/*
 * IRepair.cpp
 *
 *  Created on: 21.11.2016
 *      Author: Christian
 */

#include "IRepair.h"
#include <algorithm>
#include <cassert>
#include <iostream>

IRepair::IRepair(const mat2i& distance) :
        m_teams(distance.size()), m_rounds(m_teams * 2 - 2), m_upperBound(std::numeric_limits<decltype(m_upperBound)>::max()), m_bestSolutionValue(std::numeric_limits<decltype(m_bestSolutionValue)>::max()), m_u(3), m_distance(distance)
{
}

mat2i IRepair::solve(const mat2i& solution, int upperBound)
{
    m_bestSolution.clear();
    m_upperBound = upperBound;
    m_bestSolutionValue = std::numeric_limits<decltype(m_bestSolutionValue)>::max();
    return solveImpl(solution);
}

void IRepair::init(mat2i& solution)
{
    std::vector<int> initDomain;
    m_domain.clear();
    for (int i = -m_teams; i < 0; i++)
    {
        initDomain.push_back(i);
        initDomain.push_back(-i);
    }

    for (int t = 0; t < m_teams; t++)
    {
        m_domain.push_back(mat2i());
        for (int r = 0; r < m_rounds; r++)
        {
            m_domain[t].push_back(initDomain);

            //A team cannot play against itself in any round
            m_domain[t][r].erase(std::remove(m_domain[t][r].begin(), m_domain[t][r].end(), t + 1), m_domain[t][r].end());
            m_domain[t][r].erase(std::remove(m_domain[t][r].begin(), m_domain[t][r].end(), -(t + 1)), m_domain[t][r].end());
        }
    }

    if (solution.empty())
    {
        Common::initSolution(m_teams, solution);
    } else
    {
        for (int t = 0; t < m_teams; t++)
        {
            for (int r = 0; r < m_rounds; r++)
            {
                if (solution[t][r] != 0)
                {
                    std::vector<DomainBackupEntry> domainBackup;
                    if (!forwardCheck(t, r, solution, domainBackup))
                    {
                        std::cerr << "Inconsistent solution:" << std::endl;
                        Common::printMatrix(solution);
                        exit(1);
                    }
                }
            }
        }
    }
}

bool IRepair::forwardCheck(int team, int round, const mat2i& solution, std::vector<DomainBackupEntry>& domainBackup)
{
    int value = solution[team][round];

    if (std::find(m_domain[team][round].begin(), m_domain[team][round].end(), value) == m_domain[team][round].end())
        return false;

    /*
     * Every team plays only 1 game each round
     * Remove value of X[team][round] and -X[team][round] from the domains of all variables in the same round: X[k][round]
     */
    for (int i = 0; i < m_teams; i++)
    {
        if (solution[i][round] == 0)
        {
            std::vector<int>& _domain = m_domain[i][round];
            if (!contains(i, round, domainBackup))
                domainBackup.push_back(DomainBackupEntry(i, round, _domain));
            _domain.erase(std::remove(_domain.begin(), _domain.end(), value), _domain.end());
            _domain.erase(std::remove(_domain.begin(), _domain.end(), -value), _domain.end());
            if (_domain.empty())
                return false;
        }
    }

    /*
     * Every team plays exactly once against each other team at home and away
     * Remove value X[team][round] from the domain of all variables with the same team: X[team][k] for all k
     */
    for (int i = 0; i < m_rounds; i++)
    {
        if (solution[team][i] == 0)
        {
            std::vector<int>& _domain = m_domain[team][i];
            if (!contains(team, i, domainBackup))
                domainBackup.push_back(DomainBackupEntry(team, i, _domain));
            _domain.erase(std::remove(_domain.begin(), _domain.end(), value), _domain.end());
            if (_domain.empty())
                return false;
        }
    }

    /*
     * Opponent team has to play against this team: Set domain of the variable of the opponent game to -X[team][round]
     */
    int opponent;
    if (value < 0)
        opponent = (team + 1);
    else
        opponent = -(team + 1);

    if (solution[std::abs(value) - 1][round] == 0)
    {
        std::vector<int>& opponentDomain = m_domain[std::abs(value) - 1][round];

        if (std::find(opponentDomain.begin(), opponentDomain.end(), opponent) == opponentDomain.end())
        {
            return false;
        } else
        {
            if (!contains(std::abs(value) - 1, round, domainBackup))
                domainBackup.push_back(DomainBackupEntry(std::abs(value) - 1, round, opponentDomain));
            opponentDomain.clear();
            opponentDomain.push_back(opponent);
        }

        //Also remove -team and +team in column from all other variables domains
        for (int i = 0; i < m_teams; i++)
        {
            if (i != std::abs(value) - 1 && solution[i][round] == 0)
            {
                std::vector<int>& _domain = m_domain[i][round];
                if (!contains(i, round, domainBackup))
                    domainBackup.push_back(DomainBackupEntry(i, round, _domain));
                _domain.erase(std::remove(_domain.begin(), _domain.end(), opponent), _domain.end());
                _domain.erase(std::remove(_domain.begin(), _domain.end(), -opponent), _domain.end());
                if (_domain.empty())
                    return false;
            }
        }

    } else if (solution[std::abs(value) - 1][round] != opponent)
    {
        return false;
    }

    //Additional constraints:
    /*
     * Repeated games not allowed, remove -X[team][round] from domain of variable X[team][round+1] and X[team][round-1]
     */
    if (round > 0)
    {
        if (solution[team][round - 1] == 0)
        {
            std::vector<int>& _domain = m_domain[team][round - 1];
            if (!contains(team, round - 1, domainBackup))
                domainBackup.push_back(DomainBackupEntry(team, round - 1, _domain));
            _domain.erase(std::remove(_domain.begin(), _domain.end(), -value), _domain.end());
            if (_domain.empty())
                return false;
        }
    }
    if (round < m_rounds - 1)
    {
        if (solution[team][round + 1] == 0)
        {
            std::vector<int>& _domain = m_domain[team][round + 1];
            if (!contains(team, round + 1, domainBackup))
                domainBackup.push_back(DomainBackupEntry(team, round + 1, _domain));
            _domain.erase(std::remove(_domain.begin(), _domain.end(), -value), _domain.end());
            if (_domain.empty())
                return false;
        }
    }

    /*
     * At most U (m_u) consecutive games home or away: take all subsets of consecutive variables with length U+1 and count number of
     * home or away games, if number is U -> remove all negative or positive values from domain of the unassigned variables domain
     */
    for (int i = round - m_u; i < round + 1; i++)
    {
        if (i >= 0 && i < m_rounds)
        {
            int countAway = 0;
            int countHome = 0;
            std::vector<std::vector<int>*> domainsOfVars;
            for (int j = 0; j < m_u + 1; j++)
            {
                if (i + j < m_rounds)
                {
                    if (solution[team][i + j] < 0)
                    {
                        countAway++;
                    } else if (solution[team][i + j] > 0)
                    {
                        countHome++;
                    } else
                    {
                        domainsOfVars.push_back(&m_domain[team][i + j]);
                        if (!contains(team, i + j, domainBackup))
                            domainBackup.push_back(DomainBackupEntry(team, i + j, m_domain[team][i + j]));
                    }
                }
            }
            if (countAway == m_u)
            {
                for (auto d : domainsOfVars)
                {
                    d->erase(std::remove_if(d->begin(), d->end(), [](int x)
                    {   return x < 0;}), d->end());
                    if (d->empty())
                        return false;
                }
            } else if (countHome == m_u)
            {
                for (auto d : domainsOfVars)
                {
                    d->erase(std::remove_if(d->begin(), d->end(), [](int x)
                    {   return x > 0;}), d->end());
                    if (d->empty())
                        return false;
                }
            }
        }
    }
    return true;
}

bool IRepair::contains(int team, int round, const std::vector<DomainBackupEntry>& domainBackup)
{
    for (auto& x : domainBackup)
    {
        if (x.m_team == team && round == x.m_round)
            return true;
    }
    return false;
}

void IRepair::resetDoamin(const std::vector<DomainBackupEntry>& domainBackup)
{
    for (auto& b : domainBackup)
    {
        m_domain[b.m_team][b.m_round] = b.m_backup;
    }
}
