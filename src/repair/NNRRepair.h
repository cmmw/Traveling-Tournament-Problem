/*
 * NNRRepair.h
 *
 *  Created on: 22.11.2016
 *      Author: Christian
 */

#ifndef REPAIR_NNRREPAIR_H_
#define REPAIR_NNRREPAIR_H_

#include "IBacktrack.h"
#include "../Common.h"

class NNRRepair: public IBacktrack
{
public:
    NNRRepair(const mat2i& distance);
    virtual ~NNRRepair();

private:
    bool getNextVariable(int& team, int& round, const mat2i& solution);
    std::vector<int> valueOrderHeuristic(const mat2i& solution, int team, int round);
};

#endif /* REPAIR_NNRREPAIR_H_ */
