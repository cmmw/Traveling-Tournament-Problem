/*
 * LNS.cpp
 *
 *  Created on: 16.11.2016
 *      Author: Christian
 */

#include "LNS.h"
#include "repair/CPSolver.h"
#include "destroy/DestroyHomes.h"
#include "destroy/DestroyRandom.h"
#include "destroy/DestroyRounds.h"
#include "destroy/DestroyTeams.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <typeindex>

LNS::LNS(const mat2i& distance) :
        m_distance(distance)
{
    m_destroyMethods.push_back(new DestroyRounds(distance));
    m_destroyMethods.push_back(new DestroyTeams(distance));
    m_destroyMethods.push_back(new DestroyHomes(distance));
    m_destroyMethods.push_back(new DestroyRandom(distance));

    m_methodImproved.resize(m_destroyMethods.size());
    m_usedMethods.resize(m_destroyMethods.size());
}

LNS::~LNS()
{
    for (IDestroy* method : m_destroyMethods)
    {
        delete method;
    }
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
        int method = rand() % m_destroyMethods.size();
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
    for (unsigned int i = 0; i < m_usedMethods.size(); i++)
    {
        std::cout << typeid(*m_destroyMethods[i]).name() << ": " << m_usedMethods[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << "Methods improved solution:" << std::endl;
    for (unsigned int i = 0; i < m_methodImproved.size(); i++)
    {
        std::cout << typeid(*m_destroyMethods[i]).name() << ": " << m_methodImproved[i] << ", ";
    }
    std::cout << std::endl;
    return bestSol;
}

mat2i LNS::destroy(const mat2i& solution, int method)
{
    m_usedMethods[method]++;
    std::cout << typeid(*m_destroyMethods[method]).name() << std::endl;
    return m_destroyMethods[method]->destroy(solution);
}

mat2i LNS::repair(const mat2i& solution)
{
    CPSolver solver(m_distance);
    return solver.solve(solution, true);
}

bool LNS::accept(const mat2i& newSol, const mat2i& oldSol)
{
    int newVal = Common::eval(newSol, m_distance);
    int oldVal = Common::eval(oldSol, m_distance);
    if (newVal < oldVal)
        return true;
    return false;
}