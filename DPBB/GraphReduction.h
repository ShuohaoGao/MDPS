#ifndef GRAPHREDUCTION_H
#define GRAPHREDUCTION_H

#include "../DPHeuris/Undirected_Graph.h"
#include "Graph.h"

class GraphReduction
{
private:
    /*
        assume u<v
        common_neighbor_cnt[u][v] == |N_out(u) \cap N_out(v)|
        common_neighbor_cnt[v][u] == |N_in(u) \cap N_in(v)|
    */
    vector<vector<int>> common_neighbor_cnt;
    // out_in_neighbor_cnt[u][v] == |N_out(u) \cap N_in(v)|
    vector<vector<int>> out_in_neighbor_cnt;
    // state[a][b]==1  <=> a,b can not occur in a larger plex with size > lb
    vector<vector<bool>> state;

    int new_rule_cnt;
    Map *common_out_neighbor_cnt, *common_in_neighbor_cnt;
    vector<pii> undirected_edges; // undirected
    Graph *g;
    ui n, m;
    Undirected_Graph *ug;
    int *q;
    int tt;
    inline void process_triangle(int a, int b, int c)
    {
        bool ab = g->exist_edge(a, b);
        bool ba = g->exist_edge(b, a);
        bool ac = g->exist_edge(a, c);
        bool ca = g->exist_edge(c, a);
        bool bc = g->exist_edge(b, c);
        bool cb = g->exist_edge(c, b);
        bool ok = (ab || ba) && (ac || ca) && (bc || cb);
        if (!ok)
        {
            printf("wrong triangle %d %d %d\n", a, b, c);
            return;
        }
        // a b is bottom edge
        if (ac && bc)
            common_out_neighbor_cnt->increment({a, b});
        if (ca && cb)
            common_in_neighbor_cnt->increment({a, b});
        // a c is bottom edge
        if (ab && cb)
            common_out_neighbor_cnt->increment({a, c});
        if (ba && bc)
            common_in_neighbor_cnt->increment({a, c});
        // b c is bottom edge
        if (ca && ba)
            common_out_neighbor_cnt->increment({c, b});
        if (ab && ac)
            common_in_neighbor_cnt->increment({c, b});
    }
    // a b is bottom edge，deal with more than one triangles each invoking( more efficient )
    void process(int a, int b)
    {
        bool ab = g->exist_edge(a, b);
        bool ba = g->exist_edge(b, a);
        for (int i = 0; i <= tt; i++)
        {
            int c = q[i];
            bool ac = g->exist_edge(a, c);
            bool ca = g->exist_edge(c, a);
            bool bc = g->exist_edge(b, c);
            bool cb = g->exist_edge(c, b);
            // a b is bottom edge
            if (ac && bc)
                common_out_neighbor_cnt->increment({a, b});
            if (ca && cb)
                common_in_neighbor_cnt->increment({a, b});
            // a c is bottom edge
            if (ab && cb)
                common_out_neighbor_cnt->increment({a, c});
            if (ba && bc)
                common_in_neighbor_cnt->increment({a, c});
            // b c is bottom edge
            if (ca && ba)
                common_out_neighbor_cnt->increment({c, b});
            if (ab && ac)
                common_in_neighbor_cnt->increment({c, b});
        }
    }

    void list_triangles()
    {
        q = new int[n];
        for (ui I = 0; I < n; I++)
        {
            ui u = ug->get_min_degree_vertex();
            vector<ui> nei = ug->neighbors(u);
            for (ui i : nei)
            {
                tt = -1;
                for (ui j : nei)
                {
                    if (j == i)
                        break;
                    if (ug->has(j, i))
                        q[++tt] = j;
                }
                process(u, i);
            }
            ug->remove_vertex(u);
        }
        delete[] q;
        for(int u:g->vertex)
        {
            for(int a:g->neighbors_out(u))
            {
                for(int b:g->neighbors_out(u))
                {
                    if(!g->exist_edge(a,b) && !g->exist_edge(b,a))
                    {
                        common_in_neighbor_cnt->increment({a,b});
                    }
                }
            }
            for(int a:g->neighbors_in(u))
            {
                for(int b:g->neighbors_in(u))
                {
                    if(!g->exist_edge(a,b) && !g->exist_edge(b,a))
                    {
                        common_out_neighbor_cnt->increment({a,b});
                    }
                }
            }
        }
    }

    void init()
    {
        n = g->get_n();
        undirected_edges.resize(m * 2);
        // treat directed edges as undirected edges
        ui idx = 0;
        for (int u : g->vertex)
            for (int v : g->neighbors_out(u))
            {
                undirected_edges[idx++] = {u, v};
                undirected_edges[idx++] = {v, u};
            }
        // sort and unique
        unique_pii(undirected_edges, n);
        m = undirected_edges.size();
        if (ug != nullptr)
            delete ug;
        ug = new Undirected_Graph(n, undirected_edges);
    }
    void compute_neighbor_cnt()
    {
        common_neighbor_cnt.clear();
        common_neighbor_cnt.resize(n);
        for (auto &s : common_neighbor_cnt)
            s.resize(n,0);
        out_in_neighbor_cnt.clear();
        out_in_neighbor_cnt.resize(n);
        for (auto &s : out_in_neighbor_cnt)
            s.resize(n,0);
        for (int u : g->vertex)
        {
            vector<ui> out = g->neighbors_out(u);
            vector<ui> in = g->neighbors_in(u);
            // u is the common in-neighbor of a,b
            for (int a : out)
                for (int b : out)
                {
                    if (b == a)
                        break;
                    else if (a < b)
                        common_neighbor_cnt[b][a]++;
                    else // a>b
                        common_neighbor_cnt[a][b]++;
                }
            // u is the common out-neighbor of a,b
            for (int a : in)
                for (int b : in)
                {
                    if (b == a)
                        break;
                    else if (a < b)
                        common_neighbor_cnt[a][b]++;
                    else // b<a
                        common_neighbor_cnt[b][a]++;
                }
            // b-->u-->a
            for (int a : out)
                for (int b : in)
                {
                    if (b == a)
                        continue;
                    out_in_neighbor_cnt[b][a]++;
                }
        }
    }
    void update_state()
    {
        for (int a : g->vertex)
            for (int b : g->vertex)
            {
                if (b == a)
                    break;
                else if (a > b)
                    swap(a, b);
                int edge_cnt = (g->exist_edge(a, b) ? 1 : 0) + (g->exist_edge(b, a) ? 1 : 0);
                // common out-neighbors of a,b
                if (common_neighbor_cnt[a][b] <= lb - 2 * paramK + 2 - edge_cnt)
                {
                    state[b][a]=state[a][b] = 1;
                    continue;
                }
                // common in-neighbors of a,b
                if (common_neighbor_cnt[b][a] <= lb - 2 * paramL + 2 - edge_cnt)
                {
                    state[b][a]=state[a][b] = 1;
                    continue;
                }
                if (out_in_neighbor_cnt[a][b] <= lb - paramK - paramL + 2 - edge_cnt || out_in_neighbor_cnt[b][a] <= lb - paramK - paramL + 2 - edge_cnt)
                {
                    state[b][a]=state[a][b] = 1;
                }
            }
    }
public:
    double graph_reduction_time;
    GraphReduction(Graph *_g)
    {
        g = _g;
        n = g->get_n();
        m = g->get_m();
        common_in_neighbor_cnt = get_suitable_map(n, m);
        common_out_neighbor_cnt = get_suitable_map(n, m);
        ug = nullptr;
        init();
        list_triangles();
        double st_time = get_system_time_microsecond();
        state.resize(n);
        for (auto &s : state)
            s.resize(n);
        compute_neighbor_cnt();
        update_state();
        graph_reduction_time = get_system_time_microsecond() - st_time;
    }
    ~GraphReduction()
    {
        delete common_in_neighbor_cnt;
        delete common_out_neighbor_cnt;
    }
    vector<vector<bool>>* get_state()
    {
        return &state;
    }
    void weak_reduce()
    {
        int *q=new int[g->N];
        vector<bool> vis(g->N);
        int hh=0,tt=-1;
        for(int u:g->vertex)
        {
            if(g->get_pd(u)<=lb)
            {
                q[++tt]=u;
                vis[u]=1;
            }
        }
        while(hh<=tt)
        {
            int u=q[hh++];
            for(int v:g->neighbors_out(u))
            {
                if(!vis[v] && g->get_in_degree(v)-1+paramL <= lb)
                {
                    vis[v]=1;
                    q[++tt]=v;
                }
            }
            for(int v:g->neighbors_in(u))
            {
                if(!vis[v] && g->get_out_degree(v)-1+paramK <= lb)
                {
                    vis[v]=1;
                    q[++tt]=v;
                }
            }
            g->remove_vertex_forever(u);
        }
    }
    void strong_reduce()
    {
        // cout<<"before reduce n= "<<g->get_n()<<endl;
        double st_time = get_system_time_microsecond();
        weak_reduce();
        Queue *q_e = new Queue(g->N * g->N); // queue of edges to be removed
        queue<int> q_v;                      // queue of vertices to be removed
        // csapp improvement
        int lb_2k = lb - 2 * paramK;
        int lb_2l = lb - 2 * paramL;
        int lb_2k1 = lb_2k + 1;
        int lb_2l1 = lb_2l + 1;
        for (int u : g->vertex)
        {
            for (int v : g->neighbors_out(u))
            {
                if (g->exist_edge(v, u))
                {
                    if (common_out_neighbor_cnt->get({u, v}) <= lb_2k || common_in_neighbor_cnt->get({u, v}) <= lb_2l)
                    {
                        q_e->push(g->get_edge_id(u, v));
                    }
                }
                else
                {
                    if (common_out_neighbor_cnt->get({u, v}) <= lb_2k1 || common_in_neighbor_cnt->get({u, v}) <= lb_2l1)
                    {
                        q_e->push(g->get_edge_id(u, v));
                    }
                }
            }
        }
        bool *v_remove = new bool[n];
        memset(v_remove, 0, sizeof(bool) * n);
        while (q_v.size() || q_e->size())
        {
            while (q_e->size())
            {
                ui edge_id = q_e->front();
                pii temp=g->get_edge_by_id(edge_id);
                q_e->pop();
                int from = temp.x, to = temp.y;
                g->remove_edge_forever(from, to);
                if (g->get_pd(from) <= lb && !v_remove[from])
                {
                    q_v.push(from);
                    v_remove[from] = 1;
                }
                if (g->get_pd(to) <= lb && !v_remove[to])
                {
                    q_v.push(to);
                    v_remove[to] = 1;
                }
                int from_cnt = g->get_in_degree(from) + g->get_out_degree(from);
                int to_cnt = g->get_in_degree(to) + g->get_out_degree(to);
                if (from_cnt <= to_cnt)
                {
                    for (int w : g->neighbors_out(from))
                    {
                        if (v_remove[w])
                            continue;
                        bool w_to = g->exist_edge(w, to);
                        bool to_w = g->exist_edge(to, w);
                        if (!w_to && !to_w)
                            continue;
                        bool from_w = true;
                        bool w_from = g->exist_edge(w, from);
                        int val_now = common_in_neighbor_cnt->reduce({w, to});
                        if (w_to && to_w)
                        {
                            if (val_now <= lb_2l)
                            {
                                q_e->push(g->get_edge_id(to, w));
                                q_e->push(g->get_edge_id(w, to));
                            }
                        }
                        else
                        {
                            if (val_now <= lb_2l1)
                            {
                                if (w_to)
                                {
                                    q_e->push(g->get_edge_id(w, to));
                                }
                                else
                                {
                                    q_e->push(g->get_edge_id(to, w));
                                }
                            }
                        }
                        if (w_to)
                        {
                            int val = common_out_neighbor_cnt->reduce({from, w});
                            if (w_from)
                            {
                                if (val <= lb_2k)
                                {
                                    q_e->push(g->get_edge_id(from, w));
                                    q_e->push(g->get_edge_id(w, from));
                                }
                            }
                            else
                            {
                                if (val <= lb_2k1)
                                {
                                    q_e->push(g->get_edge_id(from, w));
                                }
                            }
                        }
                    }
                    for (int w : g->neighbors_in(from))
                    {
                        if (v_remove[w] || !g->exist_edge(w, to))
                            continue;
                        if (g->exist_edge(from, w))
                            continue; // we only need Unidirectional edge
                        int val = common_out_neighbor_cnt->reduce({from, w});
                        if (val <= lb_2k1)
                        {
                            q_e->push(g->get_edge_id(w, from));
                        }
                    }
                }
                else
                {
                    for (int w : g->neighbors_in(to))
                    {
                        if (v_remove[w])
                            continue;
                        bool w_from = g->exist_edge(w, from);
                        bool from_w = g->exist_edge(from, w);
                        if (!w_from && !from_w)
                            continue;
                        bool w_to = true;
                        bool to_w = g->exist_edge(to, w);
                        int val_now = common_out_neighbor_cnt->reduce({w, from});
                        if (w_from && from_w)
                        {
                            if (val_now <= lb_2k)
                            {
                                q_e->push(g->get_edge_id(from, w));
                                q_e->push(g->get_edge_id(w, from));
                            }
                        }
                        else
                        {
                            if (val_now <= lb_2k1)
                            {
                                if (w_from)
                                {
                                    q_e->push(g->get_edge_id(w, from));
                                }
                                else
                                {
                                    q_e->push(g->get_edge_id(from, w));
                                }
                            }
                        }
                        if (from_w)
                        {
                            int val = common_in_neighbor_cnt->reduce({to, w});
                            if (to_w)
                            {
                                if (val <= lb_2l)
                                {
                                    q_e->push(g->get_edge_id(to, w));
                                    q_e->push(g->get_edge_id(w, to));
                                }
                            }
                            else
                            {
                                if (val <= lb_2l1)
                                {
                                    q_e->push(g->get_edge_id(w, to));
                                }
                            }
                        }
                    }
                    for (int w : g->neighbors_out(to))
                    {
                        if (v_remove[w] || !g->exist_edge(from, w))
                            continue;
                        if (g->exist_edge(w, to))
                            continue; // Unidirectional edge
                        int val = common_in_neighbor_cnt->reduce({to, w});
                        if (val <= lb_2l1)
                        {
                            q_e->push(g->get_edge_id(to, w));
                        }
                    }
                }
            }
            if (q_v.size())
            {
                int u = q_v.front();
                q_v.pop();
                for (int to : g->neighbors_out(u))
                {
                    if (g->get_in_degree(to) - 1 <= lb - paramL && !v_remove[to])
                    {
                        q_v.push(to);
                        v_remove[to] = 1;
                    }
                }

                for (int from : g->neighbors_in(u))
                {
                    if (g->get_out_degree(from) - 1 <= lb - paramK && !v_remove[from])
                    {
                        q_v.push(from);
                        v_remove[from] = 1;
                    }
                }
                for (int v : g->neighbors_out(u))
                {
                    if (v_remove[v])
                        continue;
                    for (int w : g->neighbors_out(u))
                    {
                        if (v_remove[w])
                            continue;
                        if (w == v)
                            break;
                        bool vw = g->exist_edge(v, w);
                        bool wv = g->exist_edge(w, v);
                        if (vw || wv)
                        {
                            int val = common_in_neighbor_cnt->reduce({v, w});
                            if (wv && vw)
                            {
                                if (val <= lb_2l)
                                {
                                    q_e->push(g->get_edge_id(w, v));
                                    q_e->push(g->get_edge_id(v, w));
                                }
                            }
                            else if (wv)
                            {
                                if (val <= lb_2l1)
                                    q_e->push(g->get_edge_id(w, v));
                            }
                            else
                            {
                                if (val <= lb_2l1)
                                    q_e->push(g->get_edge_id(v, w));
                            }
                        }
                    }
                }
                for (int v : g->neighbors_in(u))
                {
                    if (v_remove[v])
                        continue;
                    for (int w : g->neighbors_in(u))
                    {
                        if (v_remove[w])
                            continue;
                        if (w == v)
                            break;
                        bool vw = g->exist_edge(v, w);
                        bool wv = g->exist_edge(w, v);
                        if (vw || wv)
                        {
                            int val = common_out_neighbor_cnt->reduce({v, w});
                            if (wv && vw)
                            {
                                if (val <= lb_2k)
                                {
                                    q_e->push(g->get_edge_id(w, v));
                                    q_e->push(g->get_edge_id(v, w));
                                }
                            }
                            else if (wv)
                            {
                                if (val <= lb_2k1)
                                    q_e->push(g->get_edge_id(w, v));
                            }
                            else
                            {
                                if (val <= lb_2k1)
                                    q_e->push(g->get_edge_id(v, w));
                            }
                        }
                    }
                }
                g->remove_vertex_forever(u);
            }
        }
        delete q_e;
        compute_neighbor_cnt();
        update_state();
        // cout<<"cur n= "<<g->get_n()<<endl;
        graph_reduction_time += get_system_time_microsecond() - st_time;
    }
};

#endif