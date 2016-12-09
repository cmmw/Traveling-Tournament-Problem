/*
 * Common.cpp
 *
 *  Created on: 23.10.2016
 *      Author: Christian
 */

#include "Common.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

void Common::initSolution(int n, mat2i& solution)
{
    solution.resize(n);
    for (int i = 0; i < n; i++)
    {
        solution[i].resize(2 * n - 2);
    }
}

void Common::printMatrix(const mat2i& matrix)
{
    for (auto r : matrix)
    {
        for (auto t : r)
        {
            std::cout << std::setw(3) << t << " ";
        }
        std::cout << std::endl;
    }
}

int Common::eval(const mat2i& solution, const mat2i& distance)
{
    if (solution.empty())
        return 0;
    int costs = 0;
    int teams = distance.size();
    int rounds = 2 * distance.size() - 2;

    for (int team = 0; team < teams; team++)
    {
        //Home -> first opponent
        int opponent = solution[team][0];
        if (opponent < 0)
            costs += distance[team][std::abs(opponent) - 1];

        for (int round = 1; round < rounds; round++)
        {
            int start = solution[team][round - 1];
            opponent = solution[team][round];
            if (start < 0 && opponent > 0)
            {
                //Away -> Home
                costs += distance[std::abs(start) - 1][team];
            } else if (start > 0 && opponent < 0)
            {
                //Home -> Away
                costs += distance[team][std::abs(opponent) - 1];
            } else if (start < 0 && opponent < 0)
            {
                //Away -> Away
                costs += distance[std::abs(start) - 1][std::abs(opponent) - 1];
            }
        }

        //Last opponent -> home
        opponent = solution[team][rounds - 1];
        if (opponent < 0)
            costs += distance[std::abs(opponent) - 1][team];
    }
    return costs;
}

int Common::homeAwayTrips(const mat2i& solution)
{
    int trips = 0;
    for (unsigned int t = 0; t < solution.size(); t++)
    {
        if (solution[t][0] < 0)
            trips++;
        for (unsigned int r = 0; r < solution[t].size() - 1; r++)
        {
            if (solution[t][r] != 0 && solution[t][r + 1] != 0)
            {
                if (std::signbit(solution[t][r]) != std::signbit(solution[t][r + 1]))
                    trips++;
            }
        }
        if (solution[t][solution[t].size() - 1] < 0)
            trips++;
    }
    return trips;
}

int Common::countUnsetVariables(const mat2i& solution)
{
    int free = 0;
    for (unsigned int t = 0; t < solution.size(); t++)
    {
        for (unsigned int r = 0; r < solution[t].size(); r++)
        {
            if (solution[t][r] == 0)
            {
                free++;
            }
        }
    }
    return free;
}

int Common::getDistance(const mat2i& distance, int teamIdx, int t1, int t2)
{
    //both play at home
    if (t1 > 0 && t2 > 0)
        return 0;

    //both play away
    if (t1 < 0 && t2 < 0)
        return distance[std::abs(t1) - 1][std::abs(t2) - 1];

    //t1 home, t2 away
    if (t1 > 0 && t2 < 0)
        return distance[teamIdx][std::abs(t2) - 1];

    //t1 away, t2 home
    return distance[std::abs(t1) - 1][teamIdx];

}

mat2i Common::calcTravelMap(const mat2i& solution)
{
    int teams = solution.size();
    int rounds = teams * 2 - 2;
    mat2i travelMap(teams);
    for (auto& m : travelMap)
        m.resize(teams);

    for (int t = 0; t < teams; t++)
    {
        if (solution[t][0] < 0)
        {
            travelMap[t][std::abs(solution[t][0]) - 1]++;
            travelMap[std::abs(solution[t][0]) - 1][t]++;
        }
        if (solution[t][rounds - 1] < 0)
        {
            travelMap[t][std::abs(solution[t][rounds - 1]) - 1]++;
            travelMap[std::abs(solution[t][rounds - 1]) - 1][t]++;
        }
        for (int r = 0; r < rounds - 1; r++)
        {
            int t1 = solution[t][r];
            int t2 = solution[t][r + 1];
            if (std::signbit(t1) != std::signbit(t2))
            {
                if (t1 > 0)
                {
                    //t1 > 0, t2 < 0
                    travelMap[t][std::abs(t2) - 1]++;
                    travelMap[std::abs(t2) - 1][t]++;
                } else
                {
                    //t1 < 0, t2 > 0
                    travelMap[std::abs(t1) - 1][t]++;
                    travelMap[t][std::abs(t1) - 1]++;
                }
            } else if (t1 < 0)
            {
                //both < 0
                travelMap[std::abs(t1) - 1][std::abs(t2) - 1]++;
                travelMap[std::abs(t2) - 1][std::abs(t1) - 1]++;
            }
        }
    }
    return travelMap;
}

int Common::deltaDistance(int team, int round, int val, const mat2i& solution, const mat2i& distance)
{
    int left = team + 1, right = team + 1;
    int d = 0;

    if (round > 0)
        left = solution[team][round - 1];

    if (round < (int) solution[0].size() - 1)
        right = solution[team][round + 1];

    if (left != 0)
        d += getDistance(distance, team, left, val);

    if (right != 0)
        d += getDistance(distance, team, val, right);

    return d;
}

int Common::deltaTrips(int team, int round, int val, const mat2i& solution)
{
    int left = team + 1, right = team + 1;
    int trips = 0;

    if (round > 0)
        left = solution[team][round - 1];

    if (round < (int) solution[0].size() - 1)
        right = solution[team][round + 1];

    if (left != 0 && std::signbit(left) != std::signbit(val))
        trips++;

    if (right != 0 && std::signbit(right) != std::signbit(val))
        trips++;

    return trips;
}
