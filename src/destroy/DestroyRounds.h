/*
 * DestroyRounds.h
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#ifndef DESTROY_DESTROYROUNDS_H_
#define DESTROY_DESTROYROUNDS_H_

#include "IDestroy.h"

class DestroyRounds: public IDestroy
{
public:
    DestroyRounds(const mat2i& distance);
    virtual ~DestroyRounds();

    mat2i destroy(const mat2i& solution);
};

#endif /* DESTROY_DESTROYROUNDS_H_ */
