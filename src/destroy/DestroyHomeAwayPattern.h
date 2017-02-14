/*
 * DestroyHomeAwayPattern.h
 *
 *  Created on: Feb 14, 2017
 *      Author: christian
 */

#ifndef SRC_DESTROY_DESTROYHOMEAWAYPATTERN_H_
#define SRC_DESTROY_DESTROYHOMEAWAYPATTERN_H_

#include "IDestroy.h"
#include "../IPSolver/IPSolver.h"

class DestroyHomeAwayPattern: public IDestroy
{
public:
    DestroyHomeAwayPattern(const mat2i& distance, IPSolver& solver);
    virtual ~DestroyHomeAwayPattern();
    mat2i destroy(const mat2i& solution, int size);
};

#endif /* SRC_DESTROY_DESTROYHOMEAWAYPATTERN_H_ */
