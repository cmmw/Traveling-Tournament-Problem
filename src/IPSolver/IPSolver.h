/*
 * IPSolver.h
 *
 *  Created on: Jan 2, 2017
 *      Author: christian
 */

#ifndef SRC_IPSOLVER_IPSOLVER_H_
#define SRC_IPSOLVER_IPSOLVER_H_

#include "../Common.h"

#include <vector>
#include <ilcplex/ilocplex.h>

class IPSolver
{
public:
    IPSolver(const mat2i& distance);
    virtual ~IPSolver();

    mat2i solve(const mat2i& solution);

private:

    int m_teams;
    int m_rounds;
    mat2i m_distance;

    typedef std::vector<std::vector<IloIntVar>> IloIntVar2;
    typedef std::vector<IloIntVar2> IloIntVar3;

    void setParams(IloCplex& cplex);
    mat2i convert(const IloIntVar3& x, const IloCplex& cplex);
    void populatePartialSolution(const mat2i& sol, const IloIntVar3& x, const IloIntVar3& z, IloModel& model);

};

#endif /* SRC_IPSOLVER_IPSOLVER_H_ */
