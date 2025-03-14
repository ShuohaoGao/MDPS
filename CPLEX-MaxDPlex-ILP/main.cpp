#include "Graph.h"
#include "ilcplex/ilocplex.h"

using namespace std;

string graph_file_path;
string vertices_path;
Graph g;
int paramK, paramL;
set<ui> ground_truth;

void IntegerLinearProgram(int lb = 0)
{
    int n = g.n;
    IloEnv env;
    try
    {
        IloModel model(env);
        IloIntVarArray var_vertices(env, n, 0, 1); // variable of vertices, with values in {0, 1}

        // constrains
        // if v in DPlex, var_vertices[v]=1 and must satisfy:
        // 1. sum_{for u not in out-neighbor[v]} var_vertices[u] <= k
        // 2. sum_{for u not in in-neighbor[v]} var_vertices[u] <= l
        // if v not in DPlex, var_vertices[v]=0 and we destroy the above constrains (i.e., we make them always stand when v not in DPlex):
        // 1. sum_{for u not in out-neighbor[v]} var_vertices[u] - n <= k
        // 2. sum_{for u not in in-neighbor[v]} var_vertices[u] - n <= l

        // In summary
        // 1. sum_{for u not in out-neighbor[v]} var_vertices[u] - n * (1-var_vertices[v]) <= k
        // 2. sum_{for u not in in-neighbor[v]} var_vertices[u] - n * (1-var_vertices[v]) <= l
        for (ui v = 0; v < n; v++)
        {
            IloExpr non_out_neighbor_cnt(env);
            IloExpr non_in_neighbor_cnt(env);
            for (ui u = 0; u < n; u++)
            {
                if (!g.exist_edge(v, u))
                    non_out_neighbor_cnt += var_vertices[u];

                if (!g.exist_edge(u, v))
                    non_in_neighbor_cnt += var_vertices[u];
            }
            non_out_neighbor_cnt -= n * (1 - var_vertices[v]);
            non_in_neighbor_cnt -= n * (1 - var_vertices[v]);
            model.add(non_out_neighbor_cnt <= paramK);
            model.add(non_in_neighbor_cnt <= paramL);
        }

        IloExpr size_DPlex(env);
        for (int i = 0; i < n; ++i)
        {
            size_DPlex += var_vertices[i];
        }
        // constrain using lb
        model.add(size_DPlex > lb);

        // query vertices
        for (ui u : g.query_vertices)
        {
            model.add(var_vertices[u] == 1);
        }

        // Final objective: max DPlex size
        model.add(IloMaximize(env, size_DPlex));
        size_DPlex.end();

        IloCplex cplex(model);
        cplex.setOut(env.getNullStream()); // disable log

        // 添加以下代码以强制使用单线程
        cplex.setParam(IloCplex::Threads, 1); // 设置线程数为 1

        if (!cplex.solve())
        {
            printf("MaxDPlex was found during DPHeuris\n");
            env.end();
            return;
        }

        set<ui> max_solution;
        for (ui i = 0; i < n; i++)
        {
            if (cplex.getValue(var_vertices[i]) > 0.5)
            {
                max_solution.insert(i);
            }
        }
        for (ui i : max_solution)
            ground_truth.insert(g.vertices[i].origin_id);
        assert(ground_truth.size() == max_solution.size());
    }
    catch (IloException &e)
    {
        cout << "Concert exception caught: " << e << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception caught" << endl;
    }
    env.end();
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("4 params are required: $./ILP graph_path k l vertex_set_file_path !!! \n");
        exit(1);
    }
    graph_file_path = string(argv[1]);
    paramK = atoi(argv[2]);
    paramL = atoi(argv[3]);
    if (argc > 4)
        vertices_path = string(argv[4]);
    else
        vertices_path = "";
    int lb = g.readFromFile(graph_file_path, vertices_path);

    Timer tot_time;

    // we just need to consider k<l
    if (paramK > paramL)
    {
        swap(paramK, paramL);
        g.flip();
    }

    IntegerLinearProgram(lb);
    lb = max(lb, (int)ground_truth.size());

    printf("lb= %d use-time: %.4lf s, ground-truth:\n", lb, tot_time.get_time_seconds());
    if (ground_truth.size())
        print_set(ground_truth);
    fflush(stdout);

    return 0;
}