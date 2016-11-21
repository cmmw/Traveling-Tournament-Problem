/*
 * DFSStar.cpp
 *
 *  Created on: 21.11.2016
 *      Author: Christian
 */

#include "DFSStar.h"

DFSStar::DFSStar(const mat2i& distance) :
        IRepair(distance), m_nodes(0)
{
}

DFSStar::~DFSStar()
{
}

mat2i DFSStar::solve(const mat2i& solution, bool optimal)
{
    mat2i sol = solution;
    init(sol);
    m_nodes = 0;
    backTrack(sol, 0);
    return sol;
}

bool DFSStar::backTrack(mat2i& solution, int i)
{

}
