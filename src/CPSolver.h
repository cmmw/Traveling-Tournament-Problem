/*
 * GreedyHeu.h
 *
 *  Created on: 05.10.2016
 *      Author: Christian
 */

#ifndef CPSOLVER_H_
#define CPSOLVER_H_

#include <iostream>
#include <vector>

//Passed solution needs to be a consistent partial solution or empty

class CPSolver
{
public:
    CPSolver(const std::vector<std::vector<int>>& solution, const std::vector<std::vector<int>>& distances);
    CPSolver(const std::vector<std::vector<int>>& distances);
    virtual ~CPSolver();
    bool solve();

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

    std::vector<std::vector<int>> getSolution();

    static int eval(const std::vector<std::vector<int>>& solution, const std::vector<std::vector<int>>& distances);

private:
    int m_teams;
    int m_rounds;
    std::vector<std::vector<int>> m_solution;
    const std::vector<std::vector<int>>& m_distances;
    std::vector<std::vector<std::vector<int>>>m_domain;
    std::vector<std::vector<int>> m_bestSolution;
    int m_bestCosts;
    void initDomain();
    bool backTrack(int team, int round, std::vector<std::vector<int>>& solution, std::vector<std::vector<std::vector<int>>>& domain);
    bool forwardCheck(int team, int round, const std::vector<std::vector<int>>& solution, std::vector<std::vector<std::vector<int>>>& domain);

};

#endif /* CPSOLVER_H_ */
