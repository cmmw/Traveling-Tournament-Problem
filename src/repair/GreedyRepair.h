/*
 * GreedyRepair.h
 *
 *  Created on: 22.11.2016
 *      Author: Christian
 */

#ifndef REPAIR_GREEDYREPAIR_H_
#define REPAIR_GREEDYREPAIR_H_

#include "IBacktrack.h"
#include "../Common.h"

class GreedyRepair: public IBacktrack
{
public:
    GreedyRepair(const mat2i& distance);
    virtual ~GreedyRepair();

private:
    bool getNextVariable(int& team, int& round, const mat2i& solution);
    std::vector<int> valueOrderHeuristic(const mat2i& solution, int team, int round);
};

#endif /* REPAIR_GREEDYREPAIR_H_ */
