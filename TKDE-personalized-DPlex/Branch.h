#ifndef BRANCH_H
#define BRANCH_H

#include "Graph.h"

/**
 * @brief bnb based on maintaining infomation dynamically, i.e., it needs to record the changes and rollback before return
 */
class Branch
{
    using Set = MyBitset;

public:
    Graph_reduced g;
    vector<ui> query_vertices;
    int k, l;
    set<ui> *ground_truth;
    int lb;
    int v_just_add;

    ll dfs_cnt;
    double bnb_time;
    double kPlexT_time;

    Branch(Graph &_g, int paramK, int paramL, set<ui> *truth) : g(_g), k(paramK), l(paramL), ground_truth(truth),
                                                                query_vertices(_g.query_vertices),
                                                                dfs_cnt(0), bnb_time(0), kPlexT_time(0)
    {
        lb = ground_truth->size();
        lb = max(lb, k + l - 2);
    }
    ~Branch() {}

    /**
     * @brief entrance of branch-and-bound procedure
     */
    void run()
    {
        Timer t;
        Set S(g.n);
        for (ui u : query_vertices)
            S.set(u);
        Set C = S;
        C.flip();
        bnb(S, C);
        bnb_time = t.get_time();

        print_results();
    }
    /**
     * @brief print log
     */
    void print_results()
    {
        print_module_time("kPlexT", kPlexT_time);
        printf("dfs-cnt= %lld use-time: %.4lf s\n", dfs_cnt, bnb_time / 1e6);
    }

    /**
     * @brief recursive branch-and-bound procedure: aim to find largest DPlex containing S in G[S+C]
     */
    void bnb(Set &S, Set &C)
    {
        dfs_cnt++;
        if (S.size() > lb)
            update_lb_S(S);

        reduce_C(S, C);

        kPlexT_reduce(S, C);

        int ub = upper_bound(S, C);
        if (ub <= lb)
            return;

        // generate sub-branches
        int pivot = select_pivot(S, C);
        // branch 1: add pivot to S
        {
            auto new_S = S, new_C = C;
            new_C.reset(pivot);
            new_S.set(pivot);
            v_just_add = pivot;
            bnb(new_S, new_C);
        }
        // branch 2: remove pivot from C
        {
            C.reset(pivot);
            auto V = C;
            V |= S;
            if (V.size() <= lb)
                return;
            if (is_DPlex(V))
            {
                update_lb_S(V);
                return;
            }
            v_just_add = -1;
            bnb(S, C);
        }
    }

    // void extend(Set &S, Set &C)
    // {
    //     vector<int> vertices;
    //     for (int u : C)
    //         vertices.push_back(u);
    //     // random_shuffle(vertices.begin(), vertices.end());
    //     auto V = S;
    //     auto copy_C = C;
    //     for (int u : S)
    //     {
    //         if (S.intersect(g.Aout[u]) + k == S.size())
    //         {
    //             copy_C &= g.Aout[u];
    //         }
    //         if (S.intersect(g.Ain[u]) + l == S.size())
    //             copy_C &= g.Ain[u];
    //     }
    //     for (int u : vertices)
    //     {
    //         if (!copy_C[u])
    //             continue;
    //         int out_cnt = V.intersect(g.Aout[u]), in_cnt = V.intersect(g.Ain[u]);
    //         if (out_cnt + k < V.size() + 1 || in_cnt + l < V.size() + 1)
    //             continue;
    //         for (int v : V)
    //         {
    //             if (!g.Aout[v][u])
    //             {
    //                 if (V.intersect(g.Aout[v]) + k == V.size() + 1)
    //                 {
    //                     copy_C &= g.Aout[v];
    //                 }
    //             }
    //             if (!g.Aout[u][v])
    //             {
    //                 if (V.intersect(g.Ain[v]) + l == V.size() + 1)
    //                 {
    //                     copy_C &= g.Ain[v];
    //                 }
    //             }
    //         }
    //         V.set(u);
    //         if (out_cnt + k == V.size())
    //             copy_C &= g.Aout[u];
    //         if (in_cnt + l == V.size())
    //             copy_C &= g.Ain[u];
    //     }
    //     if (V.size() > lb)
    //     {
    //         update_lb_S(V);
    //         puts("---------");
    //     }
    // }

    bool is_DPlex(Set &S)
    {
        for (int v : S)
        {
            int pd = min(S.intersect(g.Aout[v]) + k, S.intersect(g.Ain[v]) + l);
            if (pd < S.size())
                return 0;
        }
        return 1;
    }

    /**
     * @brief S+C is DPlex and |S|+|C|>lb
     */
    void update_lb_S_C(Set &S, Set &C)
    {
        assert(S.size() + C.size() > lb);
        lb = S.size() + C.size();
        ground_truth->clear();
        for (int v : S)
            ground_truth->insert(g.origin_id[v]);
        for (int v : C)
            ground_truth->insert(g.origin_id[v]);
        assert(ground_truth->size() == lb);
        printf("Find a larger DPlex (size=%d)\n", lb);
    }

    /**
     * @brief |S|>lb
     */
    void update_lb_S(Set &S)
    {
        assert(S.size() > lb);
        lb = S.size();
        ground_truth->clear();
        for (int v : S)
            ground_truth->insert(g.origin_id[v]);
        assert(ground_truth->size() == lb);
        printf("Find a larger DPlex (size=%d)\n", lb);
    }

    /**
     * @brief select a vertex with minimum pseudo-degree in G[S+C]
     * @return pivot in C
     */
    int select_pivot(Set &S, Set &C)
    {
        int sel = -1, min_pd = INF;
        auto V = C;
        V |= S;
        for (int u : C)
        {
            int dout = V.intersect(g.Aout[u]);
            int din = V.intersect(g.Ain[u]);
            int pd = min(dout + k, din + l);
            if (pd < min_pd)
            {
                sel = u;
                min_pd = pd;
            }
        }
        return sel;
    }

    /**
     * @brief reduce u from C if 1) S+u is not a DPlex or 2) pd(u)<=lb
     */
    void reduce_C(Set &S, Set &C)
    {
        auto V = C;
        V |= S;
        for (int v : S)
        {
            if (S.intersect(g.Aout[v]) + k == S.size())
            {
                C &= g.Aout[v];
            }
            if (S.intersect(g.Ain[v]) + l == S.size())
            {
                C &= g.Ain[v];
            }
        }
        for (int u : C)
        {
            int dout = V.intersect(g.Aout[u]);
            int din = V.intersect(g.Ain[u]);
            int pd = min(dout + k, din + l);
            if (pd <= lb)
            {
                C.reset(u);
                continue;
            }
            if (S.intersect(g.Aout[u]) + k <= S.size() || S.intersect(g.Ain[u]) + l <= S.size())
            {
                C.reset(u);
                continue;
            }
        }
    }

    void kPlexT_reduce(Set &S, Set &C)
    {
        if (S.size() <= 1)
            return;
        Timer t;
        {
            Set Sout(g.n);
            auto V = S;
            V |= C;
            int sup_out = 0;
            for (int u : S)
            {
                if (V.size() - V.intersect(g.Aout[u]) > k)
                {
                    Sout.set(u);
                    sup_out += k - (S.size() - S.intersect(g.Aout[u]));
                }
            }
            vector<pii> cnt_and_vertex;
            for (int u : C)
            {
                int cnt = Sout.size() - Sout.intersect(g.Ain[u]);
                cnt_and_vertex.push_back({cnt, u});
            }
            sort(cnt_and_vertex.begin(), cnt_and_vertex.end());
            for (int v : C)
            {
                int ub_v = S.size() + 1;
                int non_out_v = S.size() - S.intersect(g.Aout[v]) + 1;
                int non_in_v = S.size() - S.intersect(g.Ain[v]) + 1;
                int sup = sup_out - (Sout.size() - Sout.intersect(g.Ain[v]));
                for (auto &h : cnt_and_vertex)
                {
                    int cnt = h.x, w = h.y;
                    if (ub_v > lb || cnt > sup)
                        break;
                    if (w == v || !C[w])
                        continue;
                    if (!g.Aout[v][w])
                    {
                        if (non_out_v == k)
                            continue;
                        if (!g.Ain[v][w])
                        {
                            if (non_in_v == l)
                                continue;
                            non_in_v++;
                        }
                        non_out_v++;
                    }
                    else if (!g.Ain[v][w])
                    {
                        if (non_in_v == l)
                            continue;
                        non_in_v++;
                    }
                    sup -= cnt;
                    ub_v++;
                }
                if (ub_v <= lb)
                    C.reset(v);
            }
        }
        {
            Set Sin(g.n);
            auto V = S;
            V |= C;
            int sup_in = 0;
            for (int u : S)
            {
                if (V.size() - V.intersect(g.Ain[u]) > l)
                {
                    Sin.set(u);
                    sup_in += l - (S.size() - S.intersect(g.Ain[u]));
                }
            }
            vector<pii> cnt_and_vertex;
            for (int u : C)
            {
                int cnt = Sin.size() - Sin.intersect(g.Aout[u]);
                cnt_and_vertex.push_back({cnt, u});
            }
            sort(cnt_and_vertex.begin(), cnt_and_vertex.end());

            for (int v : C)
            {
                int ub_v = S.size() + 1;
                int non_out_v = S.size() - S.intersect(g.Aout[v]) + 1;
                int non_in_v = S.size() - S.intersect(g.Ain[v]) + 1;
                int sup = sup_in - (Sin.size() - Sin.intersect(g.Aout[v]));
                for (auto &h : cnt_and_vertex)
                {
                    int cnt = h.x, w = h.y;
                    if (ub_v > lb || cnt > sup)
                        break;
                    if (w == v || !C[w])
                        continue;
                    if (!g.Aout[v][w])
                    {
                        if (non_out_v == k)
                            continue;
                        if (!g.Ain[v][w])
                        {
                            if (non_in_v == l)
                                continue;
                            non_in_v++;
                        }
                        non_out_v++;
                    }
                    else if (!g.Ain[v][w])
                    {
                        if (non_in_v == l)
                            continue;
                        non_in_v++;
                    }
                    sup -= cnt;
                    ub_v++;
                }
                if (ub_v <= lb)
                    C.reset(v);
            }
        }
        kPlexT_time += t.get_time();
    }

    void binary_reduce(Set &V, int min_pd, Set &C)
    {
        queue<int> q;
        Set removed(g.n);
        for (int u : V)
        {
            if (min(V.intersect(g.Aout[u]) + k, V.intersect(g.Ain[u]) + l) < min_pd)
            {
                q.push(u);
                removed.set(u);
            }
        }
        while (q.size())
        {
            int u = q.front();
            q.pop();
            V.reset(u);
            if (C[u])
                C.reset(u);
            auto out_neighbor = g.Aout[u];
            out_neighbor &= V;
            for (int v : out_neighbor)
            {
                if (removed[v])
                    continue;
                if (V.intersect(g.Ain[v]) + l < min_pd)
                {
                    removed.set(v);
                    q.push(v);
                }
            }

            auto in_neighbor = g.Ain[u];
            in_neighbor &= V;
            for (int v : in_neighbor)
            {
                if (removed[v])
                    continue;
                if (V.intersect(g.Aout[v]) + k < min_pd)
                {
                    removed.set(v);
                    q.push(v);
                }
            }
        }
    }

    int upper_bound(Set &S, Set &C)
    {
        if (S.size() + C.size() <= lb)
            return lb;
        if (part_out(S, C) <= lb)
            return lb;
        return part_in(S, C);
    }

    int part_out(Set &S, Set &C)
    {
        auto copy_S = S, copy_C = C;
        int ub = 0;
        while (copy_S.size())
        {
            int sel = -1, size = 0, ub_cnt = 0;
            for (int v : copy_S)
            {
                int sz = C.size() - C.intersect(g.Aout[v]);
                int cnt = k - (S.size() - S.intersect(g.Aout[v]));
                if (sz <= cnt)
                {
                    copy_S.reset(v);
                }
                else
                {
                    if (sel == -1 || size * cnt < sz * ub_cnt)
                    {
                        sel = v;
                        size = sz;
                        ub_cnt = cnt;
                    }
                }
            }
            if (sel == -1)
                break;
            copy_S.reset(sel);
            ub += ub_cnt;
            copy_C &= g.Aout[sel];
        }
        int min_pd = lb + 1 - ub;
        copy_C |= S;
        if (min_pd > k)
        {
            // binary_reduce(copy_C, min_pd, C);
            for (int u : C)
            {
                if (copy_C[u])
                {
                    if (copy_C.intersect(g.Aout[u]) + k < min_pd)
                    {
                        copy_C.reset(u);
                        C.reset(u);
                    }
                    else if (copy_C.intersect(g.Ain[u]) + l < min_pd)
                    {
                        copy_C.reset(u);
                        C.reset(u);
                    }
                }
                else
                {
                    if (copy_C.intersect(g.Aout[u]) + k < min_pd + 1)
                    {
                        C.reset(u);
                    }
                    else if (copy_C.intersect(g.Ain[u]) + l < min_pd + 1)
                    {
                        C.reset(u);
                    }
                }
            }
        }
        ub += copy_C.size();
        return ub;
    }

    int part_in(Set &S, Set &C)
    {
        auto copy_S = S, copy_C = C;
        int ub = 0;
        while (copy_S.size())
        {
            int sel = -1, size = 0, ub_cnt = 0;
            for (int v : copy_S)
            {
                int sz = C.size() - C.intersect(g.Ain[v]);
                int cnt = l - (S.size() - S.intersect(g.Ain[v]));
                if (sz <= cnt)
                {
                    copy_S.reset(v);
                }
                else
                {
                    if (sel == -1 || size * cnt < sz * ub_cnt)
                    {
                        sel = v;
                        size = sz;
                        ub_cnt = cnt;
                    }
                }
            }
            if (sel == -1)
                break;
            copy_S.reset(sel);
            ub += ub_cnt;
            copy_C &= g.Ain[sel];
        }
        int min_pd = lb + 1 - ub;
        copy_C |= S;
        if (min_pd > k)
        {
            // binary_reduce(copy_C, min_pd, C);
            for (int u : C)
            {
                if (copy_C[u])
                {
                    if (copy_C.intersect(g.Aout[u]) + k < min_pd)
                    {
                        copy_C.reset(u);
                        C.reset(u);
                    }
                    else if (copy_C.intersect(g.Ain[u]) + l < min_pd)
                    {
                        copy_C.reset(u);
                        C.reset(u);
                    }
                }
                else
                {
                    if (copy_C.intersect(g.Aout[u]) + k < min_pd + 1)
                    {
                        C.reset(u);
                    }
                    else if (copy_C.intersect(g.Ain[u]) + l < min_pd + 1)
                    {
                        C.reset(u);
                    }
                }
            }
        }
        ub += copy_C.size();
        return ub;
    }
};

#endif