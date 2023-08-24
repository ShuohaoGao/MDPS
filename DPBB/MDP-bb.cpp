#include "Branch.h"
#include "../Verifier/Verifier.h"

double total_time;

void print_result()
{
    printf("ground truth= %d\n", lb);
    printf("use time : %.4lf s\n\n", total_time / 1e6);
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
    unordered_map<int,int> id_map;//from the reduced graph to initial graph
    g.readFromFile(file_path, &id_map);
    printf("reduced graph: n=%d lb= %d\n", g.get_n(), lb);
    double start_clock = clock();
    double start_time = get_system_time_microsecond();
    Search DPBB(&g, &id_map);
    set<int> best_solution = DPBB.divide_and_conquer();
    total_time = get_system_time_microsecond() - start_time;
    print_result();
#ifdef VERIFY
    if(best_solution.size())
    {
        string file=get_file_name_without_suffix(file_path);
        string ori_file="../../../pre/data/";
        for(ui i=0;i+8<file.size();i++)
            ori_file+=file[i];
        ori_file += ".out";
        Verifier s(best_solution, ori_file);
        bool ok=s.is_correct(paramK, paramL);
        if(!ok)
        {
            pii h=s.get();
            printf("is a (%d,%d)-plex\n",h.x,h.y);
            printf("%s K=%d L=%d return a wrong solution!!!!\n\n",(get_file_name_without_suffix(ori_file)).c_str(),paramK, paramL);
        }
    }
#endif
    exit(0);
    // printf("use time: %.2lf\n\n", (clock() - start_clock) / CLOCKS_PER_SEC);
    return 0;
}