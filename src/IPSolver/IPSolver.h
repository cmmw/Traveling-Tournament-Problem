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

    typedef std::vector<std::vector<IloIntVar>> IloIntVar2;
    typedef std::vector<IloIntVar2> IloIntVar3;

    int m_teams;
    int m_rounds;
    mat2i m_distance;
    IloEnv m_env;
    IloModel m_model;
    IloCplex m_cplex;
    IloExpr m_objective;
    IloConstraintArray m_constraintArray;

    //x_ijk means team i plays away against team j at round k
    IloIntVar3 m_x;
    //Helper variable z_ijk (team i is at venue j at round k)
    IloIntVar3 m_z;
    //Helper variable y_tij, team t travels from i to j anywhere in the schedule (needed for objective function)
    IloIntVar3 m_y;

    void initModel();
    void setParams();
    mat2i convert();
    void populatePartialSolution(const mat2i& sol);

};

#endif /* SRC_IPSOLVER_IPSOLVER_H_ */
