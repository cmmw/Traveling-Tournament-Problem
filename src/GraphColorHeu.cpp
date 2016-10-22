/*
 * ConstHeu.cpp
 *
 *  Created on: 16.10.2016
 *      Author: Christian
 */

#include "GraphColorHeu.h"

#include <algorithm>
#include <map>

GraphColorHeu::GraphColorHeu(const mat2i& distances) :
        Algorithm(distances)
{
}

GraphColorHeu::~GraphColorHeu()
{
}

bool GraphColorHeu::solve(mat2i& solution, bool optimal)
{
    std::map<std::pair<int, int>, Game> games;

    //Construct Single Round Robin
    //create nodes
    for (int t1 = 0; t1 < m_teams; t1++)
    {
        for (int t2 = t1 + 1; t2 < m_teams; t2++)
        {
            games.insert(std::make_pair(std::make_pair(t1, t2), Game(t1, t2, -1, m_rounds / 2)));
        }
    }

    //create edges
    for (auto& g : games)
    {
        for (int t1 = 0; t1 < m_teams; t1++)
        {
            for (int t2 = t1 + 1; t2 < m_teams; t2++)
            {
                if (t1 == g.second.m_t1 && t2 == g.second.m_t2)
                    continue;
                if (t1 == g.second.m_t1 || t2 == g.second.m_t1 || t1 == g.second.m_t2 || t2 == g.second.m_t2)
                {
                    g.second.m_neighbors.push_back(&games.at(std::make_pair(t1, t2)));
                }
            }
        }
    }

    //solve graph coloring
    bool found = backTrack(games);
    if (found)
    {
        //map graph coloring solution to double round robin
        solution.clear();
        init(solution);

        for (auto& g : games)
        {
            solution[g.second.m_t1][g.second.m_round] = g.second.m_t2 + 1;
            solution[g.second.m_t2][g.second.m_round] = -(g.second.m_t1 + 1);
            //mirror second half
            solution[g.second.m_t1][g.second.m_round + (m_rounds / 2)] = -(g.second.m_t2 + 1);
            solution[g.second.m_t2][g.second.m_round + (m_rounds / 2)] = g.second.m_t1 + 1;
        }
    }
    return found;
}

bool GraphColorHeu::backTrack(std::map<std::pair<int, int>, Game>& games)
{
    //take next unassigned variable (MRV)
    Game* game = nullptr;
    int remainingValues = std::numeric_limits<int>::max();
    for (auto &g : games)
    {
        if (g.second.m_round == -1)
        {
            int values = g.second.m_domain.size();
            if (values < remainingValues)
            {
                remainingValues = values;
                game = &g.second;
            } else if (values == remainingValues)   //Tie breaking: choose variable with more unassigned neighbors
            {
                int unassigned1 = 0, unassigned2 = 0;
                for (auto& n : g.second.m_neighbors)
                {
                    if (n->m_round == -1)
                        unassigned1++;
                }
                for (auto& n : game->m_neighbors)
                {
                    if (n->m_round == -1)
                        unassigned2++;
                }
                if (unassigned1 > unassigned2)
                {
                    game = &g.second;
                }
            }
        }
    }

    //solution found
    if (game == nullptr)
    {
        return true;
    }

    //Assign value
    for (auto d : game->m_domain)
    {
        //create backup of neighbors domain
        mat2i domainBackup;
        for (unsigned int i = 0; i < game->m_neighbors.size(); i++)
        {
            domainBackup.push_back(game->m_neighbors[i]->m_domain);
        }
        //set value
        game->m_round = d;
        if (forwardCheck(*game))
        {
            if (backTrack(games))
            {
                return true;
            }
        }
        //reset domain of neighbors
        for (unsigned int i = 0; i < game->m_neighbors.size(); i++)
        {
            game->m_neighbors[i]->m_domain = domainBackup[i];
        }
    }
    game->m_round = -1;
    return false;
}

bool GraphColorHeu::forwardCheck(Game& game)
{
    for (auto& n : game.m_neighbors)
    {
        if (n->m_round == -1)
        {
            n->m_domain.erase(std::remove(n->m_domain.begin(), n->m_domain.end(), game.m_round), n->m_domain.end());
            if (n->m_domain.empty())
                return false;
        }
    }
    return true;
}
