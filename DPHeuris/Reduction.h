#ifndef REDUCTION_H
#define REDUCTION_H

#include "Graph.h"
#include "Undirected_Graph.h"

class Reduction
{
private:
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
        delete q;
    }

    void init()
    {
        m = g->m;
        n = g->n;
        undirected_edges.resize(m * 2);
        //treat directed edges as undirected edges
        pii *edges = g->initialEdges;
        for (ui i = 0; i < m; i++)
        {
            ui a = edges[i].x, b = edges[i].y;
            undirected_edges[i << 1] = {a, b};
            undirected_edges[i << 1 | 1] = {b, a};
        }
        // sort and unique
        unique_pii(undirected_edges, n);
        m = undirected_edges.size();
        if (ug != nullptr)
            delete ug;
        ug = new Undirected_Graph(n, undirected_edges);
    }

public:
    Reduction(Graph *_g)
    {
        g = _g;
        n = g->n;
        common_in_neighbor_cnt = get_suitable_map(n, g->m);
        common_out_neighbor_cnt = get_suitable_map(n, g->m);
        ug = nullptr;
        init();
    }
    ~Reduction()
    {
        delete common_in_neighbor_cnt;
        delete common_out_neighbor_cnt;
    }
    void strong_reduce(bool first_reduce = false)
    {
        list_triangles();
        Queue *q_e = new Queue(g->m + 1); // queue of edges to be removed
        queue<int> q_v;                   // queue of vertices to be removed
        pii *edges = g->initialEdges;
        // csapp improvement
        int lb_2k=lb-2*paramK;
        int lb_2l=lb-2*paramL;
        int lb_2k1=lb_2k+1;
        int lb_2l1=lb_2l+1;
        for (int i = 0; i < g->m; i++)
        {
            int u = edges[i].x;
            int v = edges[i].y;
            if (g->exist_edge(v, u))
            {
                if (common_out_neighbor_cnt->get({u, v}) <= lb_2k || common_in_neighbor_cnt->get({u, v}) <= lb_2l)
                {
                    q_e->push(g->map.get(u, v));
                }
            }
            else
            {
                if (common_out_neighbor_cnt->get({u, v}) <= lb_2k1 || common_in_neighbor_cnt->get({u, v}) <= lb_2l1)
                {
                    q_e->push(g->map.get(u, v));
                }
            }
        }
        bool *v_remove = new bool[n];
        memset(v_remove, 0, sizeof(bool) * n);
        if (first_reduce)
        {
            while (q_e->size())
            {
                auto id = q_e->front();
                q_e->pop();
                g->remove_edge_forever(id);
            }
            g->refresh();
            q_e->clear(g->m);
            init();
            common_in_neighbor_cnt->clear();
            common_out_neighbor_cnt->clear();
            list_triangles();
            for (int i = 0; i < g->m; i++)
            {
                int u = edges[i].x;
                int v = edges[i].y;
                if (g->exist_edge(v, u))
                {
                    if (common_out_neighbor_cnt->get({u, v}) <= lb_2k || common_in_neighbor_cnt->get({u, v}) <= lb_2l)
                    {
                        q_e->push(g->map.get(u, v));
                    }
                }
                else
                {
                    if (common_out_neighbor_cnt->get({u, v}) <= lb_2k1 || common_in_neighbor_cnt->get({u, v}) <= lb_2l1)
                    {
                        q_e->push(g->map.get(u, v));
                    }
                }
            }
            for (int i = 0; i < g->n; i++)
                if (g->pd[i] <= lb)
                {
                    q_v.push(i);
                    v_remove[i] = 1;
                }
        }
        while (q_v.size() || q_e->size())
        {
            while (q_e->size()) 
            {
                ui edge_id = q_e->front();
                q_e->pop();
                int from = edges[edge_id].x, to = edges[edge_id].y;
                if (!g->remove_edge_forever(edge_id)) 
                {
                    continue;
                }
                if (g->pd[from] <= lb && !v_remove[from])
                {
                    q_v.push(from);
                    v_remove[from] = 1;
                }
                if (g->pd[to] <= lb && !v_remove[to])
                {
                    q_v.push(to);
                    v_remove[to] = 1;
                }
                int from_cnt = g->din[from] + g->dout[from];
                int to_cnt = g->din[to] + g->dout[to];
                if (from_cnt <= to_cnt)
                {
                    // for (int w : g->neighbor_out(from))
                    for (ui i = g->h.h[from]; i < g->orig_m; i = g->h.edges[i].ne)
                    {
                        int w = g->initialEdges[i].y;
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
                                q_e->push(g->map.get(to, w));
                                q_e->push(g->map.get(w, to));
                            }
                        }
                        else
                        {
                            if (val_now <= lb_2l1)
                            {
                                if (w_to)
                                {
                                    q_e->push(g->map.get(w, to));
                                }
                                else
                                {
                                    q_e->push(g->map.get(to, w));
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
                                    q_e->push(g->map.get(from, w));
                                    q_e->push(g->map.get(w, from));
                                }
                            }
                            else
                            {
                                if (val <= lb_2k1)
                                {
                                    q_e->push(g->map.get(from, w));
                                }
                            }
                        }
                    }
                    // for (int w : g->neighbor_in(from))
                    for (ui i = g->reverse_h.h[from]; i < g->orig_m; i = g->reverse_h.edges[i].ne)
                    {
                        int w = g->initialEdges[i].x;
                        if (v_remove[w] || !g->exist_edge(w, to))
                            continue;
                        if (g->exist_edge(from, w))
                            continue; // we only need Unidirectional edge
                        int val = common_out_neighbor_cnt->reduce({from, w});
                        if (val <= lb_2k1)
                        {
                            q_e->push(g->map.get(w, from));
                        }
                    }
                }
                else
                {
                    // for (int w : g->neighbor_in(to))
                    for (ui i = g->reverse_h.h[to]; i < g->orig_m; i = g->reverse_h.edges[i].ne)
                    {
                        int w = g->initialEdges[i].x;
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
                                q_e->push(g->map.get(from, w));
                                q_e->push(g->map.get(w, from));
                            }
                        }
                        else
                        {
                            if (val_now <= lb_2k1)
                            {
                                if (w_from)
                                {
                                    q_e->push(g->map.get(w, from));
                                }
                                else
                                {
                                    q_e->push(g->map.get(from, w));
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
                                    q_e->push(g->map.get(to, w));
                                    q_e->push(g->map.get(w, to));
                                }
                            }
                            else
                            {
                                if (val <= lb_2l1)
                                {
                                    q_e->push(g->map.get(w, to));
                                }
                            }
                        }
                    }
                    // for (int w : g->neighbor_out(to))
                    for (ui i = g->h.h[to]; i < g->orig_m; i = g->h.edges[i].ne)
                    {
                        int w = g->initialEdges[i].y;
                        if (v_remove[w] || !g->exist_edge(from, w))
                            continue;
                        if (g->exist_edge(w, to))
                            continue; // Unidirectional edge
                        int val = common_in_neighbor_cnt->reduce({to, w});
                        if (val <= lb_2l1)
                        {
                            q_e->push(g->map.get(to, w));
                        }
                    }
                }
            }
            if (q_v.size()) 
            {
                int u = q_v.front();
                q_v.pop();
                // for (int to : g->neighbor_out(u)) 
                for (ui i = g->h.h[u]; i < g->orig_m; i = g->h.edges[i].ne)
                {
                    int to = g->initialEdges[i].y;
                    if (g->din[to] - 1 <= lb - paramL && !v_remove[to])
                    {
                        q_v.push(to);
                        v_remove[to] = 1;
                    }
                }

                // for (int from : g->neighbor_in(u))
                for (ui i = g->reverse_h.h[u]; i < g->orig_m; i = g->reverse_h.edges[i].ne)
                {
                    int from = g->initialEdges[i].x;
                    if (g->dout[from] - 1 <= lb - paramK && !v_remove[from])
                    {
                        q_v.push(from);
                        v_remove[from] = 1;
                    }
                }
                for (int v : g->neighbor_out(u))
                {
                    if (v_remove[v])
                        continue;
                    for (int w : g->neighbor_out(u))
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
                                    q_e->push(g->map.get(w, v));
                                    q_e->push(g->map.get(v, w));
                                }
                            }
                            else if (wv)
                            {
                                if (val <= lb_2l1)
                                    q_e->push(g->map.get(w, v));
                            }
                            else
                            {
                                if (val <= lb_2l1)
                                    q_e->push(g->map.get(v, w));
                            }
                        }
                    }
                }
                for (int v : g->neighbor_in(u))
                {
                    if (v_remove[v])
                        continue;
                    for (int w : g->neighbor_in(u))
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
                                    q_e->push(g->map.get(w, v));
                                    q_e->push(g->map.get(v, w));
                                }
                            }
                            else if (wv)
                            {
                                if (val <= lb_2k1)
                                    q_e->push(g->map.get(w, v));
                            }
                            else
                            {
                                if (val <= lb_2k1)
                                    q_e->push(g->map.get(v, w));
                            }
                        }
                    }
                }
                g->remove_vertex_forever(u);
            }
        }
        delete q_e;
    }
};

#endif