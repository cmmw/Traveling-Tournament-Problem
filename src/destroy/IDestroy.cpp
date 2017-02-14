/*
 * IDestroy.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#include "IDestroy.h"
#include <iostream>

IDestroy::IDestroy(const mat2i& distance, IPSolver& solver) :
        m_teams(distance.size()), m_rounds(2 * m_teams - 2), m_distance(distance), m_solver(solver)
{
}

IDestroy::~IDestroy()
{
}
