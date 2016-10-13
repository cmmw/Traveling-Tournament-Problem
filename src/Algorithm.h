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

typedef std::vector<std::vector<int>> mat2i;
typedef std::vector<mat2i> mat3i;

class Algorithm
{
public:
    Algorithm(const mat2i& distances);
    virtual ~Algorithm();

    void init(mat2i& solution);
    virtual bool solve(mat2i& solution, bool optimal = false) = 0;
    static int eval(const std::vector<std::vector<int> >& solution, const mat2i& distances);

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
    const mat2i& m_distances;
    mat3i m_domain;

    bool forwardCheck(int team, int round, const mat2i& solution, mat3i& domain);

    class ValueSorter
    {
    public:
        ValueSorter(int rounds, int currTeam, int currRound, const mat2i& solution, const mat2i& distances) :
                m_rounds(rounds), m_currTeam(currTeam), m_currRound(currRound), m_solution(solution), m_distances(distances)
        {
        }

        bool operator()(int value1, int value2);

    private:
        int m_rounds;
        int m_currTeam;
        int m_currRound;
        const mat2i& m_solution;
        const mat2i& m_distances;
    };
};

#endif /* ALGORITHM_H_ */
