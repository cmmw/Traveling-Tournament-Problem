/*
 * DestroyRow.h
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#ifndef SRC_DESTROY_DESTROYROWS_H_
#define SRC_DESTROY_DESTROYROWS_H_

#include "IDestroy.h"

class DestroyRows: public IDestroy
{
public:
    DestroyRows(const mat2i& distance, IPSolver& solver);
    virtual ~DestroyRows();
    mat2i destroy(const mat2i& solution, int size);

private:
    std::vector<int> teamDistance(const mat2i& solution);
};

#endif /* SRC_DESTROY_DESTROYROWS_H_ */
