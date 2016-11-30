/*
 * IBacktrack.h
 *
 *  Created on: 30.11.2016
 *      Author: Christian
 */

#ifndef REPAIR_IBACKTRACK_H_
#define REPAIR_IBACKTRACK_H_

#include "IRepair.h"

class IBacktrack: public IRepair
{
public:
    IBacktrack(const mat2i& distance);
    virtual ~IBacktrack();

private:
    mat2i solveImpl(const mat2i& solution);
    bool backTrack(mat2i& solution, int k, int variablesLeft);
    virtual bool getNextVariable(int& team, int& round, const mat2i& solution) = 0;
    virtual std::vector<int> valueOrderHeuristic(const mat2i& solution, int team, int round) = 0;
};

#endif /* REPAIR_IBACKTRACK_H_ */
