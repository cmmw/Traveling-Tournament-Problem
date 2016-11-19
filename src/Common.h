/*
 * Common.h
 *
 *  Created on: 23.10.2016
 *      Author: Christian
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <vector>

typedef std::vector<std::vector<int>> mat2i;
typedef std::vector<mat2i> mat3i;

class Common
{
public:
    Common() = delete;

    static void initSolution(int n, mat2i& solution);
    static void printMatrix(const mat2i& matrix);
    static int eval(const mat2i& solution, const mat2i& distance);
};

#endif /* COMMON_H_ */
