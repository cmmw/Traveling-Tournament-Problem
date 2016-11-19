/*
 * DestroyHomes.h
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#ifndef DESTROY_DESTROYHOMES_H_
#define DESTROY_DESTROYHOMES_H_

#include "IDestroy.h"

class DestroyHomes: public IDestroy
{
public:
    DestroyHomes(const mat2i& distance);
    virtual ~DestroyHomes();

    mat2i destroy(const mat2i& solution);

private:
    mat2i m_distance;
};

#endif /* DESTROY_DESTROYHOMES_H_ */
