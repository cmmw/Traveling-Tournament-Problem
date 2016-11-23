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
    bool getNextVariable(int& team, int& round, const mat2i& solution);
    std::vector<int> valueOrderHeuristic(const mat2i& solution, int team, int round);
    int getDistance(int team, int t1, int t2);
};

#endif /* REPAIR_NNREPAIR_H_ */
