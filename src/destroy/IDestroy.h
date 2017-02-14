/*
 * IDestroy.h
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#ifndef SRC_DESTROY_IDESTROY_H_
#define SRC_DESTROY_IDESTROY_H_

#include "../Common.h"
#include "../IPSolver/IPSolver.h"

class IDestroy
{
public:
    IDestroy(const mat2i& distance, IPSolver& solver);
    virtual ~IDestroy();
    virtual mat2i destroy(const mat2i& solution, int size) = 0;

protected:
    int m_teams;
    int m_rounds;
    mat2i m_distance;
    IPSolver& m_solver;
};

#endif /* SRC_DESTROY_IDESTROY_H_ */
