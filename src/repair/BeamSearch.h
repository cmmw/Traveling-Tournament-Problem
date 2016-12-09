/*
 * BeamSearch.h
 *
 *  Created on: 07.12.2016
 *      Author: Christian
 */

#ifndef REPAIR_BEAMSEARCH_H_
#define REPAIR_BEAMSEARCH_H_

#include "IRepair.h"

struct SolWithDom
{
    mat2i solution;
    mat3i domain;
    int distance;
    int trips;
};

class BeamSearch: public IRepair
{
public:
    BeamSearch(const mat2i& distance);
    virtual ~BeamSearch();

private:
    unsigned int m_width;

    virtual mat2i solveImpl(const mat2i& solution);
    bool beamSearch(mat2i& solution);
    bool getNextVariable(int& team, int& round, const mat2i& solution);
};

#endif /* REPAIR_BEAMSEARCH_H_ */
