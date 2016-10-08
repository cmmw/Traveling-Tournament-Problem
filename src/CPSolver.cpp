/*
 * GreedyHeu.cpp
 *
 *  Created on: 05.10.2016
 *      Author: Christian
 */

#include "CPSolver.h"
#include <algorithm>
#include <iostream>
#include <limits>

CPSolver::CPSolver(const std::vector<std::vector<int>>& solution, const std::vector<std::vector<int>>& distances) :
        m_teams(distances.size()), m_rounds(2 * distances.size() - 2), m_solution(solution), m_distances(distances), m_bestCosts(std::numeric_limits<int>::max())
{
    initDomain();
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            if (m_solution[t][r] != 0)
            {
                if (!forwardCheck(t + 1, r, m_solution, m_domain))
                {
                    std::cerr << "Inconsistent solution" << std::endl;
                    exit(1);
                }
            }
        }
    }
}

CPSolver::CPSolver(const std::vector<std::vector<int> >& distances) :
        m_teams(distances.size()), m_rounds(2 * distances.size() - 2), m_distances(distances), m_bestCosts(std::numeric_limits<int>::max())
{
    initDomain();
}

CPSolver::~CPSolver()
{
}

void CPSolver::initDomain()
{
    std::vector<int> initDomain;
    m_domain.clear();
    for (int i = -m_teams; i < 0; i++)
    {
        initDomain.push_back(i);
        initDomain.push_back(-i);
    }

    m_solution.resize(m_teams);
    for (int t = 0; t < m_teams; t++)
    {
        m_solution[t].resize(m_rounds);
        m_domain.push_back(std::vector<std::vector<int>>());
        for (int r = 0; r < m_rounds; r++)
        {
            m_domain[t].push_back(initDomain);

            //A team cannot play against itself in any round
            m_domain[t][r].erase(std::remove(m_domain[t][r].begin(), m_domain[t][r].end(), t + 1), m_domain[t][r].end());
            m_domain[t][r].erase(std::remove(m_domain[t][r].begin(), m_domain[t][r].end(), -(t + 1)), m_domain[t][r].end());
        }
    }
}

bool CPSolver::solve()
{
    std::vector<std::vector<int>> solution = m_solution;
    backTrack(1, 0, solution, m_domain);
    return !m_bestSolution.empty();
}

bool CPSolver::backTrack(int team, int round, std::vector<std::vector<int>>& solution, std::vector<std::vector<std::vector<int>>>& domain)
{
    if (team > m_teams)
    {
        return backTrack(1, round+1, solution, domain);
    }
    if(round >= m_rounds)
    {
        int costs = CPSolver::eval(solution, m_distances);
        if(costs < m_bestCosts)
        {
            m_bestCosts = costs;
            m_bestSolution = solution;
            std::cout << m_bestCosts << std::endl;
        }
        return false;   //false -> search further, true -> end search
    }

    if(solution[team - 1][round] != 0)
    {
        return backTrack(team + 1, round, solution, domain);
    }

    for(auto d : domain[team - 1][round])
    {
        auto domainCopy = domain;
        solution[team - 1][round] = d;
        if(forwardCheck(team, round, solution, domainCopy))
        {
            if(backTrack(team + 1, round, solution, domainCopy))
            {
                return true;
            }
        }
    }
    solution[team - 1][round] = 0;
    return false;
}

std::vector<std::vector<int> > CPSolver::getSolution()
{
    return m_bestSolution;
}

bool CPSolver::forwardCheck(int team, int round, const std::vector<std::vector<int>>& solution, std::vector<std::vector<std::vector<int>>>& domain)
{
    int value = solution[team - 1][round];

    /*
     * Every team plays only 1 game each round
     * Remove value of X[team][round] and -X[team][round] from the domains of all variables in the same round: X[k][round]
     */
    for (int i = 0; i < m_teams; i++)
    {
        if(solution[i][round] == 0)
        {
            std::vector<int>& _domain = domain[i][round];
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
        if(solution[team - 1][i] == 0)
        {
            std::vector<int>& _domain = domain[team - 1][i];
            _domain.erase(std::remove(_domain.begin(), _domain.end(), value), _domain.end());
            if (_domain.empty())
            return false;
        }
    }

    /*
     * Opponent team has to play against this team: Set domain of the variable of the opponent game to -X[team][round]
     */
    if(solution[std::abs(value) - 1][round] == 0)
    {
        std::vector<int>& opponentDomain = domain[std::abs(value) - 1][round];
        int opponent;
        if(value < 0)
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
            opponentDomain.clear();
            opponentDomain.push_back(opponent);
        }
    }

    //Additional constraints:
    /*
     * Repeated games not allowed, remove -X[team][round] from domain of variable X[team][round+1] and X[team][round-1]
     */
    if(round > 0)
    {
        if(solution[team-1][round-1] == 0)
        {
            std::vector<int>& _domain = domain[team-1][round-1];
            _domain.erase(std::remove(_domain.begin(), _domain.end(), -value), _domain.end());
            if(_domain.empty())
            return false;
        }
    }
    if(round < m_rounds-1)
    {
        if(solution[team-1][round+1] == 0)
        {
            std::vector<int>& _domain = domain[team-1][round+1];
            _domain.erase(std::remove(_domain.begin(), _domain.end(), -value), _domain.end());
            if(_domain.empty())
            return false;
        }
    }

    /*
     * At most U (fixed to 3 for now) consecutive games home or away: take all subsets of consecutive variables with length U and count number of
     * home or away games, if number is U-1 -> remove all negative or positive values from domain of the unassigned variables domain
     */
    int u = 3;
    for(int i = round - u + 1; i < round + 1; i++)
    {
        if(i >= 0 && i < m_rounds)
        {
            int countAway = 0;
            int countHome = 0;
            std::vector<std::vector<int>*> domainsOfVars;
            for(int j = 0; j < u; j++)
            {
                if(i + j < m_rounds)
                {
                    if(solution[team-1][i+j] < 0)
                    {
                        countAway++;
                    } else if(solution[team-1][i+j] > 0)
                    {
                        countHome++;
                    } else
                    {
                        domainsOfVars.push_back(&domain[team-1][i+j]);
                    }
                }
            }
            if(countAway == u-1)
            {
                for(auto d : domainsOfVars)
                {
                    d->erase(std::remove_if(d->begin(), d->end(), [](int x)
                            {   return x < 0;}), d->end());
                    if(d->empty())
                    return false;
                }
            } else if(countHome == u-1)
            {
                for(auto d : domainsOfVars)
                {
                    d->erase(std::remove_if(d->begin(), d->end(), [](int x)
                            {   return x > 0;}), d->end());
                    if(d->empty())
                    return false;
                }
            }
        }
    }
    return true;
}

int CPSolver::eval(const std::vector<std::vector<int> >& solution, const std::vector<std::vector<int>>& distances)
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
