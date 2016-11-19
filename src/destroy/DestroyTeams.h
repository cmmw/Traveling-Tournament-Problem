/*
 * DestroyTeams.h
 *
 *  Created on: 19.11.2016
 *      Author: Christian
 */

#ifndef DESTROY_DESTROYTEAMS_H_
#define DESTROY_DESTROYTEAMS_H_

#include "IDestroy.h"

class DestroyTeams: public IDestroy
{
public:
    DestroyTeams(const mat2i& distance);
    virtual ~DestroyTeams();

    mat2i destroy(const mat2i& solution);
};

#endif /* DESTROY_DESTROYTEAMS_H_ */
