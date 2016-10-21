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
                    if (!forwardCheck(t, r, solution, m_domain, domainBackup))
                    {
                        std::cerr << "Inconsistent solution" << std::endl;
                        exit(1);
                    }
                }
            }
        }
    }
}

bool Algorithm::forwardCheck(int team, int round, const mat2i& solution, mat3i& domain, std::vector<DomainEntry>& domainBackup)
{
    int value = solution[team][round];

    if (std::find(domain[team][round].begin(), domain[team][round].end(), value) == domain[team][round].end())
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
        if (solution[team][i] == 0)
        {
            std::vector<int>& _domain = domain[team][i];
            if (!contains(team, i, domainBackup))
                domainBackup.push_back(DomainEntry(team, i, _domain));
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
        if (solution[team][round - 1] == 0)
        {
            std::vector<int>& _domain = domain[team][round - 1];
            if (!contains(team, round - 1, domainBackup))
                domainBackup.push_back(DomainEntry(team, round - 1, _domain));
            _domain.erase(std::remove(_domain.begin(), _domain.end(), -value), _domain.end());
            if (_domain.empty())
                return false;
        }
    }
    if (round < m_rounds - 1)
    {
        if (solution[team][round + 1] == 0)
        {
            std::vector<int>& _domain = domain[team][round + 1];
            if (!contains(team, round + 1, domainBackup))
                domainBackup.push_back(DomainEntry(team, round + 1, _domain));
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
                        domainsOfVars.push_back(&domain[team][i + j]);
                        if (!contains(team, i + j, domainBackup))
                            domainBackup.push_back(DomainEntry(team, i + j, domain[team][i + j]));
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

bool Algorithm::getMrv(int& team, int& round, const mat2i& solution, const mat3i& domain)
{
    bool found = false;
    int teams = solution.size();
    int rounds = solution[0].size();
    int currDomSize = std::numeric_limits<int>::max();
    for (int t = 0; t < teams; t++)
    {
        for (int r = 0; r < rounds; r++)
        {
            if (solution[t][r] != 0)
                continue;
            int s = domain[t][r].size();
            if (s < currDomSize)
            {
                currDomSize = s;
                team = t;
                round = r;
                found = true;
            }
        }
    }
    return found;
}

int Algorithm::ruledOutValues(int team, int round, int value, const mat2i& solution, const mat3i& domain)
{
    int ruledOut = 0;
    ruledOut += ruledOutHorizontal(team, round, value, solution, domain);
    ruledOut += ruledOutVertical(team, round, value, solution, domain);
    ruledOut += ruledOutOpponent(team, round, value, solution, domain);
    ruledOut += ruledOutNoRepeat(team, round, value, solution, domain);
    ruledOut += ruledOutAtMost(team, round, value, solution, domain);

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
    ruledOut += ruledOutHorizontal(team, round, value, solution, domain);
    ruledOut += ruledOutVertical(team, round, value, solution, domain);
    ruledOut += ruledOutNoRepeat(team, round, value, solution, domain);
    ruledOut += ruledOutAtMost(team, round, value, solution, domain);
    return ruledOut;
}

int Algorithm::ruledOutHorizontal(int team, int round, int value, const mat2i& solution, const mat3i& domain)
{
    int cnt = 0;
    /*
     * Every team plays exactly once against each other team at home and away
     */
    for (int i = 0; i < m_rounds; i++)
    {
        if (i != round && solution[team][i] == 0)
        {
            const std::vector<int>& _domain = domain[team][i];
            if (std::find(_domain.begin(), _domain.end(), value) != _domain.end())
                cnt++;
        }
    }
    return cnt;
}

int Algorithm::ruledOutVertical(int team, int round, int value, const mat2i& solution, const mat3i& domain)
{
    int cnt = 0;
    /*
     * Every team plays only 1 game each round
     */
    for (int i = 0; i < m_teams; i++)
    {
        if (i != team && solution[i][round] == 0)
        {
            const std::vector<int>& _domain = domain[i][round];
            if (std::find(_domain.begin(), _domain.end(), value) != _domain.end())
                cnt++;
            if (std::find(_domain.begin(), _domain.end(), -value) != _domain.end())
                cnt++;
        }
    }
    return cnt;
}

int Algorithm::ruledOutOpponent(int team, int round, int value, const mat2i& solution, const mat3i& domain)
{
    int cnt = 0;
    /*
     * Opponent team has to play against this team: Set domain of the variable of the opponent game to -X[team][round]
     */
    if (solution[std::abs(value) - 1][round] == 0)
    {
        const std::vector<int>& opponentDomain = domain[std::abs(value) - 1][round];
        cnt += (opponentDomain.size() - 1);
    }

    return cnt;
}

int Algorithm::ruledOutNoRepeat(int team, int round, int value, const mat2i& solution, const mat3i& domain)
{
    int cnt = 0;
    /*
     * Repeated games not allowed, remove -X[team][round] from domain of variable X[team][round+1] and X[team][round-1]
     */
    if (round > 0)
    {
        if (solution[team][round - 1] == 0)
        {
            const std::vector<int>& _domain = domain[team][round - 1];
            if (std::find(_domain.begin(), _domain.end(), -value) != _domain.end())
                cnt++;
        }
    }
    if (round < m_rounds - 1)
    {
        if (solution[team][round + 1] == 0)
        {
            const std::vector<int>& _domain = domain[team][round + 1];
            if (std::find(_domain.begin(), _domain.end(), -value) != _domain.end())
                cnt++;
        }
    }
    return cnt;
}

int Algorithm::ruledOutAtMost(int team, int round, int value, const mat2i& solution, const mat3i& domain)
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
                        domainsOfVars.push_back(&domain[team][i + j]);
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

int Algorithm::conflicts(const mat2i& solution)
{
    int conflicts = 0;
    int teams = solution.size();
    int rounds = solution[0].size();

    /*
     * Every team plays only 1 game each round
     */
    for (int r = 0; r < rounds; r++)
    {
        std::vector<int> teamPlays(teams);
        for (int t = 0; t < teams; t++)
        {
            if (solution[t][r] != 0)
                teamPlays[std::abs(solution[t][r]) - 1]++;
        }
        for (auto tp : teamPlays)
        {
            if (tp != 1)
                conflicts++;
        }
    }

    /*
     * Every team plays exactly once against each other team at home and away
     */
    for (int t = 0; t < teams; t++)
    {
        for (int opponent = -teams; opponent <= teams; opponent++)
        {
            if (opponent == 0 || std::abs(opponent) == (t + 1))
                continue;
            int count = 0;
            for (int r = 0; r < rounds; r++)
            {
                if (solution[t][r] == opponent)
                {
                    count++;
                }
            }
            if (count != 1)
                conflicts++;
        }
    }

    /*
     * A team can not play against itself
     */
    for (int r = 0; r < rounds; r++)
    {
        for (int t = 0; t < teams; t++)
        {
            if (std::abs(solution[t][r]) == t + 1)
                conflicts++;
        }
    }

    /*
     * Opponent team has to play against this team
     */

    //Additional constraints:
    /*
     * Repeated games not allowed
     */
    for (int r = 1; r < rounds; r++)
    {
        for (int t = 0; t < teams; t++)
        {
            if (std::abs(solution[t][r - 1]) == std::abs(solution[t][r]))
                conflicts++;
        }
    }

    /*
     * At most U (fixed to 3 for now) consecutive games home or away
     */

    return conflicts;
}

std::set<std::pair<int, int>> Algorithm::getConflicted(const mat2i& solution)
{
    std::set<std::pair<int, int>> conflicted;
    int teams = solution.size();
    int rounds = solution[0].size();

    /*
     * Every team plays only 1 game each round
     */
    for (int r = 0; r < rounds; r++)
    {
        for (int t1 = 0; t1 < teams; t1++)
        {
            int curr = std::abs(solution[t1][r]);
            for (int t2 = 0; t2 < teams; t2++)
            {
                if (t2 != t1 && std::abs(solution[t2][r]) == curr)
                {
                    conflicted.insert(std::make_pair(t1, r));
                    break;
                }
            }
        }
    }

    /*
     * Every team plays exactly once against each other team at home and away
     */
    for (int t = 0; t < teams; t++)
    {
        for (int r1 = 0; r1 < rounds; r1++)
        {
            int curr = solution[t][r1];
            for (int r2 = 0; r2 < rounds; r2++)
            {
                if (r1 != r2 && solution[t][r2] == curr)
                {
                    conflicted.insert(std::make_pair(t, r1));
                    break;
                }
            }
        }
    }

    /*
     * A team can not play against itself
     */
    for (int r = 0; r < rounds; r++)
    {
        for (int t = 0; t < teams; t++)
        {
            if (std::abs(solution[t][r]) == t + 1)
            {
                conflicted.insert(std::make_pair(t, r));
            }
        }
    }

    //Additional constraints:
    /*
     * Repeated games not allowed
     */
    for (int r = 1; r < rounds; r++)
    {
        for (int t = 0; t < teams; t++)
        {
            if (std::abs(solution[t][r - 1]) == std::abs(solution[t][r]))
                conflicted.insert(std::make_pair(t, r - 1));
        }
    }

    return conflicted;
}
