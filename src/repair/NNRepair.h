/*
 * NNRepair.h
 *
 *  Created on: 22.11.2016
 *      Author: Christian
 */

#ifndef REPAIR_NNREPAIR_H_
#define REPAIR_NNREPAIR_H_

#include "IRepair.h"
#include "../Common.h"

class NNRepair: public IRepair
{
public:
    NNRepair(const mat2i& distance);
    virtual ~NNRepair();

private:
    void solveImpl(const mat2i& solution);
    bool backTrack(mat2i& solution, int lds);
    bool getNextVariable(int& team, int& round, const mat2i& solution);
    int getDistance(int team, int t1, int t2);
};

#endif /* REPAIR_NNREPAIR_H_ */
