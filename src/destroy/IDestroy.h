/*
 * IDestroy.h
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#ifndef DESTROY_IDESTROY_H_
#define DESTROY_IDESTROY_H_

#include "../Common.h"

class IDestroy
{
public:
    IDestroy(const mat2i& distance) :
            m_teams(distance.size()), m_rounds(m_teams * 2 - 2), m_distance(distance)
    {
    }
    virtual ~IDestroy() = default;

    virtual mat2i destroy(const mat2i& solution) = 0;

protected:
    int m_teams;
    int m_rounds;
    mat2i m_distance;
};

#endif /* DESTROY_IDESTROY_H_ */
