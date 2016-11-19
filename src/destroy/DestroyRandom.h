/*
 * DestroyRandom.h
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#ifndef DESTROY_DESTROYRANDOM_H_
#define DESTROY_DESTROYRANDOM_H_

#include "IDestroy.h"

class DestroyRandom: public IDestroy
{
public:
    DestroyRandom(const mat2i& distance);
    virtual ~DestroyRandom();

    mat2i destroy(const mat2i& solution);
};

#endif /* DESTROY_DESTROYRANDOM_H_ */
