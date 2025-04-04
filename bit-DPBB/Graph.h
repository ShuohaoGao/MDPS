#ifndef GRAPH_H
#define GRAPH_H

#include "LinearHeap.h"
#include "MyBitset.h"

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
    int readFromFile(string file_path)
    {
        string suffix = get_file_name_suffix(file_path);
        ifstream in(file_path);
        if (!in.is_open())
        {
            printf("Failed to open %s \n", file_path.c_str());
            fflush(stdout);
            exit(1);
        }
        int lb;
        in >> n >> m >> lb;
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
            vertex.id = u;
            in >> vertex.origin_id;
            vertex.dout = vertex.out_neighbors.size();
            vertex.din = vertex.in_neighbors.size();
            vertex.out_neighbors.shrink_to_fit();
            vertex.in_neighbors.shrink_to_fit();
        }
        in.close();
        printf("Graph init ok\n");
        fflush(stdout);
        return lb;
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

class Graph_reduced
{
public:
    int n;
    AdjacentMatrix Aout, Ain;
    vector<ui> origin_id;
    Graph_reduced(Graph &g) : n(g.n), Aout(AdjacentMatrix(g.n)), Ain(AdjacentMatrix(g.n)), origin_id(vector<ui>(g.n))
    {
        for (auto &vertex : g.vertices)
        {
            ui u = vertex.id;
            origin_id[u] = vertex.origin_id;
            for (ui v : vertex.out_neighbors)
            {
                Aout.add_edge(u, v);
                Ain.add_edge(v, u);
            }
        }
    }
    inline bool exist_edge(int a, int b)
    {
        return Aout[a][b];
    }
};

#endif