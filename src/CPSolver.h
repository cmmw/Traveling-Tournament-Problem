/*
 * CPSolver.h
 *
 *  Created on: 05.10.2016
 *      Author: Christian
 */

#ifndef CPSOLVER_H_
#define CPSOLVER_H_

#include "Algorithm.h"
#include <iostream>
#include <vector>

//Passed solution needs to be a consistent partial solution or empty

class CPSolver: public Algorithm
{
public:
    CPSolver(const mat2i& distances);
    virtual ~CPSolver();
    virtual bool solve(mat2i& solution, bool optimal = false);

private:
    int m_bestCosts;
    bool m_solutionFound;
    bool m_searchOptimum;
    bool backTrack(int team, int round, mat2i& solution, mat2i& solutionOut);
};

#endif /* CPSOLVER_H_ */
