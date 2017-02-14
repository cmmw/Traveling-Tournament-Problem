/*
 * DestroyRandom.h
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#ifndef SRC_DESTROY_DESTROYRANDOM_H_
#define SRC_DESTROY_DESTROYRANDOM_H_

#include "IDestroy.h"

class DestroyRandom: public IDestroy
{
public:
    DestroyRandom(const mat2i& distance, IPSolver& solver);
    virtual ~DestroyRandom();
    mat2i destroy(const mat2i& solution, int size);
};

#endif /* SRC_DESTROY_DESTROYRANDOM_H_ */
