/*
 * DestroyTeams.h
 *
 *  Created on: Jan 19, 2017
 *      Author: christian
 */

#ifndef SRC_DESTROY_DESTROYTEAMS_H_
#define SRC_DESTROY_DESTROYTEAMS_H_

#include "IDestroy.h"

class DestroyTeams: public IDestroy
{
public:
    DestroyTeams(const mat2i& distance);
    virtual ~DestroyTeams();
    mat2i destroy(const mat2i& solution, int size);

private:
    std::vector<int> homeDistance(const mat2i& solution);
};

#endif /* SRC_DESTROY_DESTROYTEAMS_H_ */
