/*
 * CPSolver.h
 *
 *  Created on: 23.10.2016
 *      Author: Christian
 */

#ifndef CPSOLVER_H_
#define CPSOLVER_H_

#include "IRepair.h"

class CSPRepair: public IRepair
{
public:
    bool m_useMRV;
    bool m_useLCV;

    CSPRepair(const mat2i& distance);
    virtual ~CSPRepair() = default;

private:
    void solveImpl(const mat2i& solution);
    bool backTrack(mat2i& solution);

    bool getNextVariable(int& team, int& round, const mat2i& solution);
    std::vector<int> valueOrderHeuristic(const mat2i& solution, int team, int round);
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