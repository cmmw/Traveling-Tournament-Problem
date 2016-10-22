/*
 * ConstHeu.h
 *
 *  Created on: 16.10.2016
 *      Author: Christian
 */

#ifndef GRAPHCOLORHEU_H_
#define GRAPHCOLORHEU_H_

#include "Algorithm.h"
#include <map>

class Game
{
public:
    Game() = delete;
    Game(int t1, int t2, int round, int rounds) :
            m_t1(t1), m_t2(t2), m_round(round)
    {
        for (int i = 0; i < rounds; i++)
        {
            m_domain.push_back(i);
        }
    }

    int m_t1;
    int m_t2;
    int m_round;
    std::vector<Game*> m_neighbors;
    std::vector<int> m_domain;
};

class GraphColorHeu: public Algorithm
{
public:
    GraphColorHeu(const mat2i& distances);
    virtual ~GraphColorHeu();

    bool solve(mat2i& solution, bool optimal = false);
    bool backTrack(std::map<std::pair<int, int>, Game>& games);
    bool forwardCheck(Game& games);
};

#endif /* GRAPHCOLORHEU_H_ */
