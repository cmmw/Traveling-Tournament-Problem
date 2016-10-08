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
    CPSolver(const std::vector<std::vector<int>>& distances);
    virtual ~CPSolver();
    virtual bool solve(std::vector<std::vector<int>>& solution, bool optimal = false);

private:
    int m_bestCosts;
    bool m_solutionFound;
    bool m_searchOptimum;
    bool backTrack(int team, int round, std::vector<std::vector<int>>& solution, std::vector<std::vector<std::vector<int>>>& domain, std::vector<std::vector<int>>& solutionOut);
};

#endif /* CPSOLVER_H_ */
