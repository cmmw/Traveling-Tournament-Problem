/*
 * DestroyVenues.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: christian
 */

#include "DestroyVenues.h"

DestroyVenues::DestroyVenues(const mat2i& distance, IPSolver& solver) :
        IDestroy(distance, solver)
{
}

DestroyVenues::~DestroyVenues()
{
}

mat2i DestroyVenues::destroy(const mat2i& solution, int size)
{
    m_solver.relaxVenues(solution);
    return solution;
}
