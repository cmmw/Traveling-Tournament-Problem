/*
 * IRepair.h
 *
 *  Created on: 21.11.2016
 *      Author: Christian
 */

#ifndef REPAIR_IREPAIR_H_
#define REPAIR_IREPAIR_H_

#include "../Common.h"
#include <limits>

class IRepair
{
public:
    IRepair(const mat2i& distance);
    virtual ~IRepair() = default;

    mat2i solve(const mat2i& solution, int upperBound = std::numeric_limits<int>::max());

protected:
    int m_teams;
    int m_rounds;
    int m_upperBound;
    int m_bestSolutionValue;
    mat2i m_bestSolution;
    mat2i m_distance;
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
    bool forwardCheck(int team, int round, const mat2i& solution, std::vector<DomainBackupEntry>& domainBackup);
    bool contains(int team, int round, const std::vector<DomainBackupEntry>& domainBackup);

private:
    virtual void solveImpl(const mat2i& solution) = 0;

};

#endif /* REPAIR_IREPAIR_H_ */
