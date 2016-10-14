/*
 * Algorithm.cpp
 *
 *  Created on: 08.10.2016
 *      Author: Christian
 */

#include "Algorithm.h"
#include <algorithm>

Algorithm::Algorithm(const mat2i& distances) :
        m_teams(distances.size()), m_rounds(2 * distances.size() - 2), m_distances(distances)
{
}

Algorithm::~Algorithm()
{
}

void Algorithm::init(std::vector<std::vector<int> >& solution)
{
    //Init domain
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

    //If solution is empty, create an empty m_teams x m_rounds matrix, otherwise reduce domain for each variable using the partial solution
    if (solution.empty())
    {
        solution.resize(m_teams);
        for (int i = 0; i < m_teams; i++)
        {
            solution[i].resize(m_rounds);
        }
    } else
    {
        for (int t = 0; t < m_teams; t++)
        {
            for (int r = 0; r < m_rounds; r++)
            {
                if (solution[t][r] != 0)
                {
                    std::vector<DomainEntry> domainBackup;
                    if (!forwardCheck(t + 1, r, solution, m_domain, domainBackup))
                    {
                        std::cerr << "Inconsistent solution" << std::endl;
                        exit(1);
                    }
                }
            }
        }
    }
}

int Algorithm::getNumTeams()
{
    return m_teams;
}

int Algorithm::getNumRounds()
{
    return m_rounds;
}

bool Algorithm::forwardCheck(int team, int round, const mat2i& solution, mat3i& domain, std::vector<DomainEntry>& domainBackup)
{
    int value = solution[team - 1][round];

    if (std::find(domain[team - 1][round].begin(), domain[team - 1][round].end(), value) == domain[team - 1][round].end())
        return false;

    /*
     * Every team plays only 1 game each round
     * Remove value of X[team][round] and -X[team][round] from the domains of all variables in the same round: X[k][round]
     */
    for (int i = 0; i < m_teams; i++)
    {
        if (solution[i][round] == 0)
        {
            std::vector<int>& _domain = domain[i][round];
            if (!contains(i, round, domainBackup))
                domainBackup.push_back(DomainEntry(i, round, _domain));
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
        if (solution[team - 1][i] == 0)
        {
            std::vector<int>& _domain = domain[team - 1][i];
            if (!contains(team - 1, i, domainBackup))
                domainBackup.push_back(DomainEntry(team - 1, i, _domain));
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
            opponent = team;
        } else
        {
            opponent = -team;
        }

        if (std::find(opponentDomain.begin(), opponentDomain.end(), opponent) == opponentDomain.end())
        {
            return false;
        } else
        {
            if (!contains(std::abs(value) - 1, round, domainBackup))
                domainBackup.push_back(DomainEntry(std::abs(value) - 1, round, opponentDomain));
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
        if (solution[team - 1][round - 1] == 0)
        {
            std::vector<int>& _domain = domain[team - 1][round - 1];
            if (!contains(team - 1, round - 1, domainBackup))
                domainBackup.push_back(DomainEntry(team - 1, round - 1, _domain));
            _domain.erase(std::remove(_domain.begin(), _domain.end(), -value), _domain.end());
            if (_domain.empty())
                return false;
        }
    }
    if (round < m_rounds - 1)
    {
        if (solution[team - 1][round + 1] == 0)
        {
            std::vector<int>& _domain = domain[team - 1][round + 1];
            if (!contains(team - 1, round + 1, domainBackup))
                domainBackup.push_back(DomainEntry(team - 1, round + 1, _domain));
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
                    if (solution[team - 1][i + j] < 0)
                    {
                        countAway++;
                    } else if (solution[team - 1][i + j] > 0)
                    {
                        countHome++;
                    } else
                    {
                        domainsOfVars.push_back(&domain[team - 1][i + j]);
                        if (!contains(team - 1, i + j, domainBackup))
                            domainBackup.push_back(DomainEntry(team - 1, i + j, domain[team - 1][i + j]));
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

bool Algorithm::contains(int team, int round, const std::vector<DomainEntry>& domainBackup)
{
    for (auto& x : domainBackup)
    {
        if (x.m_team == team && round == x.m_round)
            return true;
    }
    return false;
}

int Algorithm::eval(const std::vector<std::vector<int> >& solution, const mat2i& distances)
{
    int costs = 0;
    int teams = distances.size();
    int rounds = 2 * distances.size() - 2;

    for (int team = 0; team < teams; team++)
    {
        //Home -> first opponent
        int opponent = solution[team][0];
        if (opponent < 0)
            costs += distances[team][std::abs(opponent) - 1];

        for (int round = 1; round < rounds; round++)
        {
            int start = solution[team][round - 1];
            opponent = solution[team][round];
            if (start < 0 && opponent > 0)
            {
                //Away -> Home
                costs += distances[std::abs(start) - 1][team];
            } else if (start > 0 && opponent < 0)
            {
                //Home -> Away
                costs += distances[team][std::abs(opponent) - 1];
            } else if (start < 0 && opponent < 0)
            {
                //Away -> Away
                costs += distances[std::abs(start) - 1][std::abs(opponent) - 1];
            }
        }

        //Last opponent -> home
        opponent = solution[team][rounds - 1];
        if (opponent < 0)
            costs += distances[std::abs(opponent) - 1][team];
    }
    return costs;
}

bool Algorithm::ValueSorter::operator()(int value1, int value2)
{
    int costs1 = 0;
    int costs2 = 0;
    value1 = std::abs(value1);
    value2 = std::abs(value2);
    if (m_currRound > 0)
    {
        int opponent = m_solution[m_currTeam - 1][m_currRound - 1];
        if (opponent != 0)
        {
            costs1 += m_distances[std::abs(opponent) - 1][value1 - 1];
            costs2 += m_distances[std::abs(opponent) - 1][value2 - 1];
        }
    }

    if (m_currRound < m_rounds - 2)
    {
        int opponent = m_solution[m_currTeam - 1][m_currRound + 1];
        if (opponent != 0)
        {
            costs1 += m_distances[value1 - 1][std::abs(opponent) - 1];
            costs2 += m_distances[value2 - 1][std::abs(opponent) - 1];
        }
    }
    return costs1 < costs2;
}

