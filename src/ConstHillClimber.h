/*
 * ConstHeu.h
 *
 *  Created on: 16.10.2016
 *      Author: Christian
 */

#ifndef CONSTHILLCLIMBER_H_
#define CONSTHILLCLIMBER_H_

#include "Algorithm.h"

class ConstHillClimber: public Algorithm
{
public:
    ConstHillClimber(const mat2i& distances);
    virtual ~ConstHillClimber();

    bool solve(mat2i& solution, bool optimal = false);
};

#endif /* CONSTHILLCLIMBER_H_ */
