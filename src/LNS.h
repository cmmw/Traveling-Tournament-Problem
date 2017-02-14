/*
 * LNS.h
 *
 *  Created on: 16.11.2016
 *      Author: Christian
 */

#ifndef LNS_H_
#define LNS_H_

#include "Common.h"
#include "destroy/IDestroy.h"
#include "IPSolver/IPSolver.h"

class LNS
{
public:
    LNS(const mat2i& distance);
    virtual ~LNS();

    mat2i solve(const mat2i& solution);

private:
    int m_teams;
    int m_rounds;
    mat2i m_distance;
    int m_destroyLb;
    int m_destroyUb;
    IPSolver m_solver;

    std::vector<IDestroy*> m_destroyMethods;
    std::vector<int> m_destroyMethodsUsed;
    std::vector<int> m_destroyMethodsImproved;

    void destroy(const mat2i& solution, int method, int destroySize);
    bool accept(int optVal, int currentVal);
    void printStatistics();
};

#endif /* LNS_H_ */
