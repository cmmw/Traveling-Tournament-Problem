/*
 * EdgeRepair.cpp
 *
 *  Created on: 30.11.2016
 *      Author: Christian
 */

#include "EdgeRepair.h"
#include <algorithm>
#include <iostream>

EdgeRepair::EdgeRepair(const mat2i& distance) :
        IRepair(distance)
{
    // TODO Auto-generated constructor stub

}

EdgeRepair::~EdgeRepair()
{
    // TODO Auto-generated destructor stub
}

mat2i EdgeRepair::solveImpl(const mat2i& solution)
{
    mat2i sol = solution;
    init(sol);

    //find home away pattern
    mat2i pat = initPattern(sol);

    pattern(pat);
    int x = -4;
    std::cout << x / std::abs(x) << std::endl;
    Common::printMatrix(pat);

    exit(1);

    return m_bestSolution;
}

mat2i EdgeRepair::initPattern(const mat2i& solution)
{
    m_patDomain.clear();
    mat2i pat(m_teams);
    for (auto& p : pat)
        p.resize(m_rounds);

    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int v = solution[t][r];
            if (v != 0)
            {
                pat[t][r] = v / std::abs(v);
            }
        }
    }
    return pat;
}

bool EdgeRepair::pattern(mat2i& pat)
{

    return false;
}

bool EdgeRepair::patForwardCheck(int team, int round, const mat2i& solution, std::vector<DomainBackupEntry>& domainBackup)
{
    int homeGames = 0;
    int awayGames = 0;

    //Round has the same amount of home-games and away-games, both = m_teams/2
    for (int t = 0; t < m_teams; t++)
    {
        int v = solution[t][round];
        if (v > 0)
        {
            homeGames++;
        } else if (v < 0)
        {
            awayGames++;
        }
        if (awayGames > m_teams / 2 || homeGames > m_teams / 2)
            return false;
    }

    for (int t = 0; t < m_teams; t++)
    {
        int v = solution[t][round];
        if (v == 0)
        {
            if (awayGames == m_teams / 2)
            {

            }
            if (homeGames == m_teams / 2)
            {

            }
        }
    }

    homeGames = 0;
    awayGames = 0;
    //Team has the same amount of home-games and away-games, both = m_rounds/2
    for (int r = 0; r < m_rounds; r++)
    {
        int v = solution[team][r];
        if (v > 0)
        {

        } else if (v < 0)
        {

        } else
        {

        }
    }

    homeGames = 0;
    awayGames = 0;
    //At most m_u consecutive home- or away-games

    return true;
}
