#ifndef GRAPH_H
#define GRAPH_H

#include "MyHashMap.h"
#include "LinearHeap.h"

// 邻接表
struct AdjacentList
{
    struct edge // 可以是正向边，也可以是反向边
    {
        ui a, b;
        ui pre, ne;
    };
    edge *edges;
    ui m; // 容量而非边数，所以不会减小
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
class Graph
{
public:
    ui n, m;
    ui orig_n, orig_m;
    ui *din, *dout, *pd;       // pseudo-degree
    AdjacentList h, reverse_h; // 正向边与反向边
    MyHashMap map;             // serve for func: exist_edge(a,b)
    ui *vis;                   // li san hua
    pii *initialEdges;
    bool *v_remove;
    int *q; // 用于弱缩减
    Graph()
    {
        vis = din = dout = pd = nullptr;
        v_remove = nullptr;
        q = nullptr;
    }
    ~Graph()
    {
        delete[] vis;
        delete[] din;
        delete[] dout;
        delete[] pd;
        if (v_remove != nullptr)
            delete[] v_remove;
    }
    bool exist_edge(ui a, ui b)
    {
        return map.has(a, b);
    }
    void initDegree(pii *edges)
    {
        if (din == nullptr)
            din = new ui[n];
        if (dout == nullptr)
            dout = new ui[n];
        if (pd == nullptr)
            pd = new ui[n];
        memset(din, 0, sizeof(ui) * n);
        memset(dout, 0, sizeof(ui) * n);
        for (ui i = 0; i < m; i++)
            dout[edges[i].x]++, din[edges[i].y]++;
        for (ui i = 0; i < n; i++)
            pd[i] = min(dout[i] + paramK, din[i] + paramL);
    }
    void initAdjacentList(pii *initialEdges)
    {
        for (ui i = 0; i < m; i++)
            h.insert_edge(initialEdges[i].x, initialEdges[i].y, i);
        for (ui i = 0; i < m; i++)
            reverse_h.insert_edge(initialEdges[i].y, initialEdges[i].x, i);
    }
    void initMap(pii *edges)
    {
        for (ui i = 0; i < m; i++)
            map.insert_edge(edges[i].x, edges[i].y);
    }
    void readFromFile(string file_path)
    {
        FILE *in = fopen(file_path.c_str(), "r");
        if (in == nullptr)
        {
            printf("Failed to open %s \n", file_path.c_str());
            exit(1);
        }
        printf("File: %s k= %d l= %d\n", get_file_name_without_suffix(file_path).c_str(), paramK, paramL);
        fscanf(in, "%u%u", &n, &m);
        initialEdges = new pii[m];
        for (ui i = 0; i < m; i++)
        {
            fscanf(in, "%u%u", &initialEdges[i].x, &initialEdges[i].y);
        }
        fclose(in);
        printf("Read ok: n= %u  m= %u \n", n, m);
        h.init(m, n);
        reverse_h.init(m, n);
        initAdjacentList(initialEdges);
        initDegree(initialEdges);
        // first LB is degeneracy order, so there is no need to use hash map
        // map.init(m,n);
        // initMap(initialEdges);
        orig_m = m;
        orig_n = n;
        puts("graph init finished");
        fflush(stdout);
    }
    void dump_to_file(string path)
    {
        FILE *out = fopen(path.c_str(), "w");
        if (out == nullptr)
        {
            printf("File open failed: %s\n", path.c_str());
            exit(1);
        }
        fprintf(out, "%u %u\n", n, m);
        for (ui i = 0; i < m; i++)
        {
            pii &h = initialEdges[i];
            fprintf(out, "%u %u\n", h.x, h.y);
        }
        fclose(out);
        exit(0);
    }
    void refresh() // 重新建图
    {
        if (!m)
        {
            orig_n = orig_m = n = 0;
            return;
        }
        ui i, j;
        bool *rm = map.rm;
        for (i = 0, j = 0; j < orig_m; j++)
        {
            while (j < orig_m && rm[j])
                j++;
            if (j >= orig_m)
                break;
            initialEdges[i++] = initialEdges[j];
        }
        m = i;
        if (vis == nullptr)
            vis = new ui[orig_n];
        memset(vis, 0, sizeof(ui) * orig_n);
        for (ui i = 0; i < m; i++)
            vis[initialEdges[i].x] = 1, vis[initialEdges[i].y] = 1;
        n = 0;
        for (ui i = 0; i < orig_n; i++)
            if (vis[i])
                vis[i] = n++;
        for (ui i = 0; i < m; i++)
            initialEdges[i] = {vis[initialEdges[i].x], vis[initialEdges[i].y]};
        map.clear(m);
        h.clear(m, n);
        reverse_h.clear(m, n);
        initAdjacentList(initialEdges);
        initDegree(initialEdges);
        initMap(initialEdges);
        orig_n = n;
        orig_m = m;
    }
    ui getLB_degeneracy_order()
    {
        LinearHeap heap(n + max(paramK, paramL), n, pd);
        while (heap.get_min_key() < heap.sz)
        {
            ui u = heap.get_min_node();
            // delete u
            heap.delete_node(u);
            for (ui i = h.h[u]; i < m; i = h.edges[i].ne)
            {
                ui j = initialEdges[i].y;
                assert(initialEdges[i].x == u);
                assert(u == h.edges[i].a);
                assert(j == h.edges[i].b);
                din[j]--;
                if (pd[j] > din[j] + paramL)
                {
                    pd[j] = din[j] + paramL;
                    heap.decrease(pd[j], j);
                }
                reverse_h.remove_edge(i);
            }
            for (ui i = reverse_h.h[u]; i < m; i = reverse_h.edges[i].ne)
            {
                ui j = initialEdges[i].x;
                dout[j]--;
                if (pd[j] > dout[j] + paramK)
                {
                    pd[j] = dout[j] + paramK;
                    heap.decrease(pd[j], j);
                }
                h.remove_edge(i);
            }
        }
        return heap.sz;
    }
    vector<ui> get_order()
    {
        LinearHeap heap(n + max(paramK, paramL), n, pd);
        vector<ui> ret;
        while (heap.sz>0)
        {
            ui u = heap.get_min_node();
            ret.push_back(u);
            // delete u
            heap.delete_node(u);
            for (ui i = h.h[u]; i < m; i = h.edges[i].ne)
            {
                ui j = initialEdges[i].y;
                assert(initialEdges[i].x == u);
                assert(u == h.edges[i].a);
                assert(j == h.edges[i].b);
                din[j]--;
                if (pd[j] > din[j] + paramL)
                {
                    pd[j] = din[j] + paramL;
                    heap.decrease(pd[j], j);
                }
                reverse_h.remove_edge(i);
            }
            for (ui i = reverse_h.h[u]; i < m; i = reverse_h.edges[i].ne)
            {
                ui j = initialEdges[i].x;
                dout[j]--;
                if (pd[j] > dout[j] + paramK)
                {
                    pd[j] = dout[j] + paramK;
                    heap.decrease(pd[j], j);
                }
                h.remove_edge(i);
            }
        }
        refresh();
        reverse(ret.begin(), ret.end());
        return ret;
    }
    ui getLB_multi_degeneracy()
    {
        do
        {
            ll st1=get_system_time_microsecond();
            ui temp_lb = getLB_degeneracy_order();
            printf("Acquire order: %.4lf s\n",(get_system_time_microsecond()-st1)/1e6);
            h.clear(m, n);
            reverse_h.clear(m, n);
            initAdjacentList(initialEdges);
            initDegree(initialEdges);
            if (lb && temp_lb > lb)
                cout << "***iteration for degeneracy order worked!***" << endl;
            if (temp_lb <= lb)
                break;
            else
                lb = temp_lb;
            weak_reduce();
            if (!n)
                break;
        } while (1); // 每一轮迭代，都是线性的
        if (n)       // next, prepare for strong reduce
        {
            map.init(m, n);
            initMap(initialEdges);
            if (v_remove == nullptr)
                v_remove = new bool[n];
            memset(v_remove, 0, sizeof(bool) * n);
            orig_n = n;
            orig_m = m;
        }
        return lb;
    }
    void weak_reduce(bool init_map = false)
    {
        if (q == nullptr)
            q = new int[n];
        if (vis == nullptr)
            vis = new ui[orig_n];
        memset(vis, 0, sizeof(ui) * n); // vis[u]=1 indicates u is in queue
        int hh = 0, tt = -1;
        for (ui i = 0; i < n; i++)
            if (pd[i] <= lb)
                q[++tt] = i, vis[i] = 1;
        while (hh <= tt)
        {
            ui u = q[hh++];
            for (ui i = h.h[u]; i < m; i = h.edges[i].ne)
            {
                ui j = initialEdges[i].y;
                if (vis[j])
                    continue;
                din[j]--;
                if (pd[j] > din[j] + paramL)
                {
                    pd[j] = din[j] + paramL;
                    if (pd[j] <= lb)
                    {
                        q[++tt] = j;
                        vis[j] = 1;
                    }
                }
                reverse_h.remove_edge(i);
            }
            for (ui i = reverse_h.h[u]; i < m; i = reverse_h.edges[i].ne)
            {
                ui j = initialEdges[i].x;
                if (vis[j])
                    continue;
                dout[j]--;
                if (pd[j] > dout[j] + paramK)
                {
                    pd[j] = dout[j] + paramK;
                    if (pd[j] <= lb)
                    {
                        q[++tt] = j;
                        vis[j] = 1;
                    }
                }
                h.remove_edge(i);
            }
        }
        m = 0;
        n = 0;
        for (ui i = 0; i < orig_m; i++)
        {
            pii &h = initialEdges[i];
            if (vis[h.x] || vis[h.y])
                continue;
            initialEdges[m++] = h;
        }
        for (ui u = 0; u < orig_n; u++)
        {
            if (vis[u])
                continue;
            vis[u] = n++;
        }
        if (n <= lb)
        {
            orig_n = n = orig_m = m = 0;
            return;
        }
        for (ui i = 0; i < m; i++)
            initialEdges[i] = {vis[initialEdges[i].x], vis[initialEdges[i].y]};
        // 重新建图
        h.clear(m, n);
        reverse_h.clear(m, n);
        initAdjacentList(initialEdges);
        initDegree(initialEdges);
        if (init_map)
        {
            map.clear(m);
            initMap(initialEdges);
        }
        orig_m = m;
        orig_n = n;
    }
    vector<int> neighbor_in(ui u)
    {
        vector<int> ret;
        for (ui i = reverse_h.h[u]; i < orig_m; i = reverse_h.edges[i].ne)
        {
            ui j = initialEdges[i].x;
            ret.push_back(j);
        }
        return ret;
    }
    vector<int> neighbor_out(ui u)
    {
        vector<int> ret;
        for (ui i = h.h[u]; i < orig_m; i = h.edges[i].ne)
        {
            ui j = initialEdges[i].y;
            ret.push_back(j);
        }
        return ret;
    }
    bool remove_edge_forever(ui a, ui b)
    {
        ui edge_id = map.remove_edge(a, b);
        if (edge_id >= orig_m)
            return false;
        m--;
        h.remove_edge(edge_id);
        reverse_h.remove_edge(edge_id);
        din[b]--;
        dout[a]--;
        pd[a] = min(pd[a], dout[a] + paramK);
        pd[b] = min(pd[b], din[b] + paramL);
        return true;
    }
    bool remove_edge_forever(ui edge_id)
    {
        if (!map.remove_edge(edge_id))
            return false;
        m--;
        h.remove_edge(edge_id);
        reverse_h.remove_edge(edge_id);
        ui a = initialEdges[edge_id].x;
        ui b = initialEdges[edge_id].y;
        din[b]--;
        dout[a]--;
        pd[a] = min(pd[a], dout[a] + paramK);
        pd[b] = min(pd[b], din[b] + paramL);
        return true;
    }
    void remove_vertex_forever(ui u)
    {
        n--;
        m -= din[u] + dout[u];
        v_remove[u] = 1;
        for (ui i = h.h[u]; i < orig_m; i = h.edges[i].ne)
        {
            ui j = initialEdges[i].y;
            din[j]--;
            if (pd[j] > din[j] + paramL)
            {
                pd[j] = din[j] + paramL;
            }
            reverse_h.remove_edge(i);
            map.remove_edge(i);
        }
        for (ui i = reverse_h.h[u]; i < orig_m; i = reverse_h.edges[i].ne)
        {
            ui j = initialEdges[i].x;
            dout[j]--;
            if (pd[j] > dout[j] + paramK)
            {
                pd[j] = dout[j] + paramK;
            }
            h.remove_edge(i);
            map.remove_edge(i);
        }
    }
    int get_in_degree(int u)
    {
        return din[u];
    }
    int get_out_degree(int u)
    {
        return dout[u];
    }
    bool satisfied(int u)
    {
        return pd[u] > lb;
    }
};

#endif