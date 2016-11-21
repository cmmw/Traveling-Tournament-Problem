/*
 * Common.h
 *
 *  Created on: 23.10.2016
 *      Author: Christian
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <vector>

typedef std::vector<std::vector<int>> mat2i;
typedef std::vector<mat2i> mat3i;

class Common
{
public:
    Common() = delete;

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

    static void initSolution(int n, mat2i& solution);
    static void printMatrix(const mat2i& matrix);
    static int eval(const mat2i& solution, const mat2i& distance);

    static bool forwardCheck(int t, int r, const mat2i& solution, mat3i& domain, std::vector<DomainBackupEntry>& domainBackup);
    static bool contains(int team, int round, const std::vector<DomainBackupEntry>& domainBackup);
};

#endif /* COMMON_H_ */
