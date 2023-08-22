#ifndef BRANCH_H
#define BRANCH_H

#include "Graph.h"
#include "UpperBound.h"
#include "Reduction.h"
#include "GraphReduction.h"

class Search
{
    Graph *g; // this is a subgraph when using DC decompostion( which brings more cache hits)
    Graph backupG;
    Plex *S;
    UpperBound *upperBound;
    GraphReduction *graphReduction;
    FastReduction *fastReduction;
    ll dfs_cnt;
    ll cnt1, cnt2;
    double bounding_time, reducing_time, sel_time;
    double plex_time, strong_reduce_time;
    double induce_time;
    bool need_weak_reduce;
    //(*mutual_exclude)[u][v]=1 --> u,v can occur in a large plex at the same time
    vector<vector<bool>> *mutual_exclude;
    int vertex_just_removed;
    ll v_rm_cnt;
    ll S_size;
    int last_ub;
    vector<int> vertex_id_map_from_backG_to_induced_graph;
    set<int> best_solution;
    unordered_map<int, int> *id_map_from_reduced_to_initial;

public:
    Search(Graph *_g, unordered_map<int, int> *id_map = nullptr)
    {
        g = _g;
        vertex_id_map_from_backG_to_induced_graph.resize(g->N, -1);
        S = new Plex(g->N, g);
        upperBound = new UpperBound(g, S);
        dfs_cnt = 0;
        cnt1 = cnt2 = 0;
        bounding_time = reducing_time = sel_time = plex_time = 0;
        backupG = *g;
        g->backupG = &backupG;
        graphReduction = new GraphReduction(&backupG);
        fastReduction = new FastReduction(S, g);
        need_weak_reduce = induce_time = 0;
        strong_reduce_time = 0;
        mutual_exclude = graphReduction->get_state();
        v_rm_cnt = 0;
        S_size = 0;
        id_map_from_reduced_to_initial = id_map;
    }
    ~Search()
    {
        delete S;
    }
    void print_result()
    {
        printf("dfs_cnt= %d, branc1_cnt= %d, branch2_cnt= %d\n", dfs_cnt, cnt1, cnt2);
        printf("bounding time: %.4lf s, reducing time: %.4lf s, binary time: %.4lf s\n", bounding_time / 1e6, reducing_time / 1e6, upperBound->binary_time / 1e6);
        printf("remove time: %.4lf s, rollback time: %.4lf s\n", g->remove_time / 1e6, g->rollback_time / 1e6);
        printf("sel time: %.4lf s\n", sel_time / 1e6);
        printf("plex time: %.4lf s\n", plex_time / 1e6);
        printf("strong reduce time: %.4lf s\n", graphReduction->graph_reduction_time / 1e6);
        printf("rm v cnt: %lld, rollback v cnt: %lld, S add v cnt: %lld\n", v_rm_cnt, g->rollback_v_cnt, S->add_v_cnt);
        printf("Plex: add time: %.4lf s, remove time: %.4lf s\n", S->plex_add_time / 1e6, S->plex_remove_time / 1e6);
        printf("average S size: %.2lf\n", 1.0 * S_size / dfs_cnt);
        printf("induce time: %.4lf s\n", induce_time / 1e6);
    }
    bool update_lb(int sz)
    {
        if (sz > lb)
        {
            lb = sz;
            best_solution.clear();
            if (sz == g->get_n())
            {
                assert(g->is_plex());
                for (int v : g->vertex)
                    best_solution.insert(g->induced_vertex[v]);
            }
            else
            {
                for (int v : S->vertex)
                    best_solution.insert(g->induced_vertex[v]);
            }
            double start_strong_reduce_time = get_system_time_microsecond();
            graphReduction->strong_reduce();
            strong_reduce_time += get_system_time_microsecond() - start_strong_reduce_time;
            vector<int> need_remove;
            for (int v : g->vertex)
                if (!backupG.has(g->induced_vertex[v]) && !S->has(v))
                    need_remove.push_back(v);
            for (int v : need_remove)
                g->remove_vertex_forever(v);
            return true;
        }
        return false;
    }
    /**
     * DC decomposition (i.e., IE framework)
     * @return the best solution (if empty, then it means heuristic solution is the best one)
    */
    set<int> divide_and_conquer()
    {
        double percentage = 0;
        while (backupG.get_n() > lb)
        {
            // just for beauty
            double percentage = 1.0 - (backupG.get_n() * 1.0 / backupG.N);
            print_progress_bar(percentage);

            ui u = backupG.get_vertex_with_min_pd();
            double start_induce = get_system_time_microsecond();
            backupG.induce(u, g, vertex_id_map_from_backG_to_induced_graph, mutual_exclude);
            induce_time += get_system_time_microsecond() - start_induce;
            int new_u = vertex_id_map_from_backG_to_induced_graph[u];
            assert(g->has(new_u));
            S->add_vertex(new_u);
            vertex_just_removed = -1;
            last_ub = g->get_pd(new_u);
            bnb();
            if (backupG.has(u))
                backupG.remove_vertex_forever(u);
            S->remove_vertex(new_u);
        }
        print_progress_bar(1.0);
#ifndef NO_PROGRESS_BAR
        cout << endl;
#endif
        print_result();
        if (!best_solution.size())
        {
            puts("The heuristic solution is the best solution!");
            set<int> ret;
            return ret;
        }
        else
        {
            printf("The best solution ( size= %u ):\n", best_solution.size());
            assert(id_map_from_reduced_to_initial != nullptr);
            set<int> temp;
            for (int u : best_solution)
            {
                temp.insert((*id_map_from_reduced_to_initial)[u]);
            }
            print_set(temp);
            return temp;
        }
    }
    void bnb()
    {
        dfs_cnt++;
        S_size += S->get_n();

        // Reduction Rules
        vector<int> v_removed;
        vector<ui> edge_removed;
        ll start_reducing_time = get_system_time_microsecond();
        fastReduction->fastReduction(v_removed, edge_removed, need_weak_reduce, mutual_exclude, last_ub);
        need_weak_reduce = 0;
        v_rm_cnt += v_removed.size();
        reducing_time += get_system_time_microsecond() - start_reducing_time;
        if (g->get_n() <= lb)
        {
            g->rollback(v_removed, edge_removed);
            return;
        }
        for (int u : v_removed)
            if (S->has(u))
            {
                g->rollback(v_removed, edge_removed);
                return;
            }

        // Bounding Method
        ll start_bounding_time = get_system_time_microsecond();
        int ub = g->get_n();
        if (vertex_just_removed != -1)
        {
            ub = min(ub, upperBound->get_old_upper_bound(vertex_just_removed, v_removed));
        }
        else
            ub = min(ub, upperBound->get_upper_bound());
        bounding_time += get_system_time_microsecond() - start_bounding_time;
        if (ub <= lb)
        {
            g->rollback(v_removed, edge_removed);
            return;
        }
        last_ub = ub;

        // Branching Rule
        ll start_sel_time = get_system_time_microsecond();
        int u = g->get_vertex_with_min_pd(S->vertex_idx_map);
        if (g->get_n() == lb + 1 && g->get_min_pd() >= g->get_n())
        {
            update_lb(g->get_n());
            // undo the opeartions above
            g->rollback(v_removed, edge_removed);
            return;
        }
        sel_time += get_system_time_microsecond() - start_sel_time;

        // branch 1: remove u from C
        vector<ui> rm_edges;
        g->remove_vertex(u, rm_edges, need_weak_reduce);
        cnt1++;
        int last_lb = lb;
        vertex_just_removed = u;
        bnb();
        // if (backupG.has(u))
        if (backupG.has(g->induced_vertex[u]))
        {
            g->rollback(u, rm_edges);
        }
        else // RAU, u is unpromising
        {
            // undo the opeartions above
            g->rollback(v_removed, edge_removed);
            return;
        }
        if (lb > last_lb)
        {
            if (ub <= lb)
            {
                // undo the opeartions above
                g->rollback(v_removed, edge_removed);
                return;
            }
            // RAU strategy: if S has unpromising vertices, then prune this branch
            for (int v : S->vertex)
            {
                // if (!backupG.has(v))
                if (!backupG.has(g->induced_vertex[v]))
                {
                    // undo the opeartions above
                    g->rollback(v_removed, edge_removed);
                    return;
                }
            }
        }

        // branch 2: include u into S
        ll S_time = get_system_time_microsecond();
        bool ok = S->add_vertex(u);
        plex_time += get_system_time_microsecond() - S_time;
        if (ok)
        {
            if (update_lb(S->get_n()))
            {
                // RAU
                for (int v : S->vertex)
                {
                    // if (!backupG.has(v))
                    if (!backupG.has(g->induced_vertex[v]))
                    {
                        // undo the opeartions above
                        g->rollback(v_removed, edge_removed);
                        S->remove_vertex(u);
                        return;
                    }
                }
            }
            cnt2++;
            vertex_just_removed = -1;
            bnb();
            S_time = get_system_time_microsecond();
            S->remove_vertex(u);
            plex_time += get_system_time_microsecond() - S_time;
        }

        // undo the opeartions above
        g->rollback(v_removed, edge_removed);
    }
};

#endif