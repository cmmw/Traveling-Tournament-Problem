/*
 * Factorization.h
 *
 *  Created on: 11.11.2016
 *      Author: Christian
 */

#ifndef FACTORIZATION_H_
#define FACTORIZATION_H_

#include "Common.h"

class Factorization
{
public:
    Factorization(const mat2i& distance);
    virtual ~Factorization();

    bool solve(mat2i &solution);

private:
    int m_teams;
    int m_rounds;
    mat2i m_distance;
};

#endif /* FACTORIZATION_H_ */
