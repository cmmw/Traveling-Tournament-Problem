/*
 * HillClimber.h
 *
 *  Created on: Mar 13, 2017
 *      Author: christian
 */

#ifndef SRC_LOCALSEARCH_HILLCLIMBER_H_
#define SRC_LOCALSEARCH_HILLCLIMBER_H_

#include "../Common.h"

class HillClimber
{
public:
    HillClimber(const mat2i& solution, const mat2i& distance);
    virtual ~HillClimber();

    mat2i solve();

private:
    int m_teams;
    int m_rounds;
    mat2i m_solution;
    mat2i m_distance;

    //Neighborhoods: Input/Output parameters solution and distance may be modified
    void swapHomes(mat2i& solution, int& distance);

};

#endif /* SRC_LOCALSEARCH_HILLCLIMBER_H_ */
