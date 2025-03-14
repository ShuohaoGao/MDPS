#ifndef GRAPH_H
#define GRAPH_H

#include "Utility.h"

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
     * @brief read edges from file
     * @return lb
     */
    int readFromFile(string file_path, string vertices_path)
    {
        ifstream in(file_path);
        if (!in.is_open())
        {
            printf("Failed to open %s \n", file_path.c_str());
            fflush(stdout);
            exit(1);
        }
        int lb;
        in >> n >> m >> lb;
        cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << " lb= " << lb << endl;
        vector<pii> edges(m);
        ui idx = 0;
        for (ui i = 0; i < m; i++)
        {
            ui a, b;
            in >> a >> b;
            assert(a != b);
            assert(a < n && b < n);
            edges[idx++] = {a, b};
        }
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
            in >> vertex.origin_id; // read origin_id from file
            vertex.dout = vertex.out_neighbors.size();
            vertex.din = vertex.in_neighbors.size();
            vertex.out_neighbors.shrink_to_fit();
            vertex.in_neighbors.shrink_to_fit();
        }
        in.close();
        printf("Graph init ok\n");
        if (vertices_path.size() > 0)
            read_vertices(vertices_path);
        fflush(stdout);
        return lb;
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
        set<ui> temp;
        int sz;
        in >> sz;
        while (sz--)
        {
            int a;
            in >> a;
            temp.insert(a);
        }
        printf("query vertex set (size=%u):", temp.size());
        for (ui x : temp)
            printf(" %u", x);
        printf("\n");
        for (ui u = 0; u < n; u++)
        {
            if (temp.count(vertices[u].origin_id))
                query_vertices.push_back(u);
        }
        if (query_vertices.size() < temp.size())
        {
            printf("MaxDPlex is found during DPHeuris-Q\n");
            exit(0);
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