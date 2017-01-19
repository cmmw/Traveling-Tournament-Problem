/*
 * IDestroy.h
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#ifndef SRC_DESTROY_IDESTROY_H_
#define SRC_DESTROY_IDESTROY_H_

#include "../Common.h"

class IDestroy
{
public:
    IDestroy(const mat2i& distance);
    virtual ~IDestroy();
    virtual mat2i destroy(const mat2i& solution, int size) = 0;

protected:
    int m_teams;
    int m_rounds;
    mat2i m_distance;
};

#endif /* SRC_DESTROY_IDESTROY_H_ */
