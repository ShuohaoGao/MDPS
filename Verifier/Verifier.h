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
    pii get()
    {
        size_t k=s.size(), l=s.size();
        for(int u:s)
            k=min(k, out[u].size()), l=min(l, in[u].size());
        return {s.size()-k, s.size()-l};
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
        size_t k=s.size();
        for(int u:s)
            k=min(k, neighbors[u].size());
        return s.size()-k;
    }
    bool is_correct(int k)
    {
        for (int u : s)
        {
            if (neighbors[u].size() + k < s.size() )
                return false;
        }
        return true;
    }
};

#endif