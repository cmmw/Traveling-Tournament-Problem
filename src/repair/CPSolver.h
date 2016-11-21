/*
 * CPSolver.h
 *
 *  Created on: 23.10.2016
 *      Author: Christian
 */

#ifndef CPSOLVER_H_
#define CPSOLVER_H_

#include "IRepair.h"

class CPSolver: public IRepair
{
public:
    bool m_useMRV;
    bool m_useLCV;

    CPSolver(const mat2i& distance);
    virtual ~CPSolver();

    mat2i solve(const mat2i& solution, bool optimal = false);

private:
    int m_nodes;
    bool m_optimal;
    int m_bestValue;
    mat2i m_bestSolution;

    bool backTrack(mat2i& solution);
    bool getUnassignedVar(int& team, int& round, const mat2i& solution);
    bool getNextUnassignedVar(int& team, int& round, const mat2i& solution);
    bool getMRV(int& team, int& round, const mat2i& solution);

    int ruledOutValues(int team, int round, int value, const mat2i& solution);
    int ruledOutHorizontal(int team, int round, int value, const mat2i& solution);
    int ruledOutVertical(int team, int round, int value, const mat2i& solution);
    int ruledOutOpponent(int team, int round, int value, const mat2i& solution);
    int ruledOutNoRepeat(int team, int round, int value, const mat2i& solution);
    int ruledOutAtMost(int team, int round, int value, const mat2i& solution);
};

#endif /* CPSOLVER_H_ */
