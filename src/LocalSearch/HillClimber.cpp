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

    m_baseSolution = m_solution;
    m_incumbentSolution = m_solution;
    m_incumbentDistance = Common::eval(m_incumbentSolution, m_distance);
    m_baseDistance = m_incumbentDistance;
    calcInRound();
}

HillClimber::~HillClimber()
{
}

mat2i HillClimber::solve()
{
    bool run = true;
    while (run)
    {
        run = false;
        //For all neighborhoods
        for (swapFunc f : m_neighborhoods)
        {
            run = (this->*f)() || run;
            if (!m_findBestImprovement && run)
                break;
        }
        std::cout << m_incumbentDistance << std::endl;
        if (run)
        {
            m_solution = m_incumbentSolution;
            m_baseSolution = m_incumbentSolution;
            calcInRound();
        }
    }
    return m_solution;
}

bool HillClimber::swapHomes()
{
    bool improved = false;
    for (int t1 = 0; t1 < m_teams - 1; t1++)
    {
        for (int t2 = t1 + 1; t2 < m_teams; t2++)
        {
            //Move
            std::vector<std::pair<int, int>> cells = moveHomes(t1, t2);

            //Check solution
            if (checkSolution(cells))
                improved = true;

            //Undo move
            undoMoves(cells);

            if (improved && !m_findBestImprovement)
                return true;
        }
    }
    return improved;
}

HillClimber::entries HillClimber::moveHomes(int t1, int t2)
{
    int r1 = m_inRound[t1][t2];
    int r2 = m_inRound[t2][t1];
    int &c1 = m_solution[t1][r1];
    int &cc1 = m_solution[t1][r2];
    int &c2 = m_solution[t2][r1];
    int &cc2 = m_solution[t2][r2];

    //Move
    std::swap(c1, cc1);
    std::swap(c2, cc2);

    //Return changed cells
    entries cells;
    cells.push_back(std::make_pair(t1, r1));
    cells.push_back(std::make_pair(t1, r2));
    cells.push_back(std::make_pair(t2, r1));
    cells.push_back(std::make_pair(t2, r2));

    return cells;
}

bool HillClimber::swapRounds()
{
    bool improved = false;
    for (int r1 = 0; r1 < m_rounds - 1; r1++)
    {
        for (int r2 = r1 + 1; r2 < m_rounds; r2++)
        {
            //Move
            entries cells = moveRounds(r1, r2);

            if (checkSolution(cells))
                improved = true;

            //Undo move
            undoMoves(cells);

            if (improved && !m_findBestImprovement)
                return true;
        }
    }
    return improved;
}

HillClimber::entries HillClimber::moveRounds(int r1, int r2)
{
    entries cells;
    for (int t = 0; t < m_teams; t++)
    {
        int& c1 = m_solution[t][r1];
        int& c2 = m_solution[t][r2];
        std::swap(c1, c2);
        cells.push_back(std::make_pair(t, r1));
        cells.push_back(std::make_pair(t, r2));
    }
    return cells;
}

bool HillClimber::swapTeams()
{
    bool improved = false;
    for (int t1 = 0; t1 < m_teams - 1; t1++)
    {
        for (int t2 = t1 + 1; t2 < m_teams; t2++)
        {
            //Move
            entries cells = moveTeams(t1, t2);

            //Check if swaps caused violations of the constraints
            if (checkSolution(cells))
                improved = true;

            //Undo move
            undoMoves(cells);

            if (improved && !m_findBestImprovement)
                return true;
        }
    }
    return improved;
}

HillClimber::entries HillClimber::moveTeams(int t1, int t2)
{
    entries cells;
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
    return cells;
}

bool HillClimber::swapPartialRounds()
{
    bool improved = false;
    for (int r1 = 0; r1 < m_rounds - 1; r1++)
    {
        for (int r2 = r1 + 1; r2 < m_rounds; r2++)
        {
            for (int t = 0; t < m_teams; t++)
            {
                //TODO: don't test teams in teamsToSwap again (use a list instead of going though all teams and remove the ones in teamsToSwap)
                entries cells = movePartialRounds(r1, r2, t);

                if (cells.empty())
                    continue;

                if (checkSolution(cells))
                    improved = true;

                //Undo move
                undoMoves(cells);

                if (improved && !m_findBestImprovement)
                    return true;
            }
        }
    }
    return improved;
}

HillClimber::entries HillClimber::movePartialRounds(int r1, int r2, int t)
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

    //If all teams have to be swapped, skip (already done in swapRounds)
    if ((int) teamsToSwap.size() == m_teams)
        return entries();

    //Swap teams
    entries cells;
    for (int t0 : teamsToSwap)
    {
        std::swap(m_solution[t0][r1], m_solution[t0][r2]);
        cells.push_back(std::make_pair(t0, r1));
        cells.push_back(std::make_pair(t0, r2));
    }
    return cells;
}

bool HillClimber::swapPartialTeams()
{
    bool improved = false;
    for (int t1 = 0; t1 < m_teams - 1; t1++)
    {
        for (int t2 = t1 + 1; t2 < m_teams; t2++)
        {
            for (int r = 0; r < m_rounds; r++)
            {
                //TODO skip all rounds that were already considered after each loop
                entries cells = movePartialTeams(t1, t2, r);

                if (cells.empty())
                    continue;

                if (checkSolution(cells))
                    improved = true;

                //Undo move
                undoMoves(cells);

                if (improved && !m_findBestImprovement)
                    return true;
            }
        }
    }
    return improved;
}

HillClimber::entries HillClimber::movePartialTeams(int t1, int t2, int r)
{
    //If t1 plays against t2
    if (std::abs(m_solution[t1][r]) - 1 == t2)
        return entries();

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
        return entries();

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
    return cells;
}

bool HillClimber::checkSolution(const std::vector<std::pair<int, int>>& cells)
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
    if (distTmp < m_incumbentDistance)
    {
        m_incumbentDistance = distTmp;
        m_incumbentSolution = m_solution;
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
            int o = m_baseSolution[t][r];
            if (o > 0)
                m_inRound[t][o - 1] = r;
        }
    }
}

void HillClimber::undoMoves(const entries& cells)
{
    for (const auto& c : cells)
    {
        m_solution[c.first][c.second] = m_baseSolution[c.first][c.second];
    }
}
