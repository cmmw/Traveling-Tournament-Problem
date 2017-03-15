/*
 * HillClimber.cpp
 *
 *  Created on: Mar 13, 2017
 *      Author: christian
 */

#include "HillClimber.h"

HillClimber::HillClimber(const mat2i& solution, const mat2i& distance) :
        m_teams(solution.size()), m_rounds(m_teams * 2 - 2), m_solution(solution), m_distance(distance)
{
}

HillClimber::~HillClimber()
{
}

mat2i HillClimber::solve()
{
    int distance = Common::eval(m_solution, m_distance);
    mat2i solution = m_solution;

    swapHomes(solution, distance);

    return solution;
}

void HillClimber::swapHomes(mat2i& solution, int& distance)
{
    mat2i helperMap(solution.size(), std::vector<int>(solution[0].size()));

    for (int t1 = 0; t1 < m_teams - 1; t1++)
    {
        for (int t2 = t1 + 1; t2 < m_teams; t2++)
        {

        }
    }
}
