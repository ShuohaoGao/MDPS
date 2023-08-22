#include "Graph.h"
#include "Reduction.h"
#include "Extend_Heuristic.h"
#include "../Verifier/Verifier.h"

Graph g;

ll program_start_time;
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
    printf("first_lb= %d , first_time= %.6lf s\n", first_round_lb, first_round_time);
    printf("lb= %d , tot_time= %.6lf s, iteration= %d , n= %u , m= %u\n",
           lb, (get_system_time_microsecond() - program_start_time) / 1e6, iteration_cnt, g.n, g.m);
    printf("heuristic solution:\n");
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
    }
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
    g.readFromFile(file_path);
    program_start_time = get_system_time_microsecond();
    ll start_time = get_system_time_microsecond();
    lb = g.getLB_multi_degeneracy(&heur_solution);
    printf("LB by degeneracy order: %u , n= %u , m= %u , ", lb, g.n, g.m);
    first_round_lb = lb;
    ll tot_time = get_system_time_microsecond() - start_time;
    first_round_time = tot_time / 1e6;
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
    tot_time = get_system_time_microsecond() - start_time;
    first_round_time += tot_time / 1e6;
    printf("use time: %.4lf s\n", tot_time / 1e6);
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
    tot_time = get_system_time_microsecond() - start_time;
    printf("add vertex use time: %.4lf s\n", tot_time / 1e6);
#ifdef BRUTE_EXTEND
    printf("extend_time= %.4lf s\n", tot_time / 1e6);
#endif
    print_result();
    char out_file[100];
    sprintf(out_file, "../reduced_graph/%s_K=%d_L=%d.txt", get_file_name_without_suffix(file_path).c_str(), paramK, paramL);
    g.dump_to_file(string(out_file));

    return 0;
}