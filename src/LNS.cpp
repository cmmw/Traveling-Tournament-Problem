/*
 * LNS.cpp
 *
 *  Created on: 16.11.2016
 *      Author: Christian
 */

#include "LNS.h"
#include "IPSolver/IPSolver.h"
#include "destroy/DestroyColumns.h"
#include "destroy/DestroyRows.h"
#include "destroy/DestroyTeams.h"
#include "destroy/DestroyRandom.h"
#include "destroy/DestroyHomeAwayPattern.h"
#include "destroy/DestroyVenues.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <typeindex>

LNS::LNS(const mat2i& distance) :
        m_teams(distance.size()), m_rounds(m_teams * 2 - 2), m_distance(distance), m_destroyLb(20), m_destroyUb(55), m_solver(distance)
{
    m_destroyMethods.push_back(new DestroyColumns(m_distance, m_solver));
    m_destroyMethods.push_back(new DestroyRows(m_distance, m_solver));
    m_destroyMethods.push_back(new DestroyTeams(m_distance, m_solver));
    m_destroyMethods.push_back(new DestroyRandom(m_distance, m_solver));
    m_destroyMethods.push_back(new DestroyHomeAwayPattern(m_distance, m_solver));
    m_destroyMethods.push_back(new DestroyVenues(m_distance, m_solver));

    m_destroyMethodsUsed.resize(m_destroyMethods.size());
    m_destroyMethodsImproved.resize(m_destroyMethods.size());
}

LNS::~LNS()
{
}

mat2i LNS::solve(const mat2i& solution)
{
    mat2i currentSol = solution;
    mat2i bestSol = currentSol;
    int bestVal = Common::eval(bestSol, m_distance);
    int currentVal = bestVal;
    int destroySize = m_destroyLb;
    int lastIncrement = 0;
    for (int i = 0; i < 235; i++)
    {
        int method = std::rand() % m_destroyMethods.size();
        destroy(currentSol, method, destroySize);
        mat2i optSol = m_solver.solve();
        int optVal = Common::eval(optSol, m_distance);

        m_destroyMethodsUsed[method]++;

        if (optVal < bestVal)
        {
            m_destroyMethodsImproved[method]++;
            bestSol = optSol;
            bestVal = optVal;
            std::cout << bestVal << std::endl;
            i = 0;
            destroySize = m_destroyLb;
            lastIncrement = 0;
        } else
        {
            lastIncrement++;
            if (lastIncrement >= 20)
            {
                destroySize = std::min(destroySize + 5, m_destroyUb);
                lastIncrement = 0;
            }
        }

        if (accept(optVal, currentVal))
        {
            currentSol = optSol;
            currentVal = optVal;
            Common::printMatrix(currentSol);
        }
    }
    printStatistics();
    return bestSol;
}

void LNS::destroy(const mat2i& solution, int method, int destroySize)
{
    std::cout << typeid(*m_destroyMethods[method]).name() << ", size: " << destroySize << "%" << std::endl;
    auto s = m_destroyMethods[method]->destroy(solution, destroySize);
//    Common::printMatrix(s);
}

bool LNS::accept(int optVal, int currentVal)
{
    return optVal < currentVal;
}

void LNS::printStatistics()
{
    std::cout << "------------USAGE------------" << std::endl;
    std::cout << "Destroy-methods used:" << std::endl;
    for (unsigned int i = 0; i < m_destroyMethodsUsed.size(); i++)
    {
        std::cout << typeid(*m_destroyMethods[i]).name() << ": " << m_destroyMethodsUsed[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << "------------IMPROVEMENTS------------" << std::endl;
    std::cout << "Destroy-methods improved solution:" << std::endl;
    for (unsigned int i = 0; i < m_destroyMethodsImproved.size(); i++)
    {
        std::cout << typeid(*m_destroyMethods[i]).name() << ": " << m_destroyMethodsImproved[i] << ", ";
    }
    std::cout << std::endl;
}
