#ifndef GRAPH_H
#define GRAPH_H

#include "../DPHeuris/Utility.h"
#include "MFastSet.h"

#define from x
#define to y
#define fill_with0(a) fill(a.begin(), a.end(), 0)
#define fill_with_neg1(a) fill(a.begin(), a.end(), -1)

struct DirectedEdge
{
    ui id;
    ui from;
    ui to;
    bool operator<(const DirectedEdge &E) const
    {
        return from < E.from || (from == E.from && to < E.to);
    }
    bool operator==(const DirectedEdge &E) const
    {
        return id == E.id;
    }
};

static // faster than std::sort ; and the a[i].y is orderd
void countingSort(vector<DirectedEdge> &a, int k)
{
    vector<vector<int>> p(k);
    for(ui i=0;i<a.size();i++)
    {
        p[a[i].x].push_back(i);
    }
    static vector<DirectedEdge> temp;
    temp.resize(a.size());
    int sz=0;
    for (int i = 0; i < k; i++)
        for (int id : p[i])
            temp[sz++]=a[id];
    // a=temp;
    swap(a,temp);
}

class Graph
{
public:
    MFastSet_LinkedList vertex;
    ll N;
    double rollback_time, remove_time;
    ll rollback_v_cnt;
    Graph *backupG;
    vector<int> din, dout, head, reverse_head, next;
    vector<DirectedEdge> reverse_edges;
    vector<DirectedEdge> edges;
    vector<bool> edge_remove_flag;
    vector<int> pii_to_edge_index;
    vector<int> induced_vertex;
    Graph()
    {
        rollback_v_cnt = 0;
        rollback_time = remove_time = 0;
    }
    ~Graph() {}
    Graph &operator=(Graph &g)
    {
        int n = g.N;
        N = n;
        reverse_edges = g.reverse_edges;
        edges = g.edges;
        pii_to_edge_index = g.pii_to_edge_index;
        din = g.din;
        dout = g.dout;
        head = g.head;
        reverse_head = g.reverse_head;
        next = g.next;
        edge_remove_flag = g.edge_remove_flag;
        vertex.init(n);
        for (int u : g.vertex)
            vertex.insert(u);
        return (*this);
    }
    void init(int n)
    {
        N = n;
        vertex.clear();
        for (int i = 0; i < n; i++)
            vertex.insert(i);
        pii_to_edge_index.resize(n * n);
        fill_with_neg1(pii_to_edge_index);
        din.resize(n);
        fill_with0(din);
        dout.resize(n);
        fill_with0(dout);
        next.resize(n);
        head.resize(n);
        fill_with_neg1(head);
        reverse_head.resize(n);
        fill_with_neg1(reverse_head);
        ui m = edges.size();
        edge_remove_flag.resize(m);
        fill_with0(edge_remove_flag);
        reverse_edges.resize(m);
        for (ui i = 0; i < m; i++)
        {
            ui a = edges[i].x, b = edges[i].y;
            din[b]++, dout[a]++;
            pii_to_edge_index[get_edge_id(a, b)] = i;
            if (head[a] == -1)
                head[a] = i;
            next[a] = i + 1;
            reverse_edges[i] = {i, b, a};
        }
        countingSort(reverse_edges, n);
        for (ui i = 0; i < m; i++)
        {
            int x = reverse_edges[i].x;
            if (reverse_head[x] == -1)
                reverse_head[x] = i;
        }
    }
    /**
     * V(g) = u + N(u) + N_2hop(u)
     */
    void induce(int u, Graph *g, vector<int> &v_map, vector<vector<bool>> *exclude)
    {
        fill_with_neg1(v_map);
        vector<bool> in(N);
        for (ui v : neighbors_in(u))
        {
            if(exclude[0][u][v]) continue;
            in[v]=1;
            for (ui w : neighbors_in(v))
                in[w] = !exclude[0][u][w];
        }
        vector<bool> vis(N);
        for (ui v : neighbors_out(u))
        {
            if(!in[v]) continue;
            vis[v]=1;
            for (ui w : neighbors_out(v))
                vis[w]=in[w];
        }
        auto &V = g->induced_vertex;
        V.clear();
        vis[u]=1;
        for (ui i = 0; i < N; i++)
        {
            if (vis[i])
                V.push_back(i);
        }
        for (ui i = 0; i < V.size(); i++)
            v_map[V[i]] = i;
        vector<DirectedEdge> &induced_edges=g->edges;
        induced_edges.clear();
        for (ui i = 0; i < edges.size(); i++)
        {
            if (edge_remove_flag[i])
                continue;
            int a = v_map[edges[i].x], b = v_map[edges[i].y];
            if (a == -1 || b == -1)
                continue;
            ui id = induced_edges.size();
            induced_edges.push_back({id, (ui)a, (ui)b});
        }
        g->init(V.size());
    }
    void readFromFile(string file_path, unordered_map<int,int> *id_map=nullptr)
    {
        FILE *in = fopen(file_path.c_str(), "r");
        if (in == nullptr)
        {
            printf("Failed to open %s \n", file_path.c_str());
            exit(1);
        }
        printf("File: %s k= %d l= %d\n", get_file_name_without_suffix(file_path).c_str(), paramK, paramL);
        ui n, m;
        fscanf(in, "%u%u%d", &n, &m, &lb);
        if (n <= lb)
        {
            printf("The heuristic solution is the best solution!\nground truth= %d\n", lb);
            puts("use time : 0.0000 s");
            exit(0);
        }
        N = n;
        vertex.init(n);
        for (ui i = 0; i < n; i++)
            vertex.insert(i);
        edges.resize(m);
        reverse_edges.resize(m);
        edge_remove_flag.resize(m);
        pii_to_edge_index.resize(N * N);
        fill(pii_to_edge_index.begin(), pii_to_edge_index.end(), -1);
        din.resize(n);
        dout.resize(n);
        next.resize(n);
        head.resize(n);
        fill(head.begin(), head.end(), -1);
        reverse_head.resize(n);
        for (ui i = 0; i < m; i++)
        {
            ui a, b;
            fscanf(in, "%u%u", &a, &b);
            edges[i] = {i, a, b};
            din[b]++, dout[a]++;
            pii_to_edge_index[get_edge_id(a, b)] = i;
            if (head[a] == -1)
                head[a] = i;
            next[a] = i + 1;
            reverse_edges[i] = {i, b, a};
        }
        if(id_map!=nullptr)
        {
            for(ui i=0;i<n;i++)
            {
                int v;
                fscanf(in, "%d", &v);
                (*id_map)[i]=v;
            }
        }
        fclose(in);
        printf("Read ok: n= %u  m= %u \n", n, m);
        fflush(stdout);

        fill(reverse_head.begin(), reverse_head.end(), -1);
        sort(reverse_edges.begin(), reverse_edges.end());
        for (int i = 0; i < m; i++)
        {
            int edge_id = reverse_edges[i].id;
            int from = reverse_edges[i].from;
            int to = reverse_edges[i].to;
            if (reverse_head[from] == -1)
                reverse_head[from] = i;
        }
    }
    vector<ui> neighbors_in(ui u)
    {
        vector<ui> ret;
        if (reverse_head[u] != -1)
        {
            for (int p = reverse_head[u]; p < (int)reverse_edges.size() && reverse_edges[p].from == u; p++)
            {
                int id = reverse_edges[p].id;
                if (edge_remove_flag[id])
                    continue;
                ret.push_back(reverse_edges[p].to);
            }
        }
        assert(ret.size() == din[u]);
        return ret;
    }
    vector<ui> neighbors_out(ui u)
    {
        vector<ui> ret;
        if (head[u] != -1)
        {
            for (int i = head[u]; i != next[u]; i++)
            {
                if (edge_remove_flag[i])
                    continue;
                ret.push_back(edges[i].to);
            }
        }
        assert(ret.size() == dout[u]);
        return ret;
    }
    int get_in_degree(ui u)
    {
        return din[u];
    }
    int get_out_degree(ui u)
    {
        return dout[u];
    }
    int get_pd(ui u)
    {
        return min(din[u] + paramL, dout[u] + paramK);
    }
    int get_n()
    {
        return vertex.size();
    }
    ui get_m()
    {
        ui ret = 0;
        // for (int u : vertex.getAll())
        for (ui u : vertex)
            ret += dout[u];
        return ret;
    }
    pii get_edge_by_id(ll key)
    {
        return {key / N, key % N};
    }
    ll get_edge_id(pii e)
    {
        return e.x * N + e.y;
    }
    ll get_edge_id(ui a, ui b)
    {
        return a * N + b;
    }
    /**
     * @return edge_removed
     * considering that after removing u, other vertices may have lower pd
     * so there may be more than one vertex to be removed, that's why we set the bool param
     */
    void remove_vertex(ui v, vector<ui> &edge_removed, bool &need_reduce)
    {
        double start_remove_time = get_system_time_microsecond();
        vertex.remove(v);
        if (head[v] != -1)
        {
            for (int i = head[v]; i != next[v]; i++)
            {
                if (edge_remove_flag[i])
                    continue;
                edge_remove_flag[i] = true;
                int to = edges[i].to;
                if (--din[to] + paramL <= lb)
                    need_reduce = true;
                edge_removed.push_back(i);
            }
        }
        if (reverse_head[v] != -1)
        {
            for (int p = reverse_head[v]; p < (int)reverse_edges.size() && reverse_edges[p].from == v; p++)
            {
                int edge_id = reverse_edges[p].id;
                if (edge_remove_flag[edge_id])
                    continue;
                edge_remove_flag[edge_id] = true;
                int from = reverse_edges[p].to;
                if (--dout[from] + paramK <= lb)
                    need_reduce = true;
                edge_removed.push_back(edge_id);
            }
        }
        din[v] = dout[v] = 0;
        remove_time += get_system_time_microsecond() - start_remove_time;
    }
    void remove_vertex(ui v, vector<ui> &edge_removed)
    {
        double start_remove_time = get_system_time_microsecond();
        vertex.remove(v);
        if (head[v] != -1)
        {
            for (int i = head[v]; i != next[v]; i++)
            {
                if (edge_remove_flag[i])
                    continue;
                edge_remove_flag[i] = true;
                din[edges[i].to]--;
                edge_removed.push_back(i);
            }
        }
        if (reverse_head[v] != -1)
        {
            for (int p = reverse_head[v]; p < (int)reverse_edges.size() && reverse_edges[p].from == v; p++)
            {
                if (edge_remove_flag[reverse_edges[p].id])
                    continue;
                edge_remove_flag[reverse_edges[p].id] = true;
                dout[reverse_edges[p].to]--;
                edge_removed.push_back(reverse_edges[p].id);
            }
        }
        din[v] = dout[v] = 0;
        remove_time += get_system_time_microsecond() - start_remove_time;
    }
    void remove_vertex_forever(ui v)
    {
        vertex.remove(v);
        if (head[v] != -1)
        {
            for (int i = head[v]; i != next[v]; i++)
            {
                if (edge_remove_flag[i])
                    continue;
                edge_remove_flag[i] = true;
                din[edges[i].to]--;
            }
        }
        if (reverse_head[v] != -1)
        {
            for (int p = reverse_head[v]; p < (int)reverse_edges.size() && reverse_edges[p].from == v; p++)
            {
                if (edge_remove_flag[reverse_edges[p].id])
                    continue;
                edge_remove_flag[reverse_edges[p].id] = true;
                dout[reverse_edges[p].to]--;
            }
        }
        din[v] = dout[v] = 0;
    }
    void remove_edge_forever(ui a, ui b)
    {
        if (head[a] == -1)
            return;
        int l = head[a], r = next[a] - 1;
        if (edges[r].to < b)
            return;
        while (l < r)
        {
            int mid = (l + r) >> 1;
            if (edges[mid].to >= b)
                r = mid;
            else
                l = mid + 1;
        }
        if (edges[l].to != b)
            return;
        int &edge_id = l;
        if (edge_remove_flag[edge_id])
            return;
        assert(has(a) && has(b));
        edge_remove_flag[edge_id] = true;
        dout[a]--;
        din[b]--;
    }
    void remove_edge_forever_by_index(int edge_id)
    {
        int a = edges[edge_id].x, b = edges[edge_id].y;
        dout[a]--;
        din[b]--;
        edge_remove_flag[edge_id] = 1;
    }
    void rollback(ui u, vector<ui> &edges)
    {
        ll start_rollback_time = get_system_time_microsecond();
        vertex.insert(u);
        for (ui i : edges)
        {
            int x = this->edges[i].x, y = this->edges[i].y;
            if (!has(x) || !has(y))
                continue;
            dout[x]++, din[y]++;
            edge_remove_flag[i] = 0;
        }
        rollback_time += get_system_time_microsecond() - start_rollback_time;
    }
    void rollback(vector<int> &V, vector<ui> &edges)
    {
        rollback_v_cnt += V.size();
        ll start_rollback_time = get_system_time_microsecond();
        for (int v : V)
        {
            assert(v < induced_vertex.size());
            if (backupG->has(induced_vertex[v]))
                vertex.insert(v);
        }
        for (ui i : edges)
        {
            int x = this->edges[i].x, y = this->edges[i].y;
            if (!has(x) || !has(y))
                continue;
            dout[x]++, din[y]++;
            edge_remove_flag[i] = 0;
        }
        rollback_time += get_system_time_microsecond() - start_rollback_time;
    }
    bool exist_edge(ui a, ui b)
    {
        int index = pii_to_edge_index[get_edge_id(a, b)];
        return index != -1 && !edge_remove_flag[index];
    }
    bool has(int v)
    {
        return vertex.has(v);
    }
    ui get_vertex_with_min_pd(set<ui> &no)
    {
        int min_pd = N + max(paramK, paramL);
        ui sel = -1;
        for (ui v : vertex)
        {
            if (no.count(v))
                continue;
            int pd = get_pd(v);
            if (pd < min_pd)
            {
                min_pd = pd;
                sel = v;
            }
        }
        return sel;
    }
    ui get_vertex_with_min_pd(vector<int> &map)
    {
        int min_pd = N*2;
        ui sel = -1;
        int cnt=0;
        for (ui v : vertex)
        {
            if (map[v] != -1) // S has v
                continue;
            // return v;
            int pd = get_pd(v);
            if (pd < min_pd)
            {
                min_pd = pd;
                sel = v;
                if(++cnt==6)
                    return sel;
            }
        }
        return sel;
    }
    ui get_vertex_with_min_pd()
    {
        int min_pd = N + max(paramK, paramL);
        ui sel = -1;
        for (ui v : vertex)
        {
            int pd = get_pd(v);
            if (pd < min_pd)
            {
                min_pd = pd;
                sel = v;
            }
        }
        return sel;
    }
    int get_min_pd()
    {
        int ret=get_n()+ min(paramK, paramL)-1;
        for(ui v:vertex)
            ret=min(ret, get_pd(v));
        return ret;
    }
    bool is_plex()
    {
        return vertex.size() <= get_pd(get_vertex_with_min_pd());
    }
};

class Plex
{
public:
    double plex_add_time, plex_remove_time;
    vector<int> vertex, din, dout, last_edge_cnt;
    vector<int> vertex_idx_map;
    vector<pii> edges;
    ll add_v_cnt;
    int last_v; // the vertex inserted recenetly
    Graph *g;
    Plex(ui n, Graph *_g)
    {
        plex_add_time = plex_remove_time = 0;
        add_v_cnt = 0;
        g = _g;
        last_v = -1;
        vertex_idx_map.resize(n);
        fill(vertex_idx_map.begin(), vertex_idx_map.end(), -1);
    }
    ~Plex() {}
    int get_in_degree(ui u)
    {
        return din[vertex_idx_map[u]];
    }
    int get_out_degree(ui u)
    {
        return dout[vertex_idx_map[u]];
    }
    int get_pd(ui u)
    {
        int idx = vertex_idx_map[u];
        return min(din[idx] + paramL, dout[idx] + paramK);
    }
    int get_n()
    {
        return vertex.size();
    }
    bool has(int u)
    {
        return vertex_idx_map[u] != -1;
    }
    void remove_vertex(ui u)
    {
        double start_remove_time = get_system_time_microsecond();
        assert(vertex_idx_map[u] == (vertex.size() - 1));
        vertex.pop_back();
        vertex_idx_map[u] = -1;
        if (last_v == u)
            last_v = -1;
        int n = vertex.size();
        for (ui i = last_edge_cnt[n]; i < edges.size(); i++)
        {
            pii &h = edges[i];
            int a = h.x, b = h.y;
            dout[a]--, din[b]--;
        }
        edges.resize(last_edge_cnt[n]);
        last_edge_cnt.pop_back();
        assert(din[n] == 0);
        assert(dout[n] == 0);
        din.pop_back();
        dout.pop_back();
        plex_remove_time += get_system_time_microsecond() - start_remove_time;
    }
    bool add_vertex(ui u)
    {
        add_v_cnt++;
        double start_add_time = get_system_time_microsecond();
        int n = vertex.size();
        assert(vertex_idx_map[u] == -1);
        vertex_idx_map[u] = n;
        vertex.push_back(u);
        din.push_back(0);
        dout.push_back(0);
        last_edge_cnt.push_back(edges.size());
        last_v = u;
        for (int i = 0; i < n; i++)
        {
            int v = vertex[i];
            if (g->exist_edge(u, v))
            {
                dout[n]++, din[i]++;
                edges.push_back({n, i});
            }
            if (g->exist_edge(v, u))
            {
                dout[i]++, din[n]++;
                edges.push_back({i, n});
            }
        }
        if (n + 1 <= min(paramL, paramK))
            return true;
        bool ok = 1;
        for (int i = 0; i <= n; i++)
        {
            if (din[i] + paramL <= n || dout[i] + paramK <= n)
            {
                ok = 0;
                break;
            }
        }
        if (!ok)
        {
            remove_vertex(u);
        }
        plex_add_time += get_system_time_microsecond() - start_add_time;
        return ok;
    }
};

#endif