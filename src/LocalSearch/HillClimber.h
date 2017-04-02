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

    typedef bool (HillClimber::*swapFunc)(mat2i& solution, int& distance);

    int m_teams;
    int m_rounds;
    //Hill climber parameters
    bool m_findBestImprovement;
    mat2i m_solution;
    mat2i m_distance;
    //inRund[0][1] = 2 means team 0 plays at home against team 1 in round 2
    //-1 means no such game exists in the current plan. Needed for some neighborhoods
    mat2i m_inRound;
    std::vector<swapFunc> m_neighborhoods;

    //Neighborhoods:
    // solution - output parameter, if true is returned, this value will be set to the found solution, otherwise it is not changed
    // distance - input/output parameter, input is the value of the current best solution, output the value of the found solution if return value is true
    // returns true if a better solution was found
    bool swapHomes(mat2i& solution, int& distance);
    bool swapRounds(mat2i& solution, int& distance);
    bool swapTeams(mat2i& solution, int& distance);
    bool swapPartialRounds(mat2i& solution, int& distance);
    bool swapPartialTeams(mat2i& solution, int& distance);
    //Check constraints and set solution to new solution if it is better
    bool checkSolution(mat2i& solution, int& distance, const std::vector<std::pair<int, int>>& cells);
    void calcInRound();
};

#endif /* SRC_LOCALSEARCH_HILLCLIMBER_H_ */