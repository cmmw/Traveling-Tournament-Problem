/*
 * Factorization.cpp
 *
 *  Created on: 11.11.2016
 *      Author: Christian
 */

#include "Factorization.h"
#include <iostream>
#include <cmath>

Factorization::Factorization(const mat2i& distance) :
        m_teams(distance.size()), m_rounds(2 * m_teams - 2), m_distance(distance)
{
}

Factorization::~Factorization()
{
}

bool Factorization::solve(mat2i& solution)
{
    solution.clear();
    Common::initSolution(m_teams, solution);

    std::vector<int> polygon(m_teams - 1);
    for (unsigned int i = 0; i < polygon.size(); i++)
    {
        polygon[i] = i + 1;
    }

    int sign = 1;
    int topTeam = m_teams;
    for (int i = 0; i < m_teams - 1; i++)
    {
        solution[topTeam - 1][i] = sign * polygon[0];
        solution[polygon[0] - 1][i] = -sign * topTeam;

        solution[topTeam - 1][i + m_teams - 1] = -sign * polygon[0];
        solution[polygon[0] - 1][i + m_teams - 1] = sign * topTeam;

        for (int l = 2; l <= m_teams / 2; l++)
        {
            solution[polygon[l - 1] - 1][i] = sign * polygon[m_teams - l];
            solution[polygon[m_teams - l] - 1][i] = -sign * polygon[l - 1];

            solution[polygon[l - 1] - 1][i + m_teams - 1] = -sign * polygon[m_teams - l];
            solution[polygon[m_teams - l] - 1][i + m_teams - 1] = sign * polygon[l - 1];
        }

        sign *= -1;

        //rotate clockwise
        int tmp = polygon.back();
        for (int i = polygon.size() - 1; i > 0; i--)
        {
            polygon[i] = polygon[i - 1];
        }
        polygon[0] = tmp;
    }
    return true;
}

