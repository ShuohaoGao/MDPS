#include "Graph.h"
#include "Reduction.h"
#include "Extend_Heuristic.h"

Graph g;

ll program_start_time;
int iteration_cnt;
int first_round_lb;
double first_round_time;
void print_result()
{
    if(g.n <= lb)
        g.n=g.m=0;
    puts("****Heuristic result****");
    printf("first_lb= %d , first_time= %.6lf s\n",first_round_lb, first_round_time);
    printf("lb= %d , tot_time= %.6lf s, iteration= %d , n= %u , m= %u\n\n", 
        lb,(get_system_time_microsecond()-program_start_time)/1e6, iteration_cnt, g.n, g.m);
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
    g.readFromFile(file_path);
    program_start_time=get_system_time_microsecond();
    ll start_time = get_system_time_microsecond();
    lb = g.getLB_multi_degeneracy();
    printf("LB by degeneracy order: %u , n= %u , m= %u , ", lb, g.n, g.m);
    first_round_lb=lb;
    ll tot_time = get_system_time_microsecond() - start_time;
    first_round_time=tot_time/1e6;
    printf("use time: %.4lf s\n", first_round_time);
    iteration_cnt++;
    if (g.n <= lb)
    {
        print_result();
        return 0;
    }

    start_time = get_system_time_microsecond();
    {
        g.refresh();
        Reduction graphReduce(&g);
        graphReduce.strong_reduce(true);
        g.refresh();
    }
    printf("cur_lb= %d , after strong reduce: n= %u , m= %u , ", lb, g.n, g.m);
    tot_time = get_system_time_microsecond() - start_time;
    first_round_time+=tot_time/1e6;
    printf("use time: %.4lf s\n", tot_time / 1e6);
    if (g.n <= lb)
    {
        print_result();
        return 0;
    }

    Heuristic_add heur;
    start_time = get_system_time_microsecond();
    do
    {
        iteration_cnt++;
        int extend_lb = heur.get_lower_bound(g.n, g.m, g.initialEdges, paramK, paramL);
        if (extend_lb <= lb)
            break;
        lb = extend_lb;
        g.weak_reduce(true);
        Reduction reduce(&g);
        reduce.strong_reduce();
        g.refresh();
        printf("cur_lb= %d , after strong reduce: n= %u , m= %u\n", lb, g.n, g.m);
        if (g.n <= lb)
            break;
    } while (1);
    tot_time = get_system_time_microsecond() - start_time;
    printf("add vertex use time: %.4lf s\n", tot_time / 1e6);
    #ifdef BRUTE_EXTEND
        printf("extend_time= %.4lf s\n", tot_time/1e6);
    #endif
    print_result();

    return 0;
}