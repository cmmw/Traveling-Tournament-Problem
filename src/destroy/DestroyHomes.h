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
    std::vector<int> homeDistance(const mat2i& solution);
};

#endif /* DESTROY_DESTROYHOMES_H_ */
