/*
 * DestroyVenues.h
 *
 *  Created on: Feb 14, 2017
 *      Author: christian
 */

#ifndef SRC_DESTROY_DESTROYVENUES_H_
#define SRC_DESTROY_DESTROYVENUES_H_

#include "IDestroy.h"

class DestroyVenues: public IDestroy
{
public:
    DestroyVenues(const mat2i& distance, IPSolver& solver);
    virtual ~DestroyVenues();
    mat2i destroy(const mat2i& solution, int size);
};

#endif /* SRC_DESTROY_DESTROYVENUES_H_ */
