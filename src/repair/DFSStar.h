/*
 * DFSStar.h
 *
 *  Created on: 21.11.2016
 *      Author: Christian
 */

#ifndef REPAIR_DFSSTAR_H_
#define REPAIR_DFSSTAR_H_

#include "IRepair.h"

class DFSStar: public IRepair
{
public:
    DFSStar(const mat2i& distance);
    virtual ~DFSStar();

    mat2i solve(const mat2i& solution, bool optimal = false);

private:
    int m_nodes;

    bool backTrack(mat2i& solution, int i);

};

#endif /* REPAIR_DFSSTAR_H_ */
