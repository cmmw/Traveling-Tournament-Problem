/*
 * NNRepair.h
 *
 *  Created on: 22.11.2016
 *      Author: Christian
 */

#ifndef REPAIR_NNLREPAIR_H_
#define REPAIR_NNLREPAIR_H_

#include "IBacktrack.h"
#include "../Common.h"

class NNLRepair: public IBacktrack
{
public:
    NNLRepair(const mat2i& distance);
    virtual ~NNLRepair();

private:
    bool getNextVariable(int& team, int& round, const mat2i& solution);
    std::vector<int> valueOrderHeuristic(const mat2i& solution, int team, int round);
};

#endif /* REPAIR_NNLREPAIR_H_ */
