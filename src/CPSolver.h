/*
 * CPSolver.h
 *
 *  Created on: 23.10.2016
 *      Author: Christian
 */

#ifndef CPSOLVER_H_
#define CPSOLVER_H_

#include "Common.h"
#include <vector>

class CPSolver
{
public:
    bool m_useMRV;
    bool m_useLCV;

    CPSolver(const mat2i& distance);
    virtual ~CPSolver();

    bool solve(mat2i& solution, bool optimal = false);

private:
    int m_teams;
    int m_rounds;
    int m_nodes;
    bool m_optimal;
    int m_bestValue;
    mat2i m_distance;
    mat2i m_bestSolution;
    mat3i m_domain;

    class DomainBackupEntry
    {
    public:
        DomainBackupEntry(int team, int round, const std::vector<int>& backup) :
                m_team(team), m_round(round), m_backup(backup)
        {
        }

        bool operator==(const DomainBackupEntry& o)
        {
            return (o.m_team == this->m_team && o.m_round == this->m_round);
        }
        int m_team;
        int m_round;
        std::vector<int> m_backup;
    };

    void init(mat2i& solution);
    bool backTrack(mat2i& solution);
    bool forwardCheck(int t, int r, const mat2i& solution, std::vector<DomainBackupEntry>& domainBackup);
    bool contains(int team, int round, const std::vector<DomainBackupEntry>& domainBackup);
    bool getUnassignedVar(int& team, int& round, const mat2i& solution);
    bool getNextUnassignedVar(int& team, int& round, const mat2i& solution);
    bool getMRV(int& team, int& round, const mat2i& solution);

    int ruledOutValues(int team, int round, int value, const mat2i& solution);
    int ruledOutHorizontal(int team, int round, int value, const mat2i& solution);
    int ruledOutVertical(int team, int round, int value, const mat2i& solution);
    int ruledOutOpponent(int team, int round, int value, const mat2i& solution);
    int ruledOutNoRepeat(int team, int round, int value, const mat2i& solution);
    int ruledOutAtMost(int team, int round, int value, const mat2i& solution);
};

#endif /* CPSOLVER_H_ */
