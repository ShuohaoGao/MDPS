#include "Graph.h"
#include "gurobi_c++.h"

using namespace std;

string graph_file_path;
string vertices_path;
Graph g;
int paramK, paramL;
set<ui> ground_truth;

void IntegerLinearProgram(int lb = 0)
{
    int n = g.n;
    GRBEnv env;
    env.set(GRB_IntParam_OutputFlag, 0); // 禁用日志输出
    try
    {
        GRBModel model(env);

        // 创建变量：顶点是否在 DPlex 中 (0 或 1)
        vector<GRBVar> var_vertices(n);
        for (ui v = 0; v < n; ++v)
        {
            var_vertices[v] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY, "x_" + to_string(v));
        }
        model.update();

        // 添加约束：
        // 1. sum_{for u not in out-neighbor[v]} var_vertices[u] - n * (1-var_vertices[v]) <= k
        // 2. sum_{for u not in in-neighbor[v]} var_vertices[u] - n * (1-var_vertices[v]) <= l
        for (ui v = 0; v < n; ++v)
        {
            GRBLinExpr non_out_neighbor_cnt = 0;
            GRBLinExpr non_in_neighbor_cnt = 0;

            for (ui u = 0; u < n; ++u)
            {
                if (!g.exist_edge(v, u))
                {
                    non_out_neighbor_cnt += var_vertices[u];
                }

                if (!g.exist_edge(u, v))
                {
                    non_in_neighbor_cnt += var_vertices[u];
                }
            }

            non_out_neighbor_cnt -= n * (1 - var_vertices[v]);
            non_in_neighbor_cnt -= n * (1 - var_vertices[v]);

            model.addConstr(non_out_neighbor_cnt <= paramK, "out_neighbor_constraint_" + to_string(v));
            model.addConstr(non_in_neighbor_cnt <= paramL, "in_neighbor_constraint_" + to_string(v));
        }

        // 添加目标函数：最大化 DPlex 的大小
        GRBLinExpr size_DPlex = 0;
        for (ui i = 0; i < n; ++i)
        {
            size_DPlex += var_vertices[i];
        }

        // 添加约束：DPlex 的大小 > lb
        model.addConstr(size_DPlex >= lb + 1, "size_constraint");

        // 查询顶点约束：查询顶点必须在 DPlex 中
        for (ui u : g.query_vertices)
        {
            model.addConstr(var_vertices[u] == 1, "query_vertex_constraint_" + to_string(u));
        }

        // 设置目标函数：最大化 DPlex 的大小
        model.setObjective(size_DPlex, GRB_MAXIMIZE);

        // 求解模型
        model.optimize();

        // 检查模型状态
        int status = model.get(GRB_IntAttr_Status);
        if (status == GRB_INFEASIBLE || status == GRB_INF_OR_UNBD)
        {
            printf("MaxDPlex was found during DPHeuris\n");
            return;
        }

        // 获取解并更新 ground_truth
        set<ui> max_solution;
        for (ui i = 0; i < n; ++i)
        {
            if (var_vertices[i].get(GRB_DoubleAttr_X) > 0.5)
            {
                max_solution.insert(i);
            }
        }

        for (ui i : max_solution)
        {
            ground_truth.insert(g.vertices[i].origin_id);
        }

        assert(ground_truth.size() == max_solution.size());
    }
    catch (GRBException &e)
    {
        cout << "Gurobi exception caught: " << e.getMessage() << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception caught" << endl;
    }
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

    // 我们只需要考虑 k < l 的情况
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