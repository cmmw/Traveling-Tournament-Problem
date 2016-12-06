/*
 * EdgeRepair.h
 *
 *  Created on: 30.11.2016
 *      Author: Christian
 */

#ifndef REPAIR_EDGEREPAIR_H_
#define REPAIR_EDGEREPAIR_H_

#include "IRepair.h"

class EdgeRepair: public IRepair
{
public:
    EdgeRepair(const mat2i& distance);
    virtual ~EdgeRepair();

private:
    mat3i m_patDomain;
    mat2i solveImpl(const mat2i& solution);

    mat2i initPattern(const mat2i& solution);
    bool pattern(mat2i& pat);
    bool patForwardCheck(int team, int round, const mat2i& solution, std::vector<DomainBackupEntry>& domainBackup);
};

#endif /* REPAIR_EDGEREPAIR_H_ */
