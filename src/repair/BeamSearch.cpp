/*
 * BeamSearch.cpp
 *
 *  Created on: 07.12.2016
 *      Author: Christian
 */

#include "BeamSearch.h"
#include <algorithm>
#include <cmath>
#include <iostream>

BeamSearch::BeamSearch(const mat2i& distance) :
        IRepair(distance), m_width(500)
{
}

BeamSearch::~BeamSearch()
{
}

mat2i BeamSearch::solveImpl(const mat2i& solution)
{
    mat2i sol = solution;
    init(sol);

    beamSearch(sol);

    return m_bestSolution;
}

bool BeamSearch::beamSearch(mat2i& s)
{

    std::vector<SolWithDom> currSolutions;
    currSolutions.reserve(m_width);
    currSolutions.push_back(
            { s, m_domain, Common::eval(s, m_distance), Common::homeAwayTrips(s) });
    do
    {
        std::vector<SolWithDom> nextSolutions;
        nextSolutions.reserve(m_width);
        for (auto sd : currSolutions)
        {
            int team;
            int round;
            if (!getNextVariable(team, round, sd.solution))
            {
                //Solution Found
                if (m_bestSolutionValue > sd.distance)
                {
                    m_bestSolutionValue = sd.distance;
                    m_bestSolution = sd.solution;
                }
            } else
            {
                for (auto d : sd.domain[team][round])
                {
                    sd.solution[team][round] = d;
                    bool setOpponent = sd.solution[std::abs(d) - 1][round] == 0;
                    if (setOpponent)
                    {
                        if (d < 0)
                            sd.solution[-d - 1][round] = (team + 1);
                        else
                            sd.solution[d - 1][round] = -(team + 1);
                    }
                    std::vector<DomainBackupEntry> domainBackup;
                    if (forwardCheck(team, round, sd.solution, domainBackup, sd.domain) && forwardCheck(std::abs(d) - 1, round, sd.solution, domainBackup, sd.domain))
                    {
                        int distance = sd.distance + Common::deltaDistance(team, round, sd.solution[team][round], sd.solution, m_distance) + Common::deltaDistance(std::abs(d) - 1, round, sd.solution[std::abs(d) - 1][round], sd.solution, m_distance);
                        int trips = sd.trips + Common::deltaTrips(team, round, sd.solution[team][round], sd.solution) + Common::deltaTrips(std::abs(d) - 1, round, sd.solution[std::abs(d) - 1][round], sd.solution);
                        nextSolutions.push_back(
                                { sd.solution, sd.domain, distance, trips });
                    }
                    if (setOpponent)
                        sd.solution[std::abs(d) - 1][round] = 0;
                    resetDoamin(domainBackup, sd.domain);
                }
            }
        }

        if (nextSolutions.size() > m_width)
        {
            std::sort(nextSolutions.begin(), nextSolutions.end(), [&](const SolWithDom& sd1, const SolWithDom& sd2)
            {
                return sd1.distance < sd2.distance;
            });
//            int alpha = m_width / 2;
//            std::sort(nextSolutions.begin() + alpha, nextSolutions.end(), [&](const SolWithDom& sd1, const SolWithDom& sd2)
//            {
//                return sd1.trips < sd2.trips;
//            });
            nextSolutions.resize(m_width);
        }

        std::swap(nextSolutions, currSolutions);

    } while (!currSolutions.empty());

    return false;
}

bool BeamSearch::getNextVariable(int& team, int& round, const mat2i& solution)
{
    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            if (solution[t][r] == 0)
            {
                team = t;
                round = r;
                return true;
            }
        }
    }
    return false;
}
