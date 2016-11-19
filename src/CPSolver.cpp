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
        m_useMRV(true), m_useLCV(true), m_teams(distance.size()), m_rounds(2 * m_teams - 2), m_nodes(0), m_optimal(false), m_bestValue(0), m_distance(distance)
{
}

CPSolver::~CPSolver()
{
}

bool CPSolver::solve(mat2i& solution, bool optimal)
{
    m_optimal = optimal;
    m_bestValue = std::numeric_limits<int>::max();
    init(solution);
    m_nodes = 0;
    backTrack(solution);
    solution = m_bestSolution;
//    std::cout << "searched nodes: " << m_nodes << std::endl;
    return !solution.empty();
}

void CPSolver::init(mat2i& solution)
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
                        std::cerr << "Inconsistent solution" << std::endl;
                        exit(1);
                    }
                }
            }
        }
    }
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

    if (m_useLCV)
    {
        std::map<int, int> ruledOut;
        std::vector<int>& values = m_domain[team][round];
        for (unsigned int i = 0; i < values.size(); i++)
        {
            ruledOut[values[i]] = ruledOutValues(team, round, values[i], solution);
        }
//            std::random_shuffle(values.begin(), values.end());
        std::sort(values.begin(), values.end(), [&](int val1, int val2)
        {   return ruledOut[val1] < ruledOut[val2];});
    }

    for (auto d : m_domain[team][round])
    {
        m_nodes++;
        std::vector<DomainBackupEntry> domainBackup;
        solution[team][round] = d;

        if (d < 0)
            solution[-d - 1][round] = (team + 1);
        else
            solution[d - 1][round] = -(team + 1);

        if (forwardCheck(team, round, solution, domainBackup) && forwardCheck(std::abs(d) - 1, round, solution, domainBackup))
        {
            if (backTrack(solution))
                return true;
        }
        solution[std::abs(d) - 1][round] = 0;
        for (auto& b : domainBackup)
        {
            m_domain[b.m_team][b.m_round] = b.m_backup;
        }
    }
    solution[team][round] = 0;
    return false;
}

bool CPSolver::forwardCheck(int team, int round, const mat2i& solution, std::vector<DomainBackupEntry>& domainBackup)
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

bool CPSolver::contains(int team, int round, const std::vector<DomainBackupEntry>& domainBackup)
{
    for (auto& x : domainBackup)
    {
        if (x.m_team == team && round == x.m_round)
            return true;
    }
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

