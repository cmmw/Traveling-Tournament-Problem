/*
 * IRepair.cpp
 *
 *  Created on: 21.11.2016
 *      Author: Christian
 */

#include "IRepair.h"
#include <algorithm>
#include <iostream>

IRepair::IRepair(const mat2i& distance) :
        m_teams(distance.size()), m_rounds(m_teams * 2 - 2), m_upperBound(std::numeric_limits<int>::max()), m_bestSolutionValue(std::numeric_limits<int>::max()), m_distance(distance)
{
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

mat2i IRepair::solve(const mat2i& solution, int upperBound)
{
    m_bestSolution.clear();
    m_upperBound = upperBound;
    m_bestSolutionValue = std::numeric_limits<int>::max();
    mat2i sol = solution;
    init(sol);
    backTrack(sol, 5);
    return m_bestSolution;
}

bool IRepair::backTrack(mat2i& solution, int lds)
{
    int team, round;
    if (!getNextVariable(team, round, solution))
    {
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
    if (solution[std::abs(value) - 1][round] == 0)
    {
        std::vector<int>& opponentDomain = m_domain[std::abs(value) - 1][round];
        int opponent;
        if (value < 0)
        {
            opponent = (team + 1);
        } else
        {
            opponent = -(team + 1);
        }

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
            std::vector<std::vector<int>*> domainsOfVars;
            for (int j = 0; j < u + 1; j++)
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
            if (countAway == u)
            {
                for (auto d : domainsOfVars)
                {
                    d->erase(std::remove_if(d->begin(), d->end(), [](int x)
                    {   return x < 0;}), d->end());
                    if (d->empty())
                        return false;
                }
            } else if (countHome == u)
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

