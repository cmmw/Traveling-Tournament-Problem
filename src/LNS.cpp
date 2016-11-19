/*
 * LNS.cpp
 *
 *  Created on: 16.11.2016
 *      Author: Christian
 */

#include "LNS.h"
#include "CPSolver.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>

LNS::LNS(const mat2i& distance) :
        m_distance(distance)
{
}

LNS::~LNS()
{
}

mat2i LNS::solve(const mat2i& solution)
{
    Common::printMatrix(solution);
    mat2i bestSol = solution;
    mat2i curSol = solution;
    bool done = false;
    int i = 0;
    while (!done)
    {
        int method = rand() % 4;
        mat2i tempSol = repair(destroy(curSol, method));

        if (accept(tempSol, curSol))
        {
            curSol = tempSol;
        }
        if (Common::eval(tempSol, m_distance) < Common::eval(bestSol, m_distance))
        {
            bestSol = tempSol;
            std::cout << Common::eval(bestSol, m_distance) << std::endl;
            m_methodImproved[method]++;
            i = 0;
        }

        if (i == 100)
            done = true;
        i++;
    }
    std::cout << "Methods used:" << std::endl;
    for (unsigned int i = 0; i < sizeof(m_usedMethods) / sizeof(m_usedMethods[0]); i++)
    {
        std::cout << i << ": " << m_usedMethods[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << "Methods improved solution:" << std::endl;
    for (unsigned int i = 0; i < sizeof(m_usedMethods) / sizeof(m_usedMethods[0]); i++)
    {
        std::cout << i << ": " << m_methodImproved[i] << ", ";
    }
    std::cout << std::endl;
    return bestSol;
}

mat2i LNS::destroy(const mat2i& solution, int method)
{
    mat2i destroyed = solution;
    int teams = solution.size();
    int rounds = solution[0].size();
    m_usedMethods[method]++;
    std::cout << method << std::endl;
    switch (method)
    {
    case 0:
        {
        //swap rounds: remove all entries in >= 2 rounds
        int r1 = rand() % rounds;
        int r2 = rand() % rounds;
        int r3 = rand() % rounds;
        int r4 = rand() % rounds;
        while (r2 == r1)
            r2 = rand() % rounds;

        while (r3 == r1 || r3 == r2)
            r3 = rand() % rounds;

        while (r4 == r1 || r4 == r2 || r4 == r3)
            r4 = rand() % rounds;

        for (int t = 0; t < teams; t++)
        {
            destroyed[t][r1] = 0;
            destroyed[t][r2] = 0;
            destroyed[t][r3] = 0;
            destroyed[t][r4] = 0;
        }
        break;
    }
    case 1:
        {
        //swap teams: remove all entries in row t1, t2 (n >= 2) expect the ones where they play against each other and in all other cells remove the entries containing t1 or t2
        int t1 = rand() % teams + 1;
        int t2 = rand() % teams + 1;
        while (t2 == t1)
            t2 = rand() % teams + 1;

        for (int t = 0; t < teams; t++)
        {
            for (int r = 0; r < rounds; r++)
            {
                int& entry = destroyed[t][r];
                int absEntry = std::abs(entry);
                if (t + 1 == t1 || t + 1 == t2)
                {
                    if (absEntry != t2 && absEntry != t1)
                    {
                        entry = 0;
                    }
                } else if (absEntry == t2 || absEntry == t1)
                {
                    entry = 0;
                }
            }
        }
        break;
    }
    case 2:
        {
        //swap homes: for >= 1 games remove the entries containing t1 or t2 in the whole schedule
        int t1 = rand() % teams + 1;
        int t2 = rand() % teams + 1;
        int t3 = rand() % teams + 1;
        int t4 = rand() % teams + 1;
        int t5 = rand() % teams + 1;
        while (t2 == t1)
            t2 = rand() % teams + 1;

        while (t3 == t1 || t3 == t2)
            t3 = rand() % teams + 1;

        while (t4 == t1 || t4 == t2 || t4 == t3)
            t4 = rand() % teams + 1;

        while (t5 == t1 || t5 == t2 || t5 == t3 || t5 == t4)
            t5 = rand() % teams + 1;

        for (int t = 0; t < teams; t++)
        {
            for (int r = 0; r < rounds; r++)
            {
                int absEntry = std::abs(destroyed[t][r]);
                if (absEntry == t1 || absEntry == t2 || absEntry == t3 || absEntry == t4 || absEntry == t5)
                    destroyed[t][r] = 0;
            }
        }
        break;
    }
    case 3:
        {
        //random
        int size = 50;
        std::vector<std::pair<int, int>> vars;
        for (int t = 0; t < teams; t++)
        {
            for (int r = 0; r < rounds; r++)
            {
                vars.push_back(std::make_pair(t, r));
            }
        }
        std::random_shuffle(vars.begin(), vars.end());

        for (int i = 0; i < size; i++)
        {
            int t = vars[i].first;
            int r = vars[i].second;
            if (destroyed[t][r] != 0)
            {
                destroyed[std::abs(destroyed[t][r]) - 1][r] = 0;
                destroyed[t][r] = 0;
            }
        }
        break;
    }
    }
    return destroyed;
}

mat2i LNS::repair(const mat2i& solution)
{
    mat2i repaired = solution;
    CPSolver solver(m_distance);
    solver.solve(repaired, true);
    return repaired;
}

bool LNS::accept(const mat2i& newSol, const mat2i& oldSol)
{
    int newVal = Common::eval(newSol, m_distance);
    int oldVal = Common::eval(oldSol, m_distance);
    if (newVal < oldVal)
        return true;
    return false;
}
