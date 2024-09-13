#ifndef GRAPH_H
#define GRAPH_H

#include "LinearHeap.h"

class Vertex
{
public:
    ui id;
    ui origin_id;
    ui din;
    ui dout;
    vector<ui> out_neighbors;
    vector<ui> in_neighbors;
    Vertex() : din(0), dout(0)
    {
    }
    ~Vertex()
    {
    }
    Vertex &operator=(const Vertex &other)
    {
        id = other.id;
        origin_id = other.origin_id;
        din = other.din;
        dout = other.dout;
        out_neighbors = other.out_neighbors;
        in_neighbors = other.in_neighbors;
        return *this;
    }
    bool operator==(const Vertex &other) const
    {
        return id == other.id;
    }
    bool operator<(const Vertex &other) const
    {
        return id < other.id;
    }
};

class Graph
{
public:
    ui n, m;
    vector<Vertex> vertices;
    vector<ui> query_vertices;
    Graph() : n(0), m(0)
    {
    }
    /**
     * @brief induce the subgraph G[S]
     * @param vis tool-array: make sure vis[0...n-1]=-1 before and after this function
     */
    Graph(vector<ui> &S, vector<int> &vis, const Graph &origin_g)
    {
        sort(S.begin(), S.end());
        n = S.size();
        assert(vis.size() >= n);
        // next, vis[u] is the new id of u in subgraph
        for (ui i = 0; i < n; i++)
        {
            assert(vis[S[i]] == -1);
            vis[S[i]] = i;
        }
        vertices.resize(n);
        for (ui u_origin : S)
        {
            ui u = vis[u_origin];
            auto &vertex = vertices[u];
            vertex.id = u;
            auto &origin_vertex = origin_g.vertices[u_origin];
            vertex.origin_id = origin_vertex.origin_id;
            for (ui v_origin : origin_vertex.out_neighbors)
            {
                int v = vis[v_origin];
                if (v == -1)
                    continue;
                vertex.out_neighbors.push_back(v);
                vertices[v].in_neighbors.push_back(u);
            }
        }
        m = 0;
        for (auto &vertex : vertices)
        {
            vertex.in_neighbors.shrink_to_fit();
            vertex.out_neighbors.shrink_to_fit();
            vertex.din = vertex.in_neighbors.size();
            vertex.dout = vertex.out_neighbors.size();
            m += vertex.din;
        }
        // clear the tool-array
        for (ui i = 0; i < n; i++)
        {
            vis[S[i]] = -1;
        }
    }
    ~Graph()
    {
    }
    Graph &operator=(const Graph &other)
    {
        n = other.n;
        m = other.m;
        vertices = other.vertices;
        return *this;
    }
    /**
     * @brief flip each edge, i.e., E' = {(u,v) | (v,u) \in E}
     */
    void flip()
    {
        for (auto &vertex : vertices)
        {
            swap(vertex.din, vertex.dout);
            swap(vertex.in_neighbors, vertex.out_neighbors);
        }
    }
    /**
     * @return whether (a, b) is in E
     */
    bool exist_edge(ui a, ui b)
    {
        return has(vertices[a].out_neighbors, b);
    }

    /**
     * @brief read edges from file where the file format can be ".txt" ".bin" ".out"
     *
     * ".txt" ".out" are text format
     * ".bin"  is binary format
     */
    void readFromFile(string file_path, string vertices_path)
    {
        string suffix = get_file_name_suffix(file_path);
        if (suffix == "bin")
        {
            FILE *in = fopen(file_path.c_str(), "rb");
            if (in == nullptr)
            {
                printf("Failed to open %s \n", file_path.c_str());
                exit(1);
            }
            ui size_int;
            fread(&size_int, sizeof(ui), 1, in);
            if (size_int != sizeof(ui))
            {
                printf("sizeof int is different: graph_file(%u), machine(%u)\n", size_int, (int)sizeof(ui));
                exit(1);
            }
            fread(&n, sizeof(ui), 1, in);
            fread(&m, sizeof(ui), 1, in);
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << endl;
            ui *d = new ui[n]; // d[u] is the number of u's out-neighbors
            ui *pstart = new ui[n + 1];
            ui *edge_to = new ui[m];
            fread(d, sizeof(ui), n, in);
            fread(edge_to, sizeof(ui), m, in);
            pstart[0] = 0;
            for (ui i = 1; i <= n; i++)
                pstart[i] = pstart[i - 1] + d[i - 1];
            vertices.resize(n);
            for (ui u = 0; u < n; u++)
            {
                auto &vertex = vertices[u];
                vertex.out_neighbors.resize(d[u]);
                for (ui i = pstart[u], p = 0; i < pstart[u + 1]; i++, p++)
                {
                    ui j = edge_to[i];
                    vertex.out_neighbors[p] = j;
                    vertices[j].in_neighbors.push_back(u);
                }
            }
            for (ui u = 0; u < n; u++)
            {
                auto &vertex = vertices[u];
                vertex.id = vertex.origin_id = u;
                vertex.dout = vertex.out_neighbors.size();
                vertex.din = vertex.in_neighbors.size();
                vertex.in_neighbors.shrink_to_fit();
            }
            delete[] d;
            delete[] pstart;
            delete[] edge_to;
        }
        else // default graph file format: n m \n edges
        {
            ifstream in(file_path);
            if (!in.is_open())
            {
                printf("Failed to open %s \n", file_path.c_str());
                fflush(stdout);
                exit(1);
            }
            in >> n >> m;
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << endl;
            vector<pii> edges(m);
            ui idx = 0;
            for (ui i = 0; i < m; i++)
            {
                ui a, b;
                in >> a >> b;
                if (a == b)
                    continue;
                assert(a < n && b < n);
                edges[idx++] = {a, b};
            }
            edges.resize(idx);
            unique_pii(edges, n);
            m = edges.size();
            vertices.resize(n);
            for (auto &h : edges)
            {
                ui a = h.x, b = h.y;
                vertices[a].out_neighbors.push_back(b);
                vertices[b].in_neighbors.push_back(a);
            }
            for (ui u = 0; u < n; u++)
            {
                auto &vertex = vertices[u];
                vertex.id = vertex.origin_id = u;
                vertex.dout = vertex.out_neighbors.size();
                vertex.din = vertex.in_neighbors.size();
                vertex.out_neighbors.shrink_to_fit();
                vertex.in_neighbors.shrink_to_fit();
            }
            in.close();
        }
        printf("Graph init ok\n");
        read_vertices(vertices_path);
        fflush(stdout);
    }

    /**
     * @brief read the query set
     */
    void read_vertices(string vertices_path)
    {
        ifstream in(vertices_path);
        if (!in.is_open())
        {
            printf("Failed to open %s \n", vertices_path.c_str());
            exit(1);
        }
        int sz;
        in >> sz;
        while (sz--)
        {
            int a;
            in >> a;
            if (a >= n)
            {
                printf("Wrong: make sure each query vertex v < n !!\n");
                exit(1);
            }
            query_vertices.push_back(a);
        }
        printf("query vertex set (size=%u):", query_vertices.size());
        for (ui x : query_vertices)
            printf(" %u", x);
        printf("\n");
    }

    /**
     * @brief given a permutation of {0,...,n-1}, resort V
     * @param vertex_id v in new graph corresponds to vertex_id[v] in old graph
     */
    void re_sort(vector<ui> &vertex_id)
    {
        assert(vertex_id.size() == n);
        vector<ui> id_in_new(n); // v in old graph corresponds to id_in_new[v] in new graph
        for (ui i = 0; i < n; i++)
        {
            id_in_new[vertex_id[i]] = i;
        }
        for (ui &v : query_vertices)
            v = id_in_new[v];
        for (auto &s : vertices)
        {
            s.id = id_in_new[s.id];
            for (auto &v : s.out_neighbors)
            {
                v = id_in_new[v];
            }
            sort(s.out_neighbors.begin(), s.out_neighbors.end());
            for (auto &v : s.in_neighbors)
            {
                v = id_in_new[v];
            }
            sort(s.in_neighbors.begin(), s.in_neighbors.end());
        }
        sort(vertices.begin(), vertices.end());
    }

    /**
     * @brief GraphReduction in preprocessing stage
     */
    void graph_reduction(int lb, int k, int l)
    {
        if (!n)
            return;
        int pre_n = n;
        reduce_two_hops(lb, k, l);
        if (!n)
            return;
        vertex_reduction(lb, k, l);
        if (!n)
            return;
        reduce_based_on_hereditary(k, l);
        if (!n)
            return;
        for (ui i = 0; i < query_vertices.size(); i++)
        {
            ui u = query_vertices[i];
            reduce_based_on_common_neighbors(u, lb, k, l);
            if (!n)
                return;
        }
        edge_reduction(lb, k, l);
        vertex_reduction(lb, k, l);
        if (!n)
            return;
        reduce_based_on_hereditary(k, l);
        if (n <= lb)
        {
            n = m = 0;
            vertices.clear();
            return;
        }
        if (pre_n > n)
            graph_reduction(lb, k, l);
    }

    /**
     * @brief given query set Q, if G[Q+u] is not a DPlex, then remove u
     */
    void reduce_based_on_hereditary(int k, int l)
    {
        if (query_vertices.size() < k)
            return;
        auto &S = query_vertices;
        vector<int> state(n);
        const int IN_S = 2;
        const int Removed = 1;
        for (ui u : S)
            state[u] = IN_S;
        vector<int> out_neighbor_in_S(n);
        vector<int> in_neighbor_in_S(n);
        for (ui u : S)
        {
            for (ui v : vertices[u].out_neighbors)
                in_neighbor_in_S[v]++;
            for (ui v : vertices[u].in_neighbors)
                out_neighbor_in_S[v]++;
        }
        vector<ui> critical_out, critical_in;
        for (ui u : S)
        {
            int dout = out_neighbor_in_S[u];
            if (dout + k == S.size())
                critical_out.push_back(u);
            int din = in_neighbor_in_S[u];
            if (din + l == S.size())
                critical_in.push_back(u);
        }
        for (ui v = 0; v < n; v++)
        {
            if (state[v] == IN_S)
                continue;
            assert(!state[v]);
            if (out_neighbor_in_S[v] + k >= S.size() + 1 && in_neighbor_in_S[v] + l >= S.size() + 1)
            {
                for (ui u : critical_out)
                    if (!has(vertices[u].out_neighbors, v))
                    {
                        state[v] = Removed;
                        break;
                    }
                for (ui u : critical_in)
                    if (!has(vertices[u].in_neighbors, v))
                    {
                        state[v] = Removed;
                        break;
                    }
            }
            else
            {
                state[v] = Removed;
            }
        }
        for (ui u : S)
            state[u] = 0;
        remove_given_vertices(state);
    }

    /**
     * @brief remove edges that can not appear in DPlex larger than lb
     */
    void edge_reduction(int lb, int k, int l)
    {
        vector<int> state(n);
        const int OUT = 2, IN = 4, IN_OUT = OUT | IN;
        vector<pii> rest_edges;
        for (auto &vertex : vertices)
        {
            ui u = vertex.id;
            auto &out_neighbors = vertex.out_neighbors;
            auto &in_neighbors = vertex.in_neighbors;
            for (ui v : out_neighbors)
                state[v] |= OUT;
            for (ui v : in_neighbors)
                state[v] |= IN;
            // we just need to consider u->v or u<->v
            for (ui v : out_neighbors)
            {
                if (state[v] == IN_OUT)
                {
                    if (lb >= 2 * k)
                    {
                        int cnt = 0;
                        for (ui w : vertices[v].out_neighbors)
                            if (state[w] & OUT)
                                cnt++;
                        if (cnt + 2 * k <= lb)
                            continue;
                    }
                    if (lb >= 2 * l)
                    {
                        int cnt = 0;
                        for (ui w : vertices[v].in_neighbors)
                            if (state[w] & IN)
                                cnt++;
                        if (cnt + 2 * l <= lb)
                            continue;
                    }
                }
                else
                {
                    if (lb >= 2 * k - 1)
                    {
                        int cnt = 0;
                        for (ui w : vertices[v].out_neighbors)
                            if (state[w] & OUT)
                                cnt++;
                        if (cnt + 2 * k - 1 <= lb)
                            continue;
                    }
                    if (lb >= 2 * l - 1)
                    {
                        int cnt = 0;
                        for (ui w : vertices[v].in_neighbors)
                            if (state[w] & IN)
                                cnt++;
                        if (cnt + 2 * l - 1 <= lb)
                            continue;
                    }
                }
                // (u,v) can not be removed
                rest_edges.push_back({u, v});
            }
            for (ui v : out_neighbors)
                state[v] = 0;
            for (ui v : in_neighbors)
                state[v] = 0;
        }
        if (m == rest_edges.size())
            return;
        sort(rest_edges.begin(), rest_edges.end());
        m = rest_edges.size();
        for (auto &vertex : vertices)
        {
            vertex.out_neighbors.clear();
            vertex.in_neighbors.clear();
        }
        for (pii h : rest_edges)
        {
            ui a = h.x, b = h.y;
            vertices[a].out_neighbors.push_back(b);
            vertices[b].in_neighbors.push_back(a);
        }
        for (auto &vertex : vertices)
        {
            vertex.dout = vertex.out_neighbors.size();
            vertex.din = vertex.in_neighbors.size();
        }
    }

    /**
     * @brief remove vertices that can not appear in DPlex larger than 2k-2
     * i.e., we remove u if pd[u] <= 2k-2
     */
    void vertex_reduction(int lb, int k, int l)
    {
        vector<int> in_queue(n);
        queue<ui> q;
        for (auto &vertex : vertices)
        {
            ui u = vertex.id;
            ui pseudo_degree = min(vertex.din + l, vertex.dout + k);
            if (pseudo_degree <= lb)
            {
                in_queue[u] = 1;
                q.push(u);
            }
        }
        if (!q.size())
            return;
        while (q.size())
        {
            ui u = q.front();
            q.pop();
            for (ui v : vertices[u].out_neighbors)
            {
                if (in_queue[v])
                    continue;
                vertices[v].din--;
                if (vertices[v].din + l <= lb)
                {
                    q.push(v);
                    in_queue[v] = 1;
                }
            }
            for (ui v : vertices[u].in_neighbors)
            {
                if (in_queue[v])
                    continue;
                vertices[v].dout--;
                if (vertices[v].dout + k <= lb)
                {
                    q.push(v);
                    in_queue[v] = 1;
                }
            }
        }
        // rebuild graph
        remove_given_vertices(in_queue);
    }

    /**
     * @brief remove vertices based on common-neighbor
     * @param u current graph aims to find DPlexes containing u
     */
    void reduce_based_on_common_neighbors(int u, int lb, int k, int l)
    {
        // 1. if 1) u->v but not v->u, and |Nout(u)\cap Nout(v)|+2k-1 <=lb or
        //       2) u<->u and |Nout(u)\cap Nout(v)|+2k <=lb or
        //       3) u is not connected to v and |Nout(u)\cap Nout(v)|+2k-2 <=lb
        //    then we can remove v
        // 2. common in-neighbors
        if (lb < 2 * k - 2)
            return;
        vector<int> state(n);
        const int OUT = 2, IN = 4, IN_OUT = OUT | IN;
        const int Removed = 1;
        for (ui v : vertices[u].out_neighbors)
            state[v] |= OUT;
        for (ui v : vertices[u].in_neighbors)
            state[v] |= IN;
        for (ui v = 0; v < n; v++)
        {
            if (v == u)
                continue;
            if (state[v] == IN_OUT)
            {
                if (lb >= 2 * k)
                {
                    int cnt = 0;
                    for (ui w : vertices[v].out_neighbors)
                        if (state[w] & OUT)
                            cnt++;
                    if (cnt + 2 * k <= lb)
                    {
                        state[v] = Removed;
                        continue;
                    }
                }
                if (lb >= 2 * l)
                {
                    int cnt = 0;
                    for (ui w : vertices[v].in_neighbors)
                        if (state[w] & IN)
                            cnt++;
                    if (cnt + 2 * l <= lb)
                    {
                        state[v] = Removed;
                        continue;
                    }
                }
            }
            else if (state[v] == OUT | state[v] == IN)
            {
                if (lb >= 2 * k - 1)
                {
                    int cnt = 0;
                    for (ui w : vertices[v].out_neighbors)
                        if (state[w] & OUT)
                            cnt++;
                    if (cnt + 2 * k - 1 <= lb)
                    {
                        state[v] = Removed;
                        continue;
                    }
                }
                if (lb >= 2 * l - 1)
                {
                    int cnt = 0;
                    for (ui w : vertices[v].in_neighbors)
                        if (state[w] & IN)
                            cnt++;
                    if (cnt + 2 * l - 1 <= lb)
                    {
                        state[v] = Removed;
                        continue;
                    }
                }
            }
            else
            {
                assert(!state[v]);
                if (lb >= 2 * k - 2)
                {
                    int cnt = 0;
                    for (ui w : vertices[v].out_neighbors)
                        if (state[w] & OUT)
                            cnt++;
                    if (cnt + 2 * k - 2 <= lb)
                    {
                        state[v] = Removed;
                        continue;
                    }
                }
                if (lb >= 2 * l - 2)
                {
                    int cnt = 0;
                    for (ui w : vertices[v].in_neighbors)
                        if (state[w] & IN)
                            cnt++;
                    if (cnt + 2 * l - 2 <= lb)
                    {
                        state[v] = Removed;
                        continue;
                    }
                }
            }
        }
        for (ui v = 0; v < n; v++)
            if ((state[v] & Removed) == 0)
                state[v] = 0;
        remove_given_vertices(state);
    }

    /**
     * @brief if |S|>=2k-1, then each pair vertices has at least one common out-neighbor
     */
    void reduce_two_hops(int lb, int k, int l)
    {
        if (lb < 2 * k - 2)
            return;
        vector<int> vis(n, 1);
        int u = query_vertices[0];
        vis[u] = 0;
        for (ui v : vertices[u].in_neighbors)
            vis[v] = 0;
        for (ui v : vertices[u].out_neighbors)
        {
            vis[v] = 0;
            for (ui w : vertices[v].in_neighbors)
                vis[w] = 0;
        }
        remove_given_vertices(vis);
    }

    /**
     * @brief 1) remove vertices 2) rebuild graph 3) re-map the query set
     */
    void remove_given_vertices(vector<int> &vertex_removed)
    {
        for (ui v : query_vertices)
            if (vertex_removed[v])
            {
                n = m = 0;
                vertices.clear();
                return;
            }
        ui idx = 0;
        auto &vertex_map = vertex_removed;
        for (ui i = 0; i < n; i++)
        {
            if (vertex_removed[i])
                vertex_map[i] = -1;
            else
                vertex_map[i] = idx++;
        }
        for (ui &v : query_vertices)
            v = vertex_map[v];
        vector<Vertex> new_vertices(idx);
        for (ui u_origin = 0; u_origin < n; u_origin++)
        {
            if (vertex_map[u_origin] == -1)
                continue;
            ui u = vertex_map[u_origin];
            auto &vertex = new_vertices[u];
            vertex.id = u;
            auto &origin_vertex = vertices[u_origin];
            vertex.origin_id = origin_vertex.origin_id;
            for (ui v_origin : origin_vertex.out_neighbors)
            {
                int v = vertex_map[v_origin];
                if (v == -1)
                    continue;
                vertex.out_neighbors.push_back(v);
                new_vertices[v].in_neighbors.push_back(u);
            }
        }
        swap(new_vertices, vertices);
        n = idx;
        m = 0;
        for (auto &vertex : vertices)
        {
            vertex.in_neighbors.shrink_to_fit();
            vertex.out_neighbors.shrink_to_fit();
            vertex.din = vertex.in_neighbors.size();
            vertex.dout = vertex.out_neighbors.size();
            m += vertex.din;
        }
    }

    /**
     * @brief compute a maximal DPlex containing the query set
     * @param ground_truth if we find a larger DPlex, then we update it
     */
    void heuristic_and_preprocessing(set<ui> &ground_truth, int k, int l)
    {
        int lb = k + l - 2;
        graph_reduction(lb, k, l);
        if (n <= lb)
            return;
        if (!is_DPlex(query_vertices, k, l))
        {
            vertices.clear();
            n = m = 0;
            return;
        }
#ifdef NO_prepro
        {
            vector<ui> S = query_vertices;
            extend(S, k, l);
        }
        return;
#endif
        // fast heuristic
        {
            vector<ui> S = query_vertices;
            extend(S, k, l);
            if (S.size() > lb)
            {
                lb = S.size();
                printf("Find a larger DPlex (size=%u)\n", S.size());
                ground_truth.clear();
                for (ui u : S)
                    ground_truth.insert(vertices[u].origin_id);
                assert(ground_truth.size() == S.size());
                graph_reduction(lb, k, l);
                printf("reduced graph: n= %u m= %u\n", n, m);
                fflush(stdout);
            }
        }
        // strong heuris
        while (n > lb)
        {
#ifdef FAST
            break;
#endif
            if (!is_DPlex(query_vertices, k, l))
            {
                vertices.clear();
                n = m = 0;
                return;
            }
            // extend n times
            vector<bool> vis(n);
            for (int u : query_vertices)
                vis[u] = 1;
            bool lb_updated = 0;
            for (ui i = 0; i < n; i++)
            {
                if (vis[i])
                    continue;
                vector<ui> S = query_vertices;
                S.push_back(i);
                extend(S, k, l);
                if (S.size() <= lb)
                    continue;
                lb = S.size();
                lb_updated = 1;
                printf("Find a larger DPlex (size=%u)\n", S.size());
                ground_truth.clear();
                for (ui u : S)
                    ground_truth.insert(vertices[u].origin_id);
                assert(ground_truth.size() == S.size());
                // reduce
                graph_reduction(lb, k, l);
                printf("reduced graph: n= %u m= %u\n", n, m);
                fflush(stdout);
                break; // reduce at once
            }
            if (!lb_updated) // repeat until lb is unchanged
                break;
        }
        // reduce again
        // int pre_n = n;
        graph_reduction(lb, k, l);
        // if(n < pre_n)
        // {
        //     printf("reduced graph: n= %u m= %u\n", n, m);
        //     fflush(stdout);
        // }
    }

    /**
     * @brief StrongHeuris
     * @param two_hop if true, we set the candidate vertex set as N^{\leq 2 out}(S[0])
     */
    void extend(vector<ui> &S, int k, int l, bool hop2 = false)
    {
        vector<int> state(n);
        const int IN_S = 2, IN_C = 4;
        for (ui u : S)
            state[u] = IN_S;
        vector<int> out_neighbor_in_S(n);
        vector<int> in_neighbor_in_S(n);
        for (ui u : S)
        {
            for (ui v : vertices[u].out_neighbors)
                in_neighbor_in_S[v]++;
            for (ui v : vertices[u].in_neighbors)
                out_neighbor_in_S[v]++;
        }
        vector<ui> C;
        for (ui i = 0; i < n; i++)
            if (state[i] == 0)
            {
                if (out_neighbor_in_S[i] + k >= S.size() + 1 && in_neighbor_in_S[i] + l >= S.size() + 1)
                {
                    state[i] = IN_C;
                    C.push_back(i);
                }
            }
        random_shuffle(C.begin(), C.end());
        while (C.size())
        {
            int sel = -1, max_sum = -1, idx = -1;
            for (ui i = 0; i < C.size(); i++)
            {
                int u = C[i];
                int sum = out_neighbor_in_S[u] + in_neighbor_in_S[u];
                if (sum > max_sum) // we prefer the vertex highly connected to S
                {
                    sel = u;
                    max_sum = sum;
                    idx = i;
                }
                else if (sum == max_sum) // we also like vertex with greater degree
                {
                    int pd = min(vertices[u].dout + k, vertices[u].din + l);
                    int pd_pre = min(vertices[sel].dout + k, vertices[sel].din + l);
                    if (pd > pd_pre)
                    {
                        sel = u;
                        max_sum = sum;
                        idx = i;
                    }
                }
            }
            assert(sel != -1);
            // remove sel from C
            swap(C[idx], C[C.size() - 1]);
            C.pop_back();
            assert(out_neighbor_in_S[sel] + k >= S.size() + 1 && in_neighbor_in_S[sel] + l >= S.size() + 1);
            bool ok = 1; // check if S+sel is DPlex
            S.push_back(sel);
            for (ui v : vertices[sel].out_neighbors)
                in_neighbor_in_S[v]++;
            for (ui v : vertices[sel].in_neighbors)
                out_neighbor_in_S[v]++;
            for (ui u : S)
            {
                int dout = out_neighbor_in_S[u];
                int din = in_neighbor_in_S[u];
                if (min(dout + k, din + l) < S.size())
                {
                    ok = 0;
                    break;
                }
            }
            if (!ok)
            {
                S.pop_back();
                for (ui v : vertices[sel].out_neighbors)
                    in_neighbor_in_S[v]--;
                for (ui v : vertices[sel].in_neighbors)
                    out_neighbor_in_S[v]--;
            }
            else
            {
                // reduce C
                ui C_sz = 0;
                for (ui i = 0; i < C.size(); i++)
                {
                    ui u = C[i];
                    int dout = out_neighbor_in_S[u];
                    int din = in_neighbor_in_S[u];
                    if (min(dout + k, din + l) < S.size() + 1)
                        continue;
                    C[C_sz++] = u;
                }
                C.resize(C_sz);
            }
        }
    }

    /**
     * @return true if G[S] is a (k,l)-plex
     */
    bool is_DPlex(vector<ui> &S, int k, int l)
    {
        unordered_map<ui, int> dout, din;
        for (ui u : S)
        {
            for (ui v : S)
            {
                if (v == u)
                    break;
                if (has(vertices[u].out_neighbors, v))
                {
                    dout[u]++;
                    din[v]++;
                }
                if (has(vertices[u].in_neighbors, v))
                {
                    din[u]++;
                    dout[v]++;
                }
            }
        }
        for (ui u : S)
        {
            int pd = min(dout[u] + k, din[u] + l);
            if (pd < S.size())
                return false;
        }
        return true;
    }

    /**
     * @brief whether S is DPlex and S is maximal
     */
    bool is_maximal(vector<ui> &S, int k, int l)
    {
        if (!is_DPlex(S, k, l))
            return false;
        vector<bool> vis(n);
        for (ui v : S)
            vis[v] = 1;
        for (ui i = 0; i < n; i++)
        {
            if (vis[i])
                continue;
            S.push_back(i);
            bool ret = is_DPlex(S, k, l);
            S.pop_back();
            if (ret)
                return false;
        }
        return true;
    }
};

#endif