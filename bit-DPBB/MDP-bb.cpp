#include "Branch.h"

double total_time;
int paramK, paramL;
int lb;
int origin_lb;
set<ui> ground_truth;

void print_result()
{
    printf("ground truth= %d\n", lb);
    printf("use time : %.4lf s\n\n", total_time / 1e6);
    if (ground_truth.size() > origin_lb)
    {
        printf("ground-truth:");
        print_set(ground_truth);
    }
    else
    {
        printf("MaxDPlex was found during DPHeuris\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("4 params are required !!! \n");
        exit(1);
    }
    string file_path(argv[1]);
    paramK = atoi(argv[2]);
    paramL = atoi(argv[3]);
    Graph g;
    lb = g.readFromFile(file_path);
    origin_lb = lb;
    lb = max(lb, paramK + paramL - 2);
    printf("reduced graph: n=%d lb= %d\n", g.n, lb);
    double start_time = get_system_time_microsecond();
    Branch bnb(g, paramK, paramL, lb, &ground_truth);
    bnb.run();
    lb = max(lb, (int)ground_truth.size());
    total_time = get_system_time_microsecond() - start_time;
    print_result();
    return 0;
}