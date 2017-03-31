/*
 * HillClimber.cpp
 *
 *  Created on: Mar 13, 2017
 *      Author: christian
 */

#include "HillClimber.h"
#include <cmath>
#include <algorithm>
#include <set>
#include <iostream>

HillClimber::HillClimber(const mat2i& solution, const mat2i& distance) :
        m_teams(solution.size()), m_rounds(m_teams * 2 - 2), m_findBestImprovement(false), m_solution(solution), m_distance(distance)
{
//    m_neighborhoods.push_back(&HillClimber::swapHomes);
    m_neighborhoods.push_back(&HillClimber::swapRounds);
    m_neighborhoods.push_back(&HillClimber::swapTeams);
    m_neighborhoods.push_back(&HillClimber::swapPartialRounds);
    m_neighborhoods.push_back(&HillClimber::swapPartialTeams);
}

HillClimber::~HillClimber()
{
}

mat2i HillClimber::solve()
{
    int distance = Common::eval(m_solution, m_distance);
    mat2i solution;
    bool run = true;
    while (run)
    {
        run = false;
        //For all neighborhoods
        for (swapFunc f : m_neighborhoods)
        {
            run = run || (this->*f)(solution, distance);
            if (!m_findBestImprovement && run)
                break;
        }
        if (run)
            m_solution = solution;
    }
    return m_solution;
}

bool HillClimber::swapHomes(mat2i& solution, int& distance)
{
    bool improved = false;
    mat2i cpy = m_solution;
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            if (cpy[t][r] == 0)
                continue;

            int& c1 = m_solution[t][r];
            int otherTeam = std::abs(c1) - 1;
            int& c2 = m_solution[otherTeam][r];

            //Search the round of the opposite game
            int r2 = 0;
            while (true)
            {
                if (-c1 == m_solution[t][r2])
                    break;
                r2++;
            }

            //Move
            int& cc1 = m_solution[t][r2];
            int& cc2 = m_solution[otherTeam][r2];
            c1 = -c1;
            c2 = -c2;
            cc1 = -cc1;
            cc2 = -cc2;

            //If no constraint violated
            if (!(Common::atMost(t, r, m_solution) > 3 || Common::atMost(otherTeam, r, m_solution) > 3 ||
                    Common::atMost(t, r2, m_solution) > 3 || Common::atMost(otherTeam, r2, m_solution) > 3))
            {
                int distTmp = Common::eval(m_solution, m_distance);
                if (distTmp < distance)
                {
                    Common::printMatrix(m_solution);
                    std::cout << "sh---" << std::endl;
                    improved = true;
                    distance = distTmp;
                    solution = m_solution;
                }
            }

            //Undo move
            c1 = -c1;
            c2 = -c2;
            cc1 = -cc1;
            cc2 = -cc2;

            if (improved && !m_findBestImprovement)
                return true;

            //delete already considered homes
            cpy[t][r] = 0;
            cpy[otherTeam][r] = 0;
            cpy[t][r2] = 0;
            cpy[otherTeam][r2] = 0;
        }
    }
    return improved;
}

bool HillClimber::swapRounds(mat2i& solution, int& distance)
{
    bool improved = false;
    for (int r1 = 0; r1 < m_rounds - 1; r1++)
    {
        for (int r2 = r1 + 1; r2 < m_rounds; r2++)
        {
            bool next = false;
            for (int t = 0; t < m_teams; t++)
            {
                //Swap teams
                int& c1 = m_solution[t][r1];
                int& c2 = m_solution[t][r2];
                std::swap(c1, c2);
                //If swap violates atMost or noRepeat constraint
                if ((std::signbit(c1) != std::signbit(c2) && (Common::atMost(t, r1, m_solution) > 3 ||
                        Common::atMost(t, r2, m_solution) > 3)) || !Common::noRepeat(t, r1, m_solution) ||
                        !Common::noRepeat(t, r2, m_solution))
                {
                    for (int t0 = 0; t0 <= t; t0++)
                        std::swap(m_solution[t0][r1], m_solution[t0][r2]);
                    next = true;
                    break;
                }
            }
            if (!next)
            {
                //Check if neighbor is better
                int distTmp = Common::eval(m_solution, m_distance);
                if (distTmp < distance)
                {
                    Common::printMatrix(m_solution);
                    std::cout << "sr---" << std::endl;
                    improved = true;
                    distance = distTmp;
                    solution = m_solution;
                }

                //Undo move
                for (int t = 0; t < m_teams; t++)
                    std::swap(m_solution[t][r1], m_solution[t][r2]);

                if (improved && !m_findBestImprovement)
                    return true;
            }
        }
    }
    return improved;
}

bool HillClimber::swapTeams(mat2i& solution, int& distance)
{
    bool improved = false;
    for (int t1 = 0; t1 < m_teams - 1; t1++)
    {
        for (int t2 = t1 + 1; t2 < m_teams; t2++)
        {
            bool next = false;
            //Swap teams
            for (int r = 0; r < m_rounds; r++)
            {
                int& c1 = m_solution[t1][r];
                int& c2 = m_solution[t2][r];
                int tt1 = std::abs(c1) - 1;
                int tt2 = std::abs(c2) - 1;
                int& cc1 = m_solution[tt1][r];
                int& cc2 = m_solution[tt2][r];
                std::swap(c1, c2);
                std::swap(cc1, cc2);
                Common::swapSigns(cc1, cc2);
            }

            //Check if swaps caused violations of the constraints
            for (int r = 0; r < m_rounds; r++)
            {
                int c1 = m_solution[t1][r];
                int c2 = m_solution[t2][r];
                int tt1 = std::abs(c1) - 1;
                int tt2 = std::abs(c2) - 1;
                if (Common::atMost(t1, r, m_solution) > 3 || Common::atMost(t2, r, m_solution) > 3 ||
                        Common::atMost(tt1, r, m_solution) > 3 || Common::atMost(tt2, r, m_solution) > 3 ||
                        !Common::noRepeat(t1, r, m_solution) || !Common::noRepeat(t2, r, m_solution) ||
                        !Common::noRepeat(tt1, r, m_solution) || !Common::noRepeat(tt2, r, m_solution))
                {
                    next = true;
                    break;
                }
            }

            if (!next)
            {
                //Check if neighbor is better
                int distTmp = Common::eval(m_solution, m_distance);
                if (distTmp < distance)
                {
                    std::cout << "***" << std::endl;
                    std::cout << t1 << ", " << t2 << std::endl;
                    Common::printMatrix(m_solution);
                    std::cout << "st---" << std::endl;
                    improved = true;
                    distance = distTmp;
                    solution = m_solution;
                }
            }

            //Undo move
            for (int r = 0; r < m_rounds; r++)
            {
                int& c1 = m_solution[t1][r];
                int& c2 = m_solution[t2][r];
                int tt1 = std::abs(c1) - 1;
                int tt2 = std::abs(c2) - 1;
                int& cc1 = m_solution[tt1][r];
                int& cc2 = m_solution[tt2][r];
                std::swap(c1, c2);
                std::swap(cc1, cc2);
                Common::swapSigns(cc1, cc2);
            }
            if (improved && !m_findBestImprovement)
                return true;
        }
    }
    return improved;
}

bool HillClimber::swapPartialRounds(mat2i& solution, int& distance)
{
    bool improved = false;
    for (int r1 = 0; r1 < m_rounds - 1; r1++)
    {
        for (int r2 = r1 + 1; r2 < m_rounds; r2++)
        {
            for (int t = 0; t < m_teams; t++)
            {
                bool next = false;
                int rr1 = r1;
                int rr2 = r2;
                std::vector<int> teamsToSwap;
                teamsToSwap.push_back(t);
                int tt = std::abs(m_solution[t][r1]) - 1;
                teamsToSwap.push_back(tt);
                while (true)
                {
                    tt = m_solution[tt][rr2];
                    tt = std::abs(tt) - 1;

                    if (std::find(teamsToSwap.begin(), teamsToSwap.end(), tt) != teamsToSwap.end())
                        break;

                    teamsToSwap.push_back(tt);
                    std::swap(rr1, rr2);
                }
                //TODO: don't test teams in teamsToSwap again (use a list instead of going though all teams and remove the ones in teamsToSwap)

                //If all teams have to be swapped, skip (already done in swapRounds)
                if ((int) teamsToSwap.size() == m_teams)
                    break;

                //Swap teams
                for (int t0 : teamsToSwap)
                {
                    std::swap(m_solution[t0][r1], m_solution[t0][r2]);
                }

                //Check constraints
                for (int t0 : teamsToSwap)
                {
                    if (Common::atMost(t0, r1, m_solution) > 3 || Common::atMost(t0, r2, m_solution) > 3 ||
                            !Common::noRepeat(t0, r1, m_solution) || !Common::noRepeat(t0, r2, m_solution))
                    {
                        next = true;
                        break;
                    }
                }

                if (!next)
                {
                    //Check if neighbor is better
                    int distTmp = Common::eval(m_solution, m_distance);
                    if (distTmp < distance)
                    {
                        Common::printMatrix(m_solution);
                        std::cout << "spr---" << std::endl;
                        improved = true;
                        distance = distTmp;
                        solution = m_solution;
                    }
                }

                //Undo move
                for (int t0 : teamsToSwap)
                {
                    std::swap(m_solution[t0][r1], m_solution[t0][r2]);
                }

                if (improved && !m_findBestImprovement)
                    return true;
            }
        }
    }
    return improved;
}

bool HillClimber::swapPartialTeams(mat2i& solution, int& distance)
{
    //inRund[0][1] = 2 means team 0 plays at home against team 1 in round 2
    //-1 means no such game exists in the current plan
    mat2i inRound(m_teams, std::vector<int>(m_teams, -1));
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int o = m_solution[t][r];
            if (o > 0)
                inRound[t][o - 1] = r;
        }
    }
    std::cout << "instance: " << std::endl;
    Common::printMatrix(m_solution);
    std::cout << "**" << std::endl;
    bool improved = false;
    for (int t1 = 0; t1 < m_teams - 1; t1++)
    {
        for (int t2 = t1 + 1; t2 < m_teams; t2++)
        {
            for (int r = 0; r < m_rounds; r++)
            {
                //TODO skip all rounds that were already considered after each loop
                bool next = false;
                //If t1 plays against t2
                if (std::abs(m_solution[t1][r]) - 1 == t2)
                    continue;
                int rr = r;
                std::vector<int> roundsToSwap;
                roundsToSwap.push_back(r);
                while (true)
                {
                    int t = m_solution[t2][rr];
                    //In which round plays tt1 against t
                    if (t < 0)
                        rr = inRound[-(t + 1)][t1];
                    else
                        rr = inRound[t1][t - 1];
                    if (rr < 0)
                    {
                        std::cout << t1 << ", " << t2 << ", " << r << std::endl;
                        Common::printMatrix(m_solution);
                        std::cout << "jlsdafjlaksdf" << std::endl;
                    }
                    if (std::find(roundsToSwap.begin(), roundsToSwap.end(), rr) != roundsToSwap.end())
                        break;
                    roundsToSwap.push_back(rr);
                }

                if ((int) roundsToSwap.size() == m_rounds)
                    continue;

                //Swap teams
                for (auto r0 : roundsToSwap)
                {
                    int tt1 = std::abs(m_solution[t1][r0]) - 1;
                    int tt2 = std::abs(m_solution[t2][r0]) - 1;
                    std::swap(m_solution[t1][r0], m_solution[t2][r0]);
                    std::swap(m_solution[tt1][r0], m_solution[tt2][r0]);
                    Common::swapSigns(m_solution[tt1][r0], m_solution[tt2][r0]);
                }

                //Check constraints
                for (auto r0 : roundsToSwap)
                {
                    int tt1 = std::abs(m_solution[t1][r0]) - 1;
                    int tt2 = std::abs(m_solution[t2][r0]) - 1;
                    if (Common::atMost(t1, r0, m_solution) > 3 || Common::atMost(t2, r0, m_solution) > 3 ||
                            Common::atMost(tt1, r0, m_solution) > 3 || Common::atMost(tt2, r0, m_solution) > 3 ||
                            !Common::noRepeat(t1, r0, m_solution) || !Common::noRepeat(t2, r0, m_solution) ||
                            !Common::noRepeat(tt1, r0, m_solution) || !Common::noRepeat(tt2, r0, m_solution))
                    {
                        next = true;
                        break;
                    }
                }

                //Check if neighbor is better
                if (!next)
                {
                    int distTmp = Common::eval(m_solution, m_distance);
                    if (distTmp < distance)
                    {
                        std::cout << "imporved" << std::endl;
                        std::cout << t1 << ", " << t2 << ", " << r << std::endl;
                        Common::printMatrix(m_solution);
                        std::cout << "spt--- " << std::endl;
                        improved = true;
                        distance = distTmp;
                        solution = m_solution;
                    }
                }

                //Undo move
                for (auto r0 : roundsToSwap)
                {
                    int tt1 = std::abs(m_solution[t1][r0]) - 1;
                    int tt2 = std::abs(m_solution[t2][r0]) - 1;
                    std::swap(m_solution[t1][r0], m_solution[t2][r0]);
                    std::swap(m_solution[tt1][r0], m_solution[tt2][r0]);
                    Common::swapSigns(m_solution[tt1][r0], m_solution[tt2][r0]);
                }
                if (improved && !m_findBestImprovement)
                    return true;
            }
        }
    }
    return improved;
}
