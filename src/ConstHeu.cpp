/*
 * ConstHeu.cpp
 *
 *  Created on: 16.10.2016
 *      Author: Christian
 */

#include "ConstHeu.h"
#include <algorithm>

ConstHeu::ConstHeu(const mat2i& distances) :
        Algorithm(distances)
{
}

ConstHeu::~ConstHeu()
{
}

bool ConstHeu::solve(mat2i& solution, bool optimal)
{
    solution.clear();
    init(solution);

    for (int t = 0; t < m_teams; t++)
    {
        std::vector<int> randomRound;
        for (int rr = 0; rr < m_teams; rr++)
        {
            if (rr != t)
            {
                randomRound.push_back((rr + 1));
                randomRound.push_back(-(rr + 1));
            }
        }
        std::random_shuffle(randomRound.begin(), randomRound.end());
        solution[t] = randomRound;
    }

    std::vector<int> domain;
    for (int i = -m_teams; i < 0; i++)
    {
        domain.push_back(i);
        domain.push_back(-i);
    }

    std::set<std::pair<int, int>> conflicted = Algorithm::getConflicted(solution);
    while (conflicted.size() > 0)
    {
        int r = std::rand() % conflicted.size();
        std::set<std::pair<int, int>>::const_iterator it = conflicted.begin();
        std::advance(it, r);
        std::pair<int, int> var = *it;
        int conflicts = Algorithm::conflicts(solution);
        int bestValue = solution[var.first][var.second];
        bool improved = false;
        for (auto x : domain)
        {
            solution[var.first][var.second] = x;
            int c = Algorithm::conflicts(solution);
            if (c < conflicts)
            {
                bestValue = x;
                conflicts = c;
                improved = true;
            }
        }
        if (!improved)
        {
            solution[var.first][var.second] = domain[std::rand() % domain.size()];
        } else
        {
            solution[var.first][var.second] = bestValue;
        }
        conflicted = Algorithm::getConflicted(solution);
//        Algorithm::printMatrix(solution);
        std::cout << "conflicted var: " << conflicted.size() << std::endl;
        std::cout << "conflicts: " << conflicts << std::endl;
    }
    return true;
}
