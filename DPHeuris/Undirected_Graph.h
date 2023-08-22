#ifndef UNDIRECTED_GRAPH_H
#define UNDIRECTED_GRAPH_H

#include "Utility.h"
#include "MyHashMap.h"
#include "LinearHeap.h"

struct AdjacentList_Undirected
{
    struct edge // the edge can be initial edge or reverse edge
    {
        ui a, b;
        ui pre, ne;
    };
    edge *edges;
    ui m; // capacity instead of edge cnt, so m won't decrease
    ll n;
    ui *h;
    void init(ui _m, ui _n)
    {
        m = _m;
        n = _n;
        edges = new edge[m];
        h = new ui[n];
        for (ui i = 0; i < n; i++)
            h[i] = m;
    }
    void free()
    {
        delete[] edges;
        delete[] h;
    }
    void clear(ui _m, ui _n)
    {
        m = _m;
        n = _n;
        for (ui i = 0; i < n; i++)
            h[i] = m;
    }
    void insert(ui key, ui edge_id)
    {
        if (h[key] != m)
        {
            edges[edge_id].ne = h[key];
            edges[h[key]].pre = edge_id;
        }
        h[key] = edge_id;
    }
    void insert_edge(ui a, ui b, ui id) // assert each edge is inserted once
    {
        edges[id] = {a, b, m, m};
        insert(a, id);
    }
    void remove_edge(ui id)
    {
        ui a = edges[id].a, b = edges[id].b;
        if (edges[id].pre == m)
        {
            h[a] = edges[id].ne;
            if (h[a] != m)
                edges[h[a]].pre = m;
        }
        else
        {
            ui pre = edges[id].pre, ne = edges[id].ne;
            edges[pre].ne = ne;
            if (ne != m)
                edges[ne].pre = pre;
        }
    }
};

class Undirected_Graph
{
private:
    ui n, m;
    ui *d;
    LinearHeap *heap;
    AdjacentList_Undirected adj;
    MyHashMap map;

public:
    Undirected_Graph(ui _n, vector<pii> &edges)
    {
        n = _n;
        d = new ui[n];
        memset(d, 0, sizeof(ui) * n);
        m = edges.size();
        adj.init(m, n);
        map.init(m, n);
        for (ui i = 0; i < m; i++)
        {
            adj.insert_edge(edges[i].x, edges[i].y, i);
            d[edges[i].x]++;
            map.insert_edge(edges[i].x, edges[i].y);
        }
        heap = new LinearHeap(n, n, d);
    }
    ~Undirected_Graph()
    {
        delete[] d;
        delete heap;
        adj.free();
        map.free();
    }
    ui get_min_degree_vertex()
    {
        ui ret = heap->get_min_node();
        heap->delete_node(ret);
        return ret;
    }
    vector<ui> neighbors(ui u)
    {
        vector<ui> ret;
        for (ui i = adj.h[u]; i < m; i = adj.edges[i].ne)
        {
            ret.push_back(adj.edges[i].b);
        }
        return ret;
    }
    bool has(ui a, ui b)
    {
        return map.has(a, b);
    }
    void remove_vertex(ui u)
    {
        for (ui i = adj.h[u]; i < m; i = adj.edges[i].ne)
        {
            ui j = adj.edges[i].b;
            d[j]--;
            heap->decrease(d[j], j);
            ui id = map.get(j, u);
            map.remove_edge(id);
            adj.remove_edge(id);
        }
    }
};

#endif