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

private:
    void solve(const mat2i& solution);
    bool backTrack(mat2i& solution);
    bool chooseVariable(int& team, int& round, const mat2i& solution);
};

#endif /* REPAIR_DFSSTAR_H_ */
