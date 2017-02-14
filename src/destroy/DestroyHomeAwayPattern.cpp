/*
 * DestroyHomeAwayPattern.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: christian
 */

#include "DestroyHomeAwayPattern.h"

DestroyHomeAwayPattern::DestroyHomeAwayPattern(const mat2i& distance, IPSolver& solver) :
        IDestroy(distance, solver)
{
}

DestroyHomeAwayPattern::~DestroyHomeAwayPattern()
{
}

mat2i DestroyHomeAwayPattern::destroy(const mat2i& solution, int size)
{
    m_solver.relaxHomeAwayPattern(solution);
    return solution;
}
