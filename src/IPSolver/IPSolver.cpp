/*
 * IPSolver.cpp
 *
 *  Created on: Jan 2, 2017
 *      Author: christian
 */

#include "../IPSolver/IPSolver.h"

#include <sstream>

IPSolver::IPSolver(const mat2i& distance) :
        m_teams(distance.size()), m_rounds(m_teams * 2 - 2), m_distance(distance)
{
}

IPSolver::~IPSolver()
{
}

mat2i IPSolver::solve(const mat2i& solution)
{
    IloEnv env;
    IloModel model(env);
    IloCplex cplex(model);

    /*Build model*/

    //x_ijk means team i plays away against team j at round k
    IloIntVar3 x;
    for (int i = 0; i < m_teams; i++)
    {
        x.push_back(std::vector<std::vector<IloIntVar>>());
        for (int j = 0; j < m_teams; j++)
        {
            x[i].push_back(std::vector<IloIntVar>());
            for (int k = 0; k < m_rounds; k++)
            {
                std::stringstream ss;
                ss << "x_" << i << "," << j << "," << k;
                x[i][j].push_back(IloIntVar(env, 0, 1, ss.str().c_str()));
            }
        }
    }

    //No team plays against itself
    for (int i = 0; i < m_teams; i++)
    {
        for (int k = 0; k < m_rounds; k++)
        {
            model.add(x[i][i][k] == 0);
        }
    }

    //Every team plays exactly one game each round
    for (int i = 0; i < m_teams; i++)
    {
        for (int k = 0; k < m_rounds; k++)
        {
            IloExpr expr(env);
            for (int j = 0; j < m_teams; j++)
            {
                expr += x[i][j][k] + x[j][i][k];
            }
            model.add(expr == 1);
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
            IloExpr expr(env);
            for (int k = 0; k < m_rounds; k++)
            {
                expr += x[i][j][k];
            }
            model.add(expr == 1);
            expr.end();
        }
    }

    //At most 3 consecutive away games (i plays at most 3 times consecutive away games)
    for (int i = 0; i < m_teams; i++)
    {
        for (int k = 0; k < m_rounds - 3; k++)
        {
            IloExpr expr(env);
            for (int l = 0; l < 4; l++)
            {
                for (int j = 0; j < m_teams; j++)
                {
                    expr += x[i][j][k + l];
                }
            }
            model.add(expr <= 3);
            expr.end();
        }
    }

    //At most 3 consecutive home games (j plays at most 3 times consecutive home games)
    for (int j = 0; j < m_teams; j++)
    {
        for (int k = 0; k < m_rounds - 3; k++)
        {
            IloExpr expr(env);
            for (int l = 0; l < 4; l++)
            {
                for (int i = 0; i < m_teams; i++)
                {
                    expr += x[i][j][k + l];
                }
            }
            model.add(expr <= 3);
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
                model.add((x[i][j][k] + x[j][i][k] + x[i][j][k + 1] + x[j][i][k + 1]) <= 1);
            }
        }
    }

    //Helper variable z_ijk (team i is at venue j at round k)
    IloIntVar3 z;
    for (int i = 0; i < m_teams; i++)
    {
        z.push_back(std::vector<std::vector<IloIntVar>>());
        for (int j = 0; j < m_teams; j++)
        {
            z[i].push_back(std::vector<IloIntVar>());
            for (int k = 0; k < m_rounds; k++)
            {
                std::stringstream ss;
                ss << "z_" << i << ", " << j << ", " << k;
                z[i][j].push_back(IloIntVar(env, 0, 1, ss.str().c_str()));
            }
        }
    }

    for (int i = 0; i < m_teams; i++)
    {

        for (int k = 0; k < m_rounds; k++)
        {
            IloExpr expr(env);
            for (int j = 0; j < m_teams; j++)
            {
                //bug in paper: (x[i][j][k] instead of x[j][i][k])
                expr += x[j][i][k];
            }
            model.add(z[i][i][k] == expr);
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
                    model.add(z[i][j][k] == x[i][j][k]);
            }
        }
    }

    //Helper variable y_tij, team t travels from i to j anywhere in the schedule
    IloIntVar3 y;
    for (int t = 0; t < m_teams; t++)
    {
        y.push_back(std::vector<std::vector<IloIntVar>>());
        for (int i = 0; i < m_teams; i++)
        {
            y[t].push_back(std::vector<IloIntVar>());
            for (int j = 0; j < m_teams; j++)
            {
                std::stringstream ss;
                ss << "y_" << t << ", " << i << ", " << j;
                y[t][i].push_back(IloIntVar(env, 0, 1, ss.str().c_str()));
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
                    model.add(y[t][i][j] >= (z[t][i][k] + z[t][j][k + 1] - 1));
                }
            }
        }
    }

    /*Populate current solution*/
    populatePartialSolution(solution, x, z, model);

    /*Objective function*/
    IloExpr objective(env);
    for (int i = 0; i < m_teams; i++)
    {
        for (int j = 0; j < m_teams; j++)
        {
            objective += m_distance[i][j] * x[i][j][0];
        }
    }

    for (int t = 0; t < m_teams; t++)
    {
        for (int i = 0; i < m_teams; i++)
        {
            for (int j = 0; j < m_teams; j++)
            {
                objective += m_distance[i][j] * y[t][i][j];
            }
        }
    }

    for (int i = 0; i < m_teams; i++)
    {
        for (int j = 0; j < m_teams; j++)
        {
            objective += m_distance[j][i] * x[i][j][m_rounds - 1];
        }
    }

    model.add(IloMinimize(env, objective));
    objective.end();

    /*Set cplex parameters*/
    setParams(cplex);
    cplex.exportModel("model.lp");

    /*Solve*/
    cplex.solve();

    mat2i sol = convert(x, cplex);

    cplex.end();
    model.end();
    env.end();
    return sol;
}

void IPSolver::setParams(IloCplex& cplex)
{
// print every x-th line of node-log and give more details
    cplex.setParam(IloCplex::MIPInterval, 1000);
    cplex.setParam(IloCplex::MIPDisplay, 2);
    cplex.setWarning(cplex.getEnv().getNullStream());
    cplex.setParam(IloCplex::Threads, 8);
    cplex.setParam(IloCplex::TiLim, 60);
}

mat2i IPSolver::convert(const IloIntVar3& x, const IloCplex& cplex)
{
    mat2i sol;
    Common::initSolution(m_teams, sol);

    for (int r = 0; r < m_rounds; r++)
    {
        for (int i = 0; i < m_teams; i++)
        {
            for (int j = 0; j < m_teams; j++)
            {
                if (cplex.getIntValue(x[i][j][r]) == 1)
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

void IPSolver::populatePartialSolution(const mat2i& sol, const IloIntVar3& x, const IloIntVar3& z, IloModel& model)
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
                if (o < 0)
                {
                    o = -o;
                    model.add(z[t][o - 1][r] == 1);
                    model.add(x[t][o - 1][r] == 1);
                } else
                {
                    model.add(z[t][t][r] == 1);
                    model.add(x[o - 1][t][r] == 1);
                }
            }
        }
    }
}
