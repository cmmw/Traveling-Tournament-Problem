/*
 * LNS.h
 *
 *  Created on: 16.11.2016
 *      Author: Christian
 */

#ifndef LNS_H_
#define LNS_H_

#include "repair/IRepair.h"
#include "destroy/IDestroy.h"
#include "Common.h"

class LNS
{
public:
    LNS(const mat2i& distance);
    virtual ~LNS();

    mat2i solve(const mat2i& solution);

private:
    int m_upperBound;
    mat2i m_distance;
    std::vector<int> m_usedRepairMethods;
    std::vector<int> m_usedDestroyMethods;
    std::vector<int> m_repairMethodImproved;
    std::vector<int> m_destroyMethodImproved;
    std::vector<IRepair*> m_repairMethods;
    std::vector<IDestroy*> m_destroyMethods;
    mat2i destroy(const mat2i& solution, int method);
    mat2i repair(const mat2i& solution, int method);
    bool accept(const mat2i& newSol, const mat2i& oldSol);
    bool permuteTeams(mat2i& solution);
    void printStatistics();
};

#endif /* LNS_H_ */
