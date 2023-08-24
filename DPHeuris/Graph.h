#ifndef GRAPH_H
#define GRAPH_H

#include "MyHashMap.h"
#include "LinearHeap.h"

struct AdjacentList
{
    struct edge // can be reverse edges
    {
        ui a, b;
        ui pre, ne;
    };
    edge *edges;
    ui m; // capacity instead of edge_cnt
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
    ui *din, *dout, *pd; // pseudo-degree
    AdjacentList h, reverse_h;
    MyHashMap map; // serve for func: exist_edge(a,b)
    ui *vis;       // discretization
    pii *initialEdges;
    bool *v_remove;
    int *q;                                 // used for weak reduce
    unordered_map<int, int> map_refresh_id; // we need to re-map the reduced graph to {0,1,...,n-1}, thus requiring to record the map
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
    // read graph, then conduct degeneracy order & weak reduce
    void readFromFile(string file_path, double &use_time, set<int> *s = nullptr)
    {
        FILE *in = fopen(file_path.c_str(), "r");
        if (in == nullptr)
        {
            printf("Failed to open %s \n", file_path.c_str());
            exit(1);
        }
        printf("File: %s k= %d l= %d\n", get_file_name_without_suffix(file_path).c_str(), paramK, paramL);
        fscanf(in, "%u%u", &n, &m);
        int *din=new int[n];
        memset(din, 0,sizeof(int)*n);
        int *dout=new int[n];
        memset(dout, 0, sizeof(int)*n);
        vector<vector<int>> in_neighbor(n),out_neighbor(n);
        for(ui i=0;i<m;i++)
        {
            int a,b;
            fscanf(in,"%d%d",&a,&b);
            dout[a]++, din[b]++;
            out_neighbor[a].push_back(b);
            in_neighbor[b].push_back(a);
        }
        fclose(in);
        printf("Read ok: n= %u  m= %u \n", n, m);
        fflush(stdout);
        double start_time=get_system_time_microsecond();
        ui *pd=new ui[n];
        for(ui i=0;i<n;i++)
            pd[i]=min(din[i]+paramL, dout[i]+paramK);
        // degeneracy order
        bool *rm=new bool[n];
        memset(rm, 0, sizeof(bool)*n);
        LinearHeap heap(n + max(paramK, paramL), n, pd);
        while (heap.get_min_key() < heap.sz)
        {
            ui u = heap.get_min_node();
            // delete u
            heap.delete_node(u);
            rm[u]=1;
            for(int v:out_neighbor[u])
            {
                if(rm[v]) continue;
                if(--din[v] + paramL < pd[v])
                {
                    pd[v]--;
                    heap.decrease(pd[v], v);
                }
            }
            for(int v:in_neighbor[u])
            {
                if(rm[v]) continue;
                if(--dout[v] + paramK < pd[v])
                {
                    pd[v]--;
                    heap.decrease(pd[v], v);
                }
            }
        }
        lb=heap.sz;
        if (s != nullptr)
        {
            s->clear();
            while (heap.sz > 0)
            {
                int u = heap.get_min_node();
                heap.delete_node(u);
                assert(s->count(u)==0);
                s->insert(u);
            }
            assert(s->size() == lb);
        }
        memset(rm, 0, sizeof(bool)*n);
        for(int i=0;i<n;i++)
            din[i]=in_neighbor[i].size();
        for(int i=0;i<n;i++)
            dout[i]=out_neighbor[i].size();
        int hh=0, tt=-1;
         ui *q=pd;
        for(int i=0;i<n;i++)
            if(din[i]+paramL<=lb || dout[i]+paramK<=lb)
                q[++tt]=i, rm[i]=1;
        while(hh<=tt)
        {
            int u=q[hh++];
            for(int v:out_neighbor[u])
            {
                if(rm[v])   continue;
                if(--din[v]+paramL<=lb)
                {
                    q[++tt]=v;
                    rm[v]=1;
                }
            }
            for(int v:in_neighbor[u])
            {
                if(rm[v]) continue;
                if(--dout[v]+paramK<=lb)
                {
                    q[++tt]=v;
                    rm[v]=1;
                }
            }
        }
        int left_n=0;
        ui *vis=pd;
        ui max_m=0;
        for(ui i=0;i<n;i++)
        {
            if(!rm[i])
            {
                max_m+=out_neighbor[i].size();
                map_refresh_id[left_n]=i;
                vis[i]=left_n++;
            }
        }
        initialEdges = new pii[max_m];
        ui left_m=0;
        for(ui i=0;i<n;i++)
        {
            if(!rm[i])
            {
                for(int v:out_neighbor[i])
                {
                    if(rm[v]) continue;
                    initialEdges[left_m++]={vis[i], vis[v]};
                }
            }
        }
        delete[] pd;
        delete[] din;
        delete[] dout;
        delete[] rm;
        in_neighbor.clear();
        in_neighbor.shrink_to_fit();
        out_neighbor.clear();
        out_neighbor.shrink_to_fit();
        n=left_n;
        m=left_m;
        if(n>0)
        {
            h.init(m, n);
            reverse_h.init(m, n);
            initAdjacentList(initialEdges);
            initDegree(initialEdges);
        }
        orig_m = m;
        orig_n = n;
        double end_time=get_system_time_microsecond();
        use_time+=(end_time-start_time)/1e6;
    }
    void dump_to_file(string path)
    {
        FILE *out = fopen(path.c_str(), "w");
        if (out == nullptr)
        {
            printf("File open failed: %s\n", path.c_str());
            exit(1);
        }
        fprintf(out, "%u %u %d\n", n, m, lb);
        for (ui i = 0; i < m; i++)
        {
            pii &h = initialEdges[i];
            fprintf(out, "%u %u\n", h.x, h.y);
        }
        puts("");
        // dump the map
        for (ui i = 0; i < n; i++)
            fprintf(out, "%d\n", map_refresh_id[i]);
        fclose(out);
        exit(0);
    }
    void refresh() // rebuild graph
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
        unordered_map<int, int> pre_map = map_refresh_id;
        for (ui i = 0; i < orig_n; i++)
            if (vis[i])
            {
                map_refresh_id[n] = map_refresh_id[i];
                vis[i] = n++;
            }
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
    ui getLB_degeneracy_order(set<int> *s = nullptr)
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
        if (s != nullptr && heap.sz > lb)
        {
            s->clear();
            while (heap.sz > 0)
            {
                int u = heap.get_min_node();
                heap.delete_node(u);
                s->insert(map_refresh_id[u]);
            }
            return s->size();
        }
        return heap.sz;
    }
    vector<ui> get_order()
    {
        LinearHeap heap(n + max(paramK, paramL), n, pd);
        vector<ui> ret;
        while (heap.sz > 0)
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
    ui getLB_multi_degeneracy(set<int> *s = nullptr)
    {
        do
        {
            ui temp_lb = getLB_degeneracy_order(s);
            h.clear(m, n);
            reverse_h.clear(m, n);
            initAdjacentList(initialEdges);
            initDegree(initialEdges);
            if (temp_lb <= lb)
                break;
            else
                lb = temp_lb;
            weak_reduce();
            if (!n)
                break;
        } while (1); // each round T(n)=O(n)
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
        unordered_map<int, int> pre_map = map_refresh_id;
        for (ui u = 0; u < orig_n; u++)
        {
            if (vis[u])
                continue;
            map_refresh_id[n] = pre_map[u];
            vis[u] = n++;
        }
        if (n <= lb)
        {
            orig_n = n = orig_m = m = 0;
            return;
        }
        for (ui i = 0; i < m; i++)
            initialEdges[i] = {vis[initialEdges[i].x], vis[initialEdges[i].y]};
        // rebuild graph
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