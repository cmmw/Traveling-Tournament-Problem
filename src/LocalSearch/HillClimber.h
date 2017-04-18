/*
 * HillClimber.h
 *
 *  Created on: Mar 13, 2017
 *      Author: christian
 */

#ifndef SRC_LOCALSEARCH_HILLCLIMBER_H_
#define SRC_LOCALSEARCH_HILLCLIMBER_H_

#include "../Common.h"

class HillClimber
{
public:
    HillClimber(const mat2i& solution, const mat2i& distance);
    virtual ~HillClimber();

    mat2i solve();

private:

    typedef bool (HillClimber::*swapFunc)();
    typedef std::vector<std::pair<int, int>> entries;

    int m_teams;
    int m_rounds;
    //Hill climber parameters
    bool m_findBestImprovement;
    mat2i m_distance;

    //Temporary solution object which is used to calculate neighbor
    mat2i m_solution;
    int m_solutionDistance;

    //Best solution so far
    mat2i m_incumbentSolution;
    int m_incumbentDistance;

    //Base solution for the current iteration
    mat2i m_baseSolution;
    int m_baseDistance;

    //inRund[0][1] = 2 means team 0 plays at home against team 1 in round 2
    //-1 means no such game exists in the current plan. Needed for some neighborhoods
    mat2i m_inRound;
    std::vector<swapFunc> m_neighborhoods;

    //Neighborhoods:
    // solution - output parameter, if true is returned, this value will be set to the found solution, otherwise it is not changed
    // distance - input/output parameter, input is the value of the current best solution, output the value of the found solution if return value is true
    // returns true if a better solution was found
    bool swapHomes();
    entries moveHomes(int t1, int t2);
    bool swapRounds();
    entries moveRounds(int r1, int r2);
    bool swapTeams();
    entries moveTeams(int t1, int t2);
    bool swapPartialRounds();
    entries movePartialRounds(int r1, int r2, int t);
    bool swapPartialTeams();
    entries movePartialTeams(int t1, int t2, int r);
    //Check constraints and set solution to new solution if it is better
    bool checkSolution(const std::vector<std::pair<int, int>>& cells);
    void calcInRound();
    void undoMoves(const entries& cells);
    inline void swap(const int t1, const int r1, const int t2, const int r2);
    inline void swapWithoutSigns(const int t1, const int r1, const int t2, const int r2);
};

#endif /* SRC_LOCALSEARCH_HILLCLIMBER_H_ */
