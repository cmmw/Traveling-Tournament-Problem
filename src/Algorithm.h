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
#include <set>

typedef std::vector<std::vector<int>> mat2i;
typedef std::vector<mat2i> mat3i;

class DomainEntry
{
public:
    DomainEntry(int team, int round, const std::vector<int>& backup) :
            m_team(team), m_round(round), m_backup(backup)
    {
    }

    int m_team;
    int m_round;
    std::vector<int> m_backup;
};

class Algorithm
{
public:
    Algorithm(const mat2i& distances);
    virtual ~Algorithm();

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

protected:
    int m_teams;
    int m_rounds;
    const mat2i& m_distances;
    mat3i m_domain;

    void init(mat2i& solution);
    bool forwardCheck(int team, int round, const mat2i& solution, mat3i& domain, std::vector<DomainEntry>& domainBackup);
    static bool contains(int team, int round, const std::vector<DomainEntry>& domainBackup);
    static bool getMrv(int& team, int& round, const mat2i& solution, const mat3i& domain);
    int ruledOutValues(int team, int round, int value, const mat2i& solution, const mat3i& domain);
    int ruledOutHorizontal(int team, int round, int value, const mat2i& solution, const mat3i& domain);
    int ruledOutVertical(int team, int round, int value, const mat2i& solution, const mat3i& domain);
    int ruledOutOpponent(int team, int round, int value, const mat2i& solution, const mat3i& domain);
    int ruledOutNoRepeat(int team, int round, int value, const mat2i& solution, const mat3i& domain);
    int ruledOutAtMost(int team, int round, int value, const mat2i& solution, const mat3i& domain);

    static int conflicts(const mat2i& solution);
    static std::set<std::pair<int, int>> getConflicted(const mat2i& solution);
};

#endif /* ALGORITHM_H_ */
