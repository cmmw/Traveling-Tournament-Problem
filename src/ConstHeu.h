/*
 * ConstHeu.h
 *
 *  Created on: 16.10.2016
 *      Author: Christian
 */

#ifndef CONSTHEU_H_
#define CONSTHEU_H_

#include "Algorithm.h"

class ConstHeu: public Algorithm
{
public:
    ConstHeu(const mat2i& distances);
    virtual ~ConstHeu();

    bool solve(mat2i& solution, bool optimal = false);
};

#endif /* CONSTHEU_H_ */
