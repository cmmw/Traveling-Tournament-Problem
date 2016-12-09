/*
 * LNS.cpp
 *
 *  Created on: 16.11.2016
 *      Author: Christian
 */

#include "LNS.h"
#include "destroy/DestroyHomes.h"
#include "destroy/DestroyRandom.h"
#include "destroy/DestroyRounds.h"
#include "destroy/DestroyTeams.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <typeindex>
#include "repair/CSPRepair.h"
#include "repair/GreedyRepair.h"
#include "repair/BeamSearch.h"
#include "repair/GreedyRepair.h"

LNS::LNS(const mat2i& distance) :
        m_upperBound(std::numeric_limits<decltype(m_upperBound)>::max()), m_distance(distance)
{
    m_destroyMethods.push_back(new DestroyRounds(distance));
    m_destroyMethods.push_back(new DestroyTeams(distance));
    m_destroyMethods.push_back(new DestroyHomes(distance));
    m_destroyMethods.push_back(new DestroyRandom(distance));

    m_repairMethods.push_back(new CSPRepair(distance));
    m_repairMethods.push_back(new GreedyRepair(distance));
    m_repairMethods.push_back(new BeamSearch(distance));

    m_usedRepairMethods.resize(m_repairMethods.size());
    m_usedDestroyMethods.resize(m_destroyMethods.size());
    m_repairMethodImproved.resize(m_repairMethods.size());
    m_destroyMethodImproved.resize(m_destroyMethods.size());
}

LNS::~LNS()
{
    for (IDestroy* method : m_destroyMethods)
        delete method;
}

mat2i LNS::solve(const mat2i& solution)
{
    m_upperBound = Common::eval(solution, m_distance);
    Common::printMatrix(solution);
    std::cout << m_upperBound << std::endl;
    mat2i bestSol = solution;
    mat2i curSol = solution;
    bool done = false;
    int i = 0;
    while (!done)
    {
        int destroyMethod = rand() % m_destroyMethods.size();
        int repairMethod = rand() % m_repairMethods.size();
        mat2i tempSol = repair(destroy(curSol, destroyMethod), repairMethod);

        if (!tempSol.empty())
        {
//            permuteTeams(tempSol);
            if (accept(tempSol, curSol))
            {
                curSol = tempSol;
            }
            if (Common::eval(tempSol, m_distance) < m_upperBound)
            {
                bestSol = tempSol;
                m_upperBound = Common::eval(tempSol, m_distance);
                std::cout << Common::eval(bestSol, m_distance) << std::endl;
                m_repairMethodImproved[repairMethod]++;
                m_destroyMethodImproved[destroyMethod]++;
                i = 0;
            }
        }

        if (i == 100)
            done = true;
        i++;
    }
    printStatistics();
    return bestSol;
}

mat2i LNS::destroy(const mat2i& solution, int method)
{
    m_usedDestroyMethods[method]++;
    std::cout << typeid(*m_destroyMethods[method]).name() << std::endl;
    return m_destroyMethods[method]->destroy(solution);
}

mat2i LNS::repair(const mat2i& solution, int method)
{
    m_usedRepairMethods[method]++;
//    std::cout << typeid(*m_repairMethods[method]).name() << std::endl;
    return m_repairMethods[method]->solve(solution, m_upperBound * 0.7);
}

bool LNS::accept(const mat2i& newSol, const mat2i& oldSol)
{
    int newVal = Common::eval(newSol, m_distance);
    int oldVal = Common::eval(oldSol, m_distance);
    if (newVal < oldVal)
        return true;
    return false;
}

bool LNS::permuteTeams(mat2i& solution)
{
    mat2i map = Common::calcTravelMap(solution);
    bool improved = false;
    int teams = solution.size();
    int oldDistance = Common::eval(solution, m_distance);
    int swapT1 = 0;
    int swapT2 = 0;
    int bestImprove = 0;
    //Improve solution by swapping 2 teams, find best pair
    for (int t1 = 0; t1 < teams; t1++)
    {
        for (int t2 = t1 + 1; t2 < teams; t2++)
        {
            //calculate improvement with teams t1 and t2 swapped
            int o1 = 0;
            int n1 = 0;
            int o2 = 0;
            int n2 = 0;
            for (int j = 0; j < teams; j++)
            {
                if (j == t2 || j == t1)
                    continue;

                o1 += (map[t1][j] * m_distance[t1][j]);
                o2 += (map[t2][j] * m_distance[t2][j]);

                n1 += (map[t1][j] * m_distance[t2][j]);
                n2 += (map[t2][j] * m_distance[t1][j]);
            }

            int improvement = (n1 + n2 - o1 - o2);

            //If change in distance is negative
            if (improvement < bestImprove)
            {
                bestImprove = improvement;
                swapT1 = t1;
                swapT2 = t2;
                improved = true;
            }
        }
    }
    if (improved)
    {
        for (auto& r : solution)
        {
            for (auto& t : r)
            {
                if (std::abs(t) == swapT1 + 1)
                    t = (swapT2 + 1) * (t / std::abs(t));
                else if (std::abs(t) == swapT2 + 1)
                    t = (swapT1 + 1) * (t / std::abs(t));
            }
        }
        std::swap(solution[swapT1], solution[swapT2]);
        std::cout << "improved team swaps: " << oldDistance << " -> " << Common::eval(solution, m_distance) << std::endl;
    }
    return improved;
}

void LNS::printStatistics()
{
    std::cout << "------------USAGE------------" << std::endl;
    std::cout << "Repair-methods used:" << std::endl;
    for (unsigned int i = 0; i < m_usedRepairMethods.size(); i++)
    {
        std::cout << typeid(*m_repairMethods[i]).name() << ": " << m_usedRepairMethods[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << "Destroy-methods used:" << std::endl;
    for (unsigned int i = 0; i < m_usedDestroyMethods.size(); i++)
    {
        std::cout << typeid(*m_destroyMethods[i]).name() << ": " << m_usedDestroyMethods[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << "------------IMPROVEMENTS------------" << std::endl;
    std::cout << "Repair-methods improved solution:" << std::endl;
    for (unsigned int i = 0; i < m_repairMethodImproved.size(); i++)
    {
        std::cout << typeid(*m_repairMethods[i]).name() << ": " << m_repairMethodImproved[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << "Destroy-methods improved solution:" << std::endl;
    for (unsigned int i = 0; i < m_destroyMethodImproved.size(); i++)
    {
        std::cout << typeid(*m_destroyMethods[i]).name() << ": " << m_destroyMethodImproved[i] << ", ";
    }
    std::cout << std::endl;
}

