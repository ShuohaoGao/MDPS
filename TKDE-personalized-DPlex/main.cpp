#include "Graph.h"
#include "Branch.h"

string graph_file_path;
string vertices_path;
Graph g;
int paramK, paramL;
set<ui> ground_truth;

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("4 params are required: $./dq graph_path k l vertex_set_file_path !!! \n");
        exit(1);
    }
    graph_file_path = string(argv[1]);
    paramK = atoi(argv[2]);
    paramL = atoi(argv[3]);
    vertices_path = string(argv[4]);
    g.readFromFile(graph_file_path, vertices_path);
    // Graph backup = g;

    Timer tot_time;

    // we just need to consider k<l
    if (paramK > paramL)
    {
        swap(paramK, paramL);
        g.flip();
    }

    if (!g.is_DPlex(g.query_vertices, paramK, paramL))
    {
        printf("The given vertex set is not a DPlex!!\n");
        printf("after preprocessing, n= 0 m= 0 lb= 0 use-time: %.4lf s\n", tot_time.get_time_seconds());
        puts("---------------{DPBB-Q results}----------------");
        printf("lb= 0 use-time: %.4lf s, ground-truth:\n", tot_time.get_time_seconds());
        exit(0);
    }

    puts("---------------{preprocessing stage}----------------");
    fflush(stdout);
    g.heuristic_and_preprocessing(ground_truth, paramK, paramL);
    printf("after preprocessing, n= %u m= %u lb= %u use-time: %.4lf s\n", g.n, g.m, ground_truth.size(), tot_time.get_time_seconds());

    int lb = max(paramK + paramL - 2, (int)ground_truth.size());
    if (g.n > lb)
    {
        puts("---------------{branch-and-bound stage}----------------");
        fflush(stdout);
        Branch branch(g, paramK, paramL, &ground_truth);
        branch.run();
        lb = max(paramK + paramL - 2, (int)ground_truth.size());
    }

    puts("---------------{DPBB-Q results}----------------");
    printf("lb= %d use-time: %.4lf s, ground-truth:\n", lb, tot_time.get_time_seconds());
    if (ground_truth.size() == lb)
        print_set(ground_truth);
    else
    {
        assert(ground_truth.size() < lb);
        printf("There is no DPlexes containing Q with at least k+l-1 vertices!!!\n");
    }
    fflush(stdout);

    // // Verify
    // if (lb == ground_truth.size())
    // {
    //     vector<ui> S;
    //     for (ui u : ground_truth)
    //         S.push_back(u);
    //     // S = vector<ui>{2, 3, 4, 6, 57, 58, 59, 63, 137, 252, 281, 305, 408, 412, 520, 685};
    //     assert(backup.is_maximal(S, paramK, paramL));
    //     printf("checked: is a maximal DPlex\n");
    // }

    return 0;
}