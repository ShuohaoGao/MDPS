#include "Graph.h"
#include "Reduction.h"
#include "Extend_Heuristic.h"
#include "../Verifier/Verifier.h"

Graph g;

double program_time;
int iteration_cnt;
int first_round_lb;
double first_round_time;
set<int> heur_solution;
string input_file;
void print_result()
{
    if (g.n <= lb)
        g.n = g.m = 0;
    puts("****Heuristic result****");
    printf("fast_lb= %d , fast_time= %.6lf s\n", first_round_lb, first_round_time);
    printf("lb= %d , tot_time= %.6lf s, iteration= %d\n", lb, program_time, iteration_cnt);
    printf("reduced graph: n= %u , m= %u\n", g.n, g.m);
    printf("heuristic solution (size = %d):\n", (int)heur_solution.size());
    assert(heur_solution.size() == lb);
    print_set(heur_solution);
    puts("");
    fflush(stdout);
#ifdef VERIFY
    Verifier s(heur_solution, input_file);
    bool ok=s.is_correct(paramK, paramL);
    if(!ok)
    {
        pii h=s.get();
        printf("is a (%d,%d)-plex\n",h.x,h.y);
        printf("%s K=%d L=%d return a wrong solution!!!!\n\n",(get_file_name_without_suffix(input_file)).c_str(),paramK, paramL);
    }else printf("the solution is checked, and we verify its correctness!\n");
#endif
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("4 params are required !!! \n");
        exit(1);
    }
    string file_path(argv[1]);
    input_file = file_path;
    paramK = atoi(argv[2]);
    paramL = atoi(argv[3]);
    first_round_time=0;
    ll program_start_time=get_system_time_microsecond();
    g.readFromFile(file_path, first_round_time, &heur_solution);
    program_time=first_round_time;
    first_round_lb = lb;
    if (g.n <= lb)
    {
        print_result();
        char out_file[100];
        sprintf(out_file, "../reduced_graph/%s_K=%d_L=%d.txt", get_file_name_without_suffix(file_path).c_str(), paramK, paramL);
        g.dump_to_file(string(out_file));
        return 0;
    }

    ll start_time = get_system_time_microsecond();
    lb = max(lb, (int)g.getLB_multi_degeneracy(&heur_solution));
    printf("FastHeuris: lb= %u , n= %u , m= %u , ", lb, g.n, g.m);
    first_round_lb = lb;
    first_round_time += (get_system_time_microsecond() - start_time)/ 1e6;
    printf("use time: %.4lf s\n", first_round_time);
    iteration_cnt++;
    if (g.n <= lb)
    {
        print_result();
        char out_file[100];
        sprintf(out_file, "../reduced_graph/%s_K=%d_L=%d.txt", get_file_name_without_suffix(file_path).c_str(), paramK, paramL);
        g.dump_to_file(string(out_file));
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
    double tot_time = (get_system_time_microsecond() - start_time)/1e6;
    first_round_time += tot_time;
    printf("first strong reduction use time: %.4lf s\n", tot_time);
    program_time=first_round_time;
    if (g.n <= lb)
    {
        print_result();
        char out_file[100];
        sprintf(out_file, "../reduced_graph/%s_K=%d_L=%d.txt", get_file_name_without_suffix(file_path).c_str(), paramK, paramL);
        g.dump_to_file(string(out_file));
        return 0;
    }

    Heuristic_add heur;
    start_time = get_system_time_microsecond();
    do
    {
        iteration_cnt++;
        double time_limit = get_system_time_microsecond() - program_start_time;
        time_limit *= 2;
        int extend_lb = heur.get_lower_bound(g.n, g.m, g.initialEdges, paramK, paramL, time_limit);
        if (extend_lb <= lb)
            break;
        lb = extend_lb;
        heur_solution.clear();
        for (int u : heur.heur_solution)
            heur_solution.insert(g.map_refresh_id[u]);
        g.weak_reduce(true);
        Reduction reduce(&g);
        reduce.strong_reduce();
        g.refresh();
        printf("cur_lb= %d , after strong reduce: n= %u , m= %u\n", lb, g.n, g.m);
        if (g.n <= lb)
            break;
    } while (1);
    tot_time = (get_system_time_microsecond() - start_time)/1e6;
    printf("StrongHeuris use time: %.4lf s\n", tot_time);
#ifdef BRUTE_EXTEND
    printf("extend_time= %.4lf s\n", tot_time);
#endif
    program_time = first_round_time + tot_time;
    print_result();
    char out_file[100];
    sprintf(out_file, "../reduced_graph/%s_K=%d_L=%d.txt", get_file_name_without_suffix(file_path).c_str(), paramK, paramL);
    g.dump_to_file(string(out_file));

    return 0;
}