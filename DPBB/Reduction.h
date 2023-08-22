#ifndef REDUCTION_H
#define REDUCTION_H

#include "Graph.h"

class FastReduction
{
    Graph *g;
    Plex *S;
    int *vis;
    int *a;
    bool *in_queue;
    int N;

public:
    FastReduction(Plex *_S, Graph *_g)
    {
        g = _g;
        S = _S;
        N = g->N;
        vis = new int[N];
        memset(vis, 0, sizeof(int) * N);
        a = new int[N];
        memset(a, 0, sizeof(int) * N);
        in_queue = new bool[N];
        memset(in_queue, 0, sizeof(bool) * N);
    }
    ~FastReduction()
    {
        delete[] vis;
        delete[] a;
        delete[] in_queue;
    }
    /**
     * remove {u| pd[u]<=lb}
     */
    void weak_reduce(vector<int> &v_removed, vector<ui> &edge_removed)
    {
        ui n = g->N;
        int hh = v_removed.size();
        int pre_sz = hh;
        for (ui v : g->vertex)
            if (g->get_pd(v) <= lb)
                v_removed.push_back(v), in_queue[v] = 1;
        while (hh < (int)v_removed.size())
        {
            ui u = v_removed[hh++];
            for (ui v : g->neighbors_out(u))
                if (!in_queue[v] && g->get_in_degree(v) - 1 + paramL <= lb)
                {
                    v_removed.push_back(v);
                    in_queue[v] = 1;
                }
            for (ui v : g->neighbors_in(u))
                if (!in_queue[v] && g->get_out_degree(v) - 1 + paramK <= lb)
                {
                    v_removed.push_back(v);
                    in_queue[v] = 1;
                }
            g->remove_vertex(u, edge_removed);
        }
        for (int i = pre_sz; i < (int)v_removed.size(); i++)
            in_queue[v_removed[i]] = 0;
    }

    /**
     * remove {u| dis[u,v]>2}
     */
    void two_hops_in(int v, vector<int> &v_removed, vector<ui> &edge_removed)
    {
        for (int u : g->neighbors_in(v))
        {
            vis[u] = 1;
            for (int j : g->neighbors_in(u))
                vis[j] = 1;
        }
        vis[v] = 1;
        ui pre_sz = v_removed.size();
        for (int u : g->vertex)
            if (!vis[u])
                v_removed.push_back(u);
        for (ui i = pre_sz; i < v_removed.size(); i++)
        {
            g->remove_vertex(v_removed[i], edge_removed);
        }
        for (int u : g->vertex)
            vis[u] = 0;
    }

    /**
     * remove {u| dis[v,u]>2}
     */
    void two_hops_out(int v, vector<int> &v_removed, vector<ui> &edge_removed)
    {
        for (int u : g->neighbors_out(v))
        {
            vis[u] = 1;
            for (int j : g->neighbors_out(u))
                vis[j] = 1;
        }
        vis[v] = 1;
        ui pre_sz = v_removed.size();
        for (int u : g->vertex)
            if (!vis[u])
                v_removed.push_back(u);
        for (ui i = pre_sz; i < v_removed.size(); i++)
        {
            g->remove_vertex(v_removed[i], edge_removed);
        }
        for (int u : g->vertex)
            vis[u] = 0;
    }

    /**
     * a vertex in S can lose at most l in-neighbors
     */
    void loss_limit_in(int v, vector<int> &v_removed, vector<ui> &edge_removed)
    {
        if (paramL > S->get_n())
            return;
        vector<int> vertex;
        for (int u : S->vertex)
            // if (S->get_in_degree(u) + paramL == S->get_n() && !g->exist_edge(v, u))
            if (S->get_in_degree(u) + paramL == S->get_n())
                vertex.push_back(u);
        for (int u : vertex)
            for (int v : g->neighbors_in(u))
            {
                vis[v]++;
            }
        ui pre_sz = v_removed.size();
        for (int v : g->vertex)
            if (!S->has(v) && vis[v] < vertex.size())
            {
                v_removed.push_back(v);
            }
        for (int u : g->vertex)
            vis[u] = 0;
        for (ui i = pre_sz; i < v_removed.size(); i++)
        {
            g->remove_vertex(v_removed[i], edge_removed);
        }
    }

    /**
     * a vertex in S can lose at most k out-neighbors
     */
    void loss_limit_out(int v, vector<int> &v_removed, vector<ui> &edge_removed)
    {
        if (paramK > S->get_n())
            return;
        vector<int> vertex;
        for (int u : S->vertex)
            // if (S->get_out_degree(u) + paramK == S->get_n() && !g->exist_edge(u, v))
            if (S->get_out_degree(u) + paramK == S->get_n())
                vertex.push_back(u);
        for (int u : vertex)
            for (int v : g->neighbors_out(u))
            {
                vis[v]++;
            }
        ui pre_sz = v_removed.size();
        for (int v : g->vertex)
            if (!S->has(v) && vis[v] < vertex.size())
            {
                v_removed.push_back(v);
            }
        for (int u : g->vertex)
            vis[u] = 0;
        for (ui i = pre_sz; i < v_removed.size(); i++)
        {
            g->remove_vertex(v_removed[i], edge_removed);
        }
    }
    /**
     * if u in C and u has many not-in neighbors in S, then u is unpromising
     * i.e., S+u is not a plex
     * T(n)=O(n+m)
     */
    void try_include(vector<int> &v_removed, vector<ui> &edge_removed)
    {
        if (S->get_n() < min(paramK, paramL))
            return;
        int *din = vis, *dout = a;
        for (int u : S->vertex)
        {
            for (int v : g->neighbors_out(u))
            {
                din[v]++;
            }
            for (int v : g->neighbors_in(u))
            {
                dout[v]++;
            }
        }
        int n = S->get_n() + 1;
        int pre_sz = v_removed.size();
        for (int v : g->vertex)
        {
            if (S->has(v))
            {
                din[v] = dout[v] = 0;
                continue;
            }
            if (din[v] + paramL < n || dout[v] + paramK < n)
            {
                v_removed.push_back(v);
            }
            din[v] = dout[v] = 0;
        }
        for (ui i = pre_sz; i < v_removed.size(); i++)
        {
            g->remove_vertex(v_removed[i], edge_removed);
        }
    }
    /**
     * v in S and u in C
     * if UB({v,u})<=lb, then u can be removed
     * @param mutual_exclude[0][u][v]=1 means UB({v,u})<=lb
     * this may be in-efficient beacause of RAU
     * T(n)=O(n)
     */
    void second_order_reduction(int v, vector<int> &v_removed, vector<ui> &edge_removed, vector<vector<bool>> *mutual_exclude)
    {
        for (int u : g->vertex)
        {
            if (S->has(u))
                continue;
            if (mutual_exclude[0][u][v])
            {
                v_removed.push_back(u);
                g->remove_vertex(u, edge_removed);
            }
        }
    }
#define CNT 18
    /**
     * reduction rules with linear time cost T(n)=O(n+m)
     */
    void fastReduction(vector<int> &v_removed, vector<ui> &edge_removed, bool need_weak_reduce, vector<vector<bool>> *mutual_exclude,int ub)
    {
        if (need_weak_reduce)
        {
            weak_reduce(v_removed, edge_removed);
        }
        int v = S->last_v;
        S->last_v = -1;
        if (v == -1)
            return;
        // second_order_reduction(v, v_removed, edge_removed, mutual_exclude);
        if (S->get_n() == 1)
        {
            two_hops_in(v, v_removed, edge_removed);
            two_hops_out(v, v_removed, edge_removed);
            return;
        }
        if(rand()*1.0/RAND_MAX >(ub-lb)*0.5/ub)
            return;
        // those reduction methods are not worthy because the reduced graph is dense
        switch (rand() % CNT)
        {
        // case 0:
        //     two_hops_in(v, v_removed, edge_removed);
        //     two_hops_out(v, v_removed, edge_removed);
        //     break;
        case 2:
            try_include(v_removed, edge_removed);
            break;
        case 3:
            loss_limit_in(v, v_removed, edge_removed);
            loss_limit_out(v, v_removed, edge_removed);
            break;
        default:
            break;
        }
        return;
    }
};

#endif