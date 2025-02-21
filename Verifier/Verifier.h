#ifndef VERIFIER_H
#define VERIFIER_H

#include "../DPHeuris/Utility.h"

class Verifier
{
    set<int> s;
    map<int, set<int>> in, out;

public:
    Verifier(set<int> _s, string file_path)
    {
        s = _s;
        if (get_file_name_suffix(file_path) == "bin")
        {
            FILE *f = fopen(file_path.c_str(), "rb");
            if (f == nullptr)
            {
                printf("Failed to open %s \n", file_path.c_str());
                exit(1);
            }
            ui size_int;
            fread(&size_int, sizeof(ui), 1, f);
            if (size_int != sizeof(ui))
            {
                printf("sizeof int is different: graph_file(%u), machine(%u)\n", size_int, (int)sizeof(ui));
                exit(1);
            }
            ui n, m;
            fread(&n, sizeof(ui), 1, f);
            fread(&m, sizeof(ui), 1, f);
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << endl;
            ui *d = new ui[n]; // d[u] is the number of u's out-neighbors
            ui *pstart = new ui[n + 1];
            ui *edge_to = new ui[m];
            fread(d, sizeof(ui), n, f);
            fread(edge_to, sizeof(ui), m, f);
            pstart[0] = 0;
            for (ui i = 1; i <= n; i++)
                pstart[i] = pstart[i - 1] + d[i - 1];

            for (ui u = 0; u < n; u++)
            {
                if (!s.count(u))
                    continue;
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    int v = edge_to[i];
                    if (s.count(v))
                    {
                        in[v].insert(u);
                        out[u].insert(v);
                    }
                }
            }

            delete[] d;
            delete[] pstart;
            delete[] edge_to;
            fclose(f);
        }
        else
        {
            FILE *f = fopen(file_path.c_str(), "r");
            if (f == nullptr)
            {
                printf("File: %s open failed!\n", file_path.c_str());
                exit(1);
            }
            int n, m;
            fscanf(f, "%d%d", &n, &m);
            while (m--)
            {
                int a, b;
                fscanf(f, "%d%d", &a, &b);
                if (!s.count(a) || !s.count(b))
                    continue;
                in[b].insert(a);
                out[a].insert(b);
            }
            fclose(f);
        }
    }
    pii get()
    {
        size_t k = s.size(), l = s.size();
        for (int u : s)
            k = min(k, out[u].size()), l = min(l, in[u].size());
        return {s.size() - k, s.size() - l};
    }
    bool is_correct(int k, int l)
    {
        for (int u : s)
        {
            if (out[u].size() + k < s.size() || in[u].size() + l < s.size())
                return false;
        }
        return true;
    }
};

class Verifier_undirected_kPlex
{
    set<int> s;
    map<int, set<int>> neighbors;

public:
    Verifier_undirected_kPlex(set<int> _s, string file_path)
    {
        s = _s;
        FILE *f = fopen(file_path.c_str(), "r");
        if (f == nullptr)
        {
            printf("File: %s open failed!\n", file_path.c_str());
            exit(1);
        }
        int n, m;
        fscanf(f, "%d%d", &n, &m);
        while (m--)
        {
            int a, b;
            fscanf(f, "%d%d", &a, &b);
            if (!s.count(a) || !s.count(b))
                continue;
            neighbors[a].insert(b);
            neighbors[b].insert(a);
        }
        fclose(f);
    }
    int get_k()
    {
        size_t k = s.size();
        for (int u : s)
            k = min(k, neighbors[u].size());
        return s.size() - k;
    }
    bool is_correct(int k)
    {
        for (int u : s)
        {
            if (neighbors[u].size() + k < s.size())
                return false;
        }
        return true;
    }
};

#endif