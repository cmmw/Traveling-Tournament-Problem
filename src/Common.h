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
    static int homeAwayTrips(const mat2i& solution);
    static int countUnsetVariables(const mat2i& solution);
    //team with index teamIdx plays against t1 and then t2, calculate travel distance
    static int getDistance(const mat2i& distance, int teamIdx, int t1, int t2);
    static mat2i calcTravelMap(const mat2i& solution);
    static int deltaDistance(int team, int round, int val, const mat2i& solution, const mat2i& distance);
    static int deltaDistance(int team, int round, const mat2i& solution, const mat2i& distance);
    static int deltaTrips(int team, int round, int val, const mat2i& solution);
    //Returns the length of the home or away sequence at team, round
    static int atMost(int team, int round, const mat2i& solution);
    //Returns false if there is a repeating game
    static int noRepeat(int team, int round, const mat2i& solution);
};

#endif /* COMMON_H_ */
