/*
 * Common.cpp
 *
 *  Created on: 23.10.2016
 *      Author: Christian
 */

#include "Common.h"
#include <iostream>
#include <algorithm>

void Common::initSolution(int n, mat2i& solution)
{
    solution.resize(n);
    for (int i = 0; i < n; i++)
    {
        solution[i].resize(2 * n - 2);
    }
}

void Common::printMatrix(const mat2i& matrix)
{
    for (auto r : matrix)
    {
        for (auto t : r)
        {
            std::cout << t << " ";
        }
        std::cout << std::endl;
    }
}

int Common::eval(const mat2i& solution, const mat2i& distance)
{
    if (solution.empty())
        return 0;
    int costs = 0;
    int teams = distance.size();
    int rounds = 2 * distance.size() - 2;

    for (int team = 0; team < teams; team++)
    {
        //Home -> first opponent
        int opponent = solution[team][0];
        if (opponent < 0)
            costs += distance[team][std::abs(opponent) - 1];

        for (int round = 1; round < rounds; round++)
        {
            int start = solution[team][round - 1];
            opponent = solution[team][round];
            if (start < 0 && opponent > 0)
            {
                //Away -> Home
                costs += distance[std::abs(start) - 1][team];
            } else if (start > 0 && opponent < 0)
            {
                //Home -> Away
                costs += distance[team][std::abs(opponent) - 1];
            } else if (start < 0 && opponent < 0)
            {
                //Away -> Away
                costs += distance[std::abs(start) - 1][std::abs(opponent) - 1];
            }
        }

        //Last opponent -> home
        opponent = solution[team][rounds - 1];
        if (opponent < 0)
            costs += distance[std::abs(opponent) - 1][team];
    }
    return costs;
}

bool Common::forwardCheck(int team, int round, const mat2i& solution, mat3i& domain, std::vector<DomainBackupEntry>& domainBackup)
{
    int teams = solution.size();
    int rounds = 2 * teams - 2;
    int value = solution[team][round];

    if (std::find(domain[team][round].begin(), domain[team][round].end(), value) == domain[team][round].end())
        return false;

    /*
     * Every team plays only 1 game each round
     * Remove value of X[team][round] and -X[team][round] from the domains of all variables in the same round: X[k][round]
     */
    for (int i = 0; i < teams; i++)
    {
        if (solution[i][round] == 0)
        {
            std::vector<int>& _domain = domain[i][round];
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
    for (int i = 0; i < rounds; i++)
    {
        if (solution[team][i] == 0)
        {
            std::vector<int>& _domain = domain[team][i];
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
        std::vector<int>& opponentDomain = domain[std::abs(value) - 1][round];
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
            std::vector<int>& _domain = domain[team][round - 1];
            if (!contains(team, round - 1, domainBackup))
                domainBackup.push_back(DomainBackupEntry(team, round - 1, _domain));
            _domain.erase(std::remove(_domain.begin(), _domain.end(), -value), _domain.end());
            if (_domain.empty())
                return false;
        }
    }
    if (round < rounds - 1)
    {
        if (solution[team][round + 1] == 0)
        {
            std::vector<int>& _domain = domain[team][round + 1];
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
        if (i >= 0 && i < rounds)
        {
            int countAway = 0;
            int countHome = 0;
            std::vector<std::vector<int>*> domainsOfVars;
            for (int j = 0; j < u + 1; j++)
            {
                if (i + j < rounds)
                {
                    if (solution[team][i + j] < 0)
                    {
                        countAway++;
                    } else if (solution[team][i + j] > 0)
                    {
                        countHome++;
                    } else
                    {
                        domainsOfVars.push_back(&domain[team][i + j]);
                        if (!contains(team, i + j, domainBackup))
                            domainBackup.push_back(DomainBackupEntry(team, i + j, domain[team][i + j]));
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

bool Common::contains(int team, int round, const std::vector<DomainBackupEntry>& domainBackup)
{
    for (auto& x : domainBackup)
    {
        if (x.m_team == team && round == x.m_round)
            return true;
    }
    return false;
}
