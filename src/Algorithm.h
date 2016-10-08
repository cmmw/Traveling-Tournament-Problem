/*
 * Algorithm.h
 *
 *  Created on: 08.10.2016
 *      Author: Christian
 */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <iostream>
#include <vector>

class Algorithm
{
public:
    Algorithm(const std::vector<std::vector<int>>& distances);
    virtual ~Algorithm();

    void init(std::vector<std::vector<int>>& solution);
    virtual bool solve(std::vector<std::vector<int>>& solution, bool optimal = false) = 0;
    static int eval(const std::vector<std::vector<int> >& solution, const std::vector<std::vector<int>>& distances);

    template<typename T>
    static void printMatrix(const std::vector<std::vector<T>>& m)
    {
        for (auto r : m)
        {
            for (auto t : r)
            {
                std::cout << t << " ";
            }
            std::cout << std::endl;
        }
    }

    int getNumTeams();
    int getNumRounds();

protected:
    int m_teams;
    int m_rounds;
    const std::vector<std::vector<int>>& m_distances;
    std::vector<std::vector<std::vector<int>>>m_domain;

    bool forwardCheck(int team, int round, const std::vector<std::vector<int>>& solution, std::vector<std::vector<std::vector<int>>>& domain);

    class ValueSorter
    {
    public:
        ValueSorter(int rounds, int currTeam, int currRound, const std::vector<std::vector<int>>& solution, const std::vector<std::vector<int>>& distances) : m_rounds(rounds), m_currTeam(currTeam), m_currRound(currRound), m_solution(solution), m_distances(distances)
        {
        }

        bool operator()(int value1, int value2);

    private:
        int m_rounds;
        int m_currTeam;
        int m_currRound;
        const std::vector<std::vector<int>>& m_solution;
        const std::vector<std::vector<int>>& m_distances;
    };
};

#endif /* ALGORITHM_H_ */
