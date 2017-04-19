/*
 * IPSolver.cpp
 *
 *  Created on: Jan 2, 2017
 *      Author: christian
 */

#include "../IPSolver/IPSolver.h"
#include "../Params.h"
#include <sstream>

IPSolver::IPSolver(const mat2i& distance) :
        m_teams(distance.size()), m_rounds(m_teams * 2 - 2), m_distance(distance), m_model(m_env), m_cplex(m_model), m_objective(m_env), m_constraintArray(m_env)
{
    /*Set cplex parameters*/
    setParams();
    /*Build model*/
    initModel();
}

IPSolver::~IPSolver()
{
    m_constraintArray.endElements();
    m_objective.end();
    m_cplex.end();
    m_model.end();
    m_env.end();
}

mat2i IPSolver::solve()
{
    m_cplex.exportModel("model.lp");

    /*Solve*/
    m_cplex.solve();

    /*Convert model to solution in mat2i format*/
    mat2i solution = convert();

    /*Delete constraints*/
    m_model.remove(m_constraintArray);
    m_constraintArray.endElements();

    return solution;
}

void IPSolver::initModel()
{
    //x_ijk means team i plays away against team j at round k
    for (int i = 0; i < m_teams; i++)
    {
        m_x.push_back(std::vector<std::vector<IloIntVar>>());
        for (int j = 0; j < m_teams; j++)
        {
            m_x[i].push_back(std::vector<IloIntVar>());
            for (int k = 0; k < m_rounds; k++)
            {
                std::stringstream ss;
                ss << "x_" << i << "," << j << "," << k;
                m_x[i][j].push_back(IloIntVar(m_env, 0, 1, ss.str().c_str()));
            }
        }
    }

    //No team plays against itself
    for (int i = 0; i < m_teams; i++)
    {
        for (int k = 0; k < m_rounds; k++)
        {
            m_model.add(m_x[i][i][k] == 0);
        }
    }

    //Every team plays exactly one game each round
    for (int i = 0; i < m_teams; i++)
    {
        for (int k = 0; k < m_rounds; k++)
        {
            IloExpr expr(m_env);
            for (int j = 0; j < m_teams; j++)
            {
                expr += m_x[i][j][k] + m_x[j][i][k];
            }
            m_model.add(expr == 1);
            expr.end();
        }
    }

    //Every team i plays against team j exactly once in all rounds (and vis a vis)
    for (int i = 0; i < m_teams; i++)
    {
        for (int j = 0; j < m_teams; j++)
        {
            if (i == j)
                continue;
            IloExpr expr(m_env);
            for (int k = 0; k < m_rounds; k++)
            {
                expr += m_x[i][j][k];
            }
            m_model.add(expr == 1);
            expr.end();
        }
    }

    //At most 3 consecutive away games (i plays at most 3 times consecutive away games)
    for (int i = 0; i < m_teams; i++)
    {
        for (int k = 0; k < m_rounds - 3; k++)
        {
            IloExpr expr(m_env);
            for (int l = 0; l < 4; l++)
            {
                for (int j = 0; j < m_teams; j++)
                {
                    expr += m_x[i][j][k + l];
                }
            }
            m_model.add(expr <= 3);
            expr.end();
        }
    }

    //At most 3 consecutive home games (j plays at most 3 times consecutive home games)
    for (int j = 0; j < m_teams; j++)
    {
        for (int k = 0; k < m_rounds - 3; k++)
        {
            IloExpr expr(m_env);
            for (int l = 0; l < 4; l++)
            {
                for (int i = 0; i < m_teams; i++)
                {
                    expr += m_x[i][j][k + l];
                }
            }
            m_model.add(expr <= 3);
            expr.end();
        }
    }

    //No repeat
    for (int i = 0; i < m_teams; i++)
    {
        for (int j = 0; j < m_teams; j++)
        {
            for (int k = 0; k < m_rounds - 1; k++)
            {
                m_model.add((m_x[i][j][k] + m_x[j][i][k] + m_x[i][j][k + 1] + m_x[j][i][k + 1]) <= 1);
            }
        }
    }

    //Helper variable z_ijk (team i is at venue j at round k)
    for (int i = 0; i < m_teams; i++)
    {
        m_z.push_back(std::vector<std::vector<IloIntVar>>());
        for (int j = 0; j < m_teams; j++)
        {
            m_z[i].push_back(std::vector<IloIntVar>());
            for (int k = 0; k < m_rounds; k++)
            {
                std::stringstream ss;
                ss << "z_" << i << ", " << j << ", " << k;
                m_z[i][j].push_back(IloIntVar(m_env, 0, 1, ss.str().c_str()));
            }
        }
    }

    for (int i = 0; i < m_teams; i++)
    {

        for (int k = 0; k < m_rounds; k++)
        {
            IloExpr expr(m_env);
            for (int j = 0; j < m_teams; j++)
            {
                //bug in paper: (x[i][j][k] instead of x[j][i][k])
                expr += m_x[j][i][k];
            }
            m_model.add(m_z[i][i][k] == expr);
            expr.end();
        }
    }

    for (int i = 0; i < m_teams; i++)
    {
        for (int j = 0; j < m_teams; j++)
        {
            for (int k = 0; k < m_rounds; k++)
            {
                if (i != j)
                    m_model.add(m_z[i][j][k] == m_x[i][j][k]);
            }
        }
    }

    //Helper variable y_tij, team t travels from i to j anywhere in the schedule
    for (int t = 0; t < m_teams; t++)
    {
        m_y.push_back(std::vector<std::vector<IloIntVar>>());
        for (int i = 0; i < m_teams; i++)
        {
            m_y[t].push_back(std::vector<IloIntVar>());
            for (int j = 0; j < m_teams; j++)
            {
                std::stringstream ss;
                ss << "y_" << t << ", " << i << ", " << j;
                m_y[t][i].push_back(IloIntVar(m_env, 0, 1, ss.str().c_str()));
            }
        }
    }

    for (int i = 0; i < m_teams; i++)
    {
        for (int j = 0; j < m_teams; j++)
        {
            for (int t = 0; t < m_teams; t++)
            {
                for (int k = 0; k < m_rounds - 1; k++)
                {
                    m_model.add(m_y[t][i][j] >= (m_z[t][i][k] + m_z[t][j][k + 1] - 1));
                }
            }
        }
    }

    /*Objective function*/
    for (int i = 0; i < m_teams; i++)
    {
        for (int j = 0; j < m_teams; j++)
        {
            m_objective += m_distance[i][j] * m_x[i][j][0];
        }
    }

    for (int t = 0; t < m_teams; t++)
    {
        for (int i = 0; i < m_teams; i++)
        {
            for (int j = 0; j < m_teams; j++)
            {
                m_objective += m_distance[i][j] * m_y[t][i][j];
            }
        }
    }

    for (int i = 0; i < m_teams; i++)
    {
        for (int j = 0; j < m_teams; j++)
        {
            m_objective += m_distance[j][i] * m_x[i][j][m_rounds - 1];
        }
    }

    m_model.add(IloMinimize(m_env, m_objective));
}

void IPSolver::setParams()
{
// print every x-th line of node-log and give more details
    m_cplex.setParam(IloCplex::MIPInterval, 1000);
    m_cplex.setParam(IloCplex::MIPDisplay, 0);
    m_cplex.setWarning(m_cplex.getEnv().getNullStream());
    m_cplex.setParam(IloCplex::Threads, P_IP_THREADS);
    m_cplex.setParam(IloCplex::TiLim, P_IP_TIMEOUT);
}

mat2i IPSolver::convert()
{
    mat2i sol;
    Common::initSolution(m_teams, sol);

    for (int r = 0; r < m_rounds; r++)
    {
        for (int i = 0; i < m_teams; i++)
        {
            for (int j = 0; j < m_teams; j++)
            {
                if (m_cplex.getIntValue(m_x[i][j][r]) == 1)
                {
                    if (sol[i][r] != 0 || sol[j][r] != 0)
                    {
                        std::cout << "This should never happen! [Bug in the ILP model]" << std::endl;
                        exit(-1);
                    }
                    sol[i][r] = -(j + 1);
                    sol[j][r] = i + 1;
                }
            }
        }
    }
    return sol;
}

void IPSolver::populatePartialSolution(const mat2i& sol)
{
    if (sol.empty())
        return;

    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int o = sol[t][r];
            if (o != 0)
            {
                //Optimize partial solution
                IloConstraint c1;
                IloConstraint c2;
                if (o < 0)
                {
                    o = -o;
                    c1 = m_z[t][o - 1][r] == 1;
                    c2 = m_x[t][o - 1][r] == 1;
                } else
                {
                    c1 = m_z[t][t][r] == 1;
                    c2 = m_x[o - 1][t][r] == 1;
                }
                m_constraintArray.add(c1);
                m_constraintArray.add(c2);

            }
        }
    }
    m_model.add(m_constraintArray);
}

void IPSolver::relaxHomeAwayPattern(const mat2i& sol)
{
    if (sol.empty())
        return;

    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int o = sol[t][r];
            if (o != 0)
            {
                //Optimize Home/Away-Pattern
                IloConstraint c;
                c = ((m_x[t][std::abs(o) - 1][r] + m_x[std::abs(o) - 1][t][r]) == 1);
                m_constraintArray.add(c);
            }
        }
    }
    m_model.add(m_constraintArray);
}

void IPSolver::relaxVenues(const mat2i& sol)
{
    if (sol.empty())
        return;

    for (int t = 0; t < m_teams; t++)
    {
        for (int r = 0; r < m_rounds; r++)
        {
            int o = sol[t][r];
            if (o != 0)
            {
                //Optimize venues
                IloExpr sum1(m_env);
                IloConstraint c1;
                IloExpr sum2(m_env);
                IloConstraint c2;

                for (int i = 0; i < m_teams; i++)
                {
                    if (o < 0)
                    {
                        sum1 += m_x[i][std::abs(o) - 1][r];
                        sum2 += m_x[t][i][r];
                    } else
                    {
                        sum1 += m_x[o - 1][i][r];
                        sum2 += m_x[i][t][r];
                    }
                }

                c1 = (sum1 == 1);
                c2 = (sum2 == 1);

                m_constraintArray.add(c1);
                m_constraintArray.add(c2);
            }
        }
    }
    m_model.add(m_constraintArray);
}
