/*
 * LNS.h
 *
 *  Created on: 16.11.2016
 *      Author: Christian
 */

#ifndef LNS_H_
#define LNS_H_

#include "destroy/IDestroy.h"
#include "Common.h"

class LNS
{
public:
    LNS(const mat2i& distance);
    virtual ~LNS();

    mat2i solve(const mat2i& solution);

private:
    mat2i m_distance;
    std::vector<int> m_usedMethods;
    std::vector<int> m_methodImproved;
    std::vector<IDestroy*> m_destroyMethods;
    mat2i destroy(const mat2i& solution, int method);
    mat2i repair(const mat2i& solution);
    bool accept(const mat2i& newSol, const mat2i& oldSol);
};

#endif /* LNS_H_ */
