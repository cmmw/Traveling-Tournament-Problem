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
        m_teams(solution.size()), m_rounds(m_teams * 2 - 2), m_findBestImprovement(false), m_solution(solution), m_distance(distance), m_inRound(m_teams, std::vector<int>(m_teams, -1))
{
    m_neighborhoods.push_back(&HillClimber::swapHomes);
    m_neighborhoods.push_back(&HillClimber::swapRounds);
    m_neighborhoods.push_back(&HillClimber::swapTeams);
    m_neighborhoods.push_back(&HillClimber::swapPartialRounds);
    m_neighborhoods.push_back(&HillClimber::swapPartialTeams);
    calcInRound();
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
            run = (this->*f)(solution, distance) || run;
            if (!m_findBestImprovement && run)
                break;
        }
        std::cout << distance << std::endl;
        if (run)
        {
            m_solution = solution;
            calcInRound();
        }
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
            int r2 = (c1 < 0) ? m_inRound[t][-(c1 + 1)] : m_inRound[c1 - 1][t];

            //Move
            int& cc1 = m_solution[t][r2];
            int& cc2 = m_solution[otherTeam][r2];
            c1 = -c1;
            c2 = -c2;
            cc1 = -cc1;
            cc2 = -cc2;

            //Check solution
            std::vector<std::pair<int, int>> cells;
            cells.push_back(std::make_pair(t, r));
            cells.push_back(std::make_pair(t, r2));
            cells.push_back(std::make_pair(otherTeam, r));
            cells.push_back(std::make_pair(otherTeam, r2));

            if (checkSolution(solution, distance, cells))
                improved = true;

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
            //Swap teams
            std::vector<std::pair<int, int>> cells;
            for (int t = 0; t < m_teams; t++)
            {
                int& c1 = m_solution[t][r1];
                int& c2 = m_solution[t][r2];
                std::swap(c1, c2);
                cells.push_back(std::make_pair(t, r1));
                cells.push_back(std::make_pair(t, r2));
            }

            if (checkSolution(solution, distance, cells))
                improved = true;

            //Undo move
            for (int t = 0; t < m_teams; t++)
                std::swap(m_solution[t][r1], m_solution[t][r2]);

            if (improved && !m_findBestImprovement)
                return true;

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
            //Swap teams
            std::vector<std::pair<int, int>> cells;
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
                cells.push_back(std::make_pair(t1, r));
                cells.push_back(std::make_pair(t2, r));
                cells.push_back(std::make_pair(tt1, r));
                cells.push_back(std::make_pair(tt2, r));
            }

            //Check if swaps caused violations of the constraints
            if (checkSolution(solution, distance, cells))
                improved = true;

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
                std::vector<std::pair<int, int>> cells;
                for (int t0 : teamsToSwap)
                {
                    std::swap(m_solution[t0][r1], m_solution[t0][r2]);
                    cells.push_back(std::make_pair(t0, r1));
                    cells.push_back(std::make_pair(t0, r2));
                }

                if (checkSolution(solution, distance, cells))
                    improved = true;

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
    bool improved = false;
    for (int t1 = 0; t1 < m_teams - 1; t1++)
    {
        for (int t2 = t1 + 1; t2 < m_teams; t2++)
        {
            for (int r = 0; r < m_rounds; r++)
            {
                //TODO skip all rounds that were already considered after each loop
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
                        rr = m_inRound[-(t + 1)][t1];
                    else
                        rr = m_inRound[t1][t - 1];
                    if (rr < 0)
                    {
                        std::cout << t1 << ", " << t2 << ", " << r << std::endl;
                    }
                    if (std::find(roundsToSwap.begin(), roundsToSwap.end(), rr) != roundsToSwap.end())
                        break;
                    roundsToSwap.push_back(rr);
                }

                if ((int) roundsToSwap.size() == m_rounds)
                    continue;

                //Swap teams
                std::vector<std::pair<int, int>> cells;
                for (auto r0 : roundsToSwap)
                {
                    int tt1 = std::abs(m_solution[t1][r0]) - 1;
                    int tt2 = std::abs(m_solution[t2][r0]) - 1;
                    std::swap(m_solution[t1][r0], m_solution[t2][r0]);
                    std::swap(m_solution[tt1][r0], m_solution[tt2][r0]);
                    Common::swapSigns(m_solution[tt1][r0], m_solution[tt2][r0]);
                    cells.push_back(std::make_pair(t1, r0));
                    cells.push_back(std::make_pair(t2, r0));
                    cells.push_back(std::make_pair(tt1, r0));
                    cells.push_back(std::make_pair(tt2, r0));
                }

                if (checkSolution(solution, distance, cells))
                    improved = true;

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

bool HillClimber::checkSolution(mat2i& solution, int& distance, const std::vector<std::pair<int, int>>& cells)
{
    //Check constraints
    for (std::pair<int, int> p : cells)
    {
        int t = p.first;
        int r = p.second;
        if (!Common::noRepeat(t, r, m_solution) || Common::atMost(t, r, m_solution) > 3)
            return false;
    }

    //Check if solution is better
    int distTmp = Common::eval(m_solution, m_distance);
    if (distTmp < distance)
    {
        distance = distTmp;
        solution = m_solution;
        return true;
    }
    return false;
}

void HillClimber::calcInRound()
{
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int o = m_solution[t][r];
            if (o > 0)
                m_inRound[t][o - 1] = r;
        }
    }
}
