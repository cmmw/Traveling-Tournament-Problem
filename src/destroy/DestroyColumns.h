/*
 * DestroyColumn.h
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#ifndef SRC_DESTROY_DESTROYCOLUMNS_H_
#define SRC_DESTROY_DESTROYCOLUMNS_H_

#include "IDestroy.h"

class DestroyColumns: public IDestroy
{
public:
    DestroyColumns(const mat2i& distance, IPSolver& solver);
    virtual ~DestroyColumns();
    mat2i destroy(const mat2i& solution, int size);

private:
    std::vector<int> roundDistances(const mat2i& solution);
};

#endif /* SRC_DESTROY_DESTROYCOLUMNS_H_ */
