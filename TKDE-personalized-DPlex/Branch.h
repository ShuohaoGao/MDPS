#ifndef BRANCH_H
#define BRANCH_H

#include "Graph.h"

/**
 * @brief bnb based on maintaining infomation dynamically, i.e., it needs to record the changes and rollback before return
 */
class Branch
{
public:
    vector<int> S;
    vector<bool> in_S;             // u is in S <=> in_S[u]=1
    vector<int> C, pos_in_C;       // C[i]=u <=> pos_in_C[u] = i
    vector<int> dout, din;         // degree in G[S+C]
    vector<int> out_neighbor_in_S; // |Nout(u)\cap S|
    vector<int> in_neighbor_in_S;  // |Nin(u)\cap S|
    Graph *g;
    int k, l;
    set<ui> *ground_truth;
    int lb;

    ll dfs_cnt;
    double bnb_time;

    vector<int> int_array1, int_array2; // size=n, default value = 0

    Branch(Graph *_g, int paramK, int paramL, set<ui> *truth) : g(_g), k(paramK), l(paramL), ground_truth(truth),
                                                                dfs_cnt(0), bnb_time(0)
    {
        lb = ground_truth->size();
        lb = max(lb, k + l - 2);
        ui n = g->n;
        in_S.resize(n);
        for (ui v : g->query_vertices)
        {
            S.push_back(v);
            in_S[v] = 1;
        }
        for (ui i = 0; i < n; i++)
        {
            if (in_S[i])
                continue;
            C.push_back(i);
        }
        pos_in_C.resize(n, -1);
        for (ui i = 0; i < C.size(); i++)
        {
            int u = C[i];
            pos_in_C[u] = i;
        }
        dout.resize(n);
        din.resize(n);
        for (ui i = 0; i < n; i++)
        {
            dout[i] = g->vertices[i].dout;
            din[i] = g->vertices[i].din;
        }
        out_neighbor_in_S.resize(n);
        in_neighbor_in_S.resize(n);
        for (int u : S)
        {
            for (ui v : g->vertices[u].out_neighbors)
                in_neighbor_in_S[v]++;
            for (ui v : g->vertices[u].in_neighbors)
                out_neighbor_in_S[v]++;
        }
        int_array1.resize(n);
        int_array2.resize(n);
    }
    ~Branch() {}

    /**
     * @brief entrance of branch-and-bound procedure
     */
    void run()
    {
        Timer t;
        bnb();
        bnb_time = t.get_time();

        print_results();
    }
    /**
     * @brief print log
     */
    void print_results()
    {
        printf("dfs-cnt= %lld use-time: %.4lf s\n", dfs_cnt, bnb_time / 1e6);
    }
    /**
     * @brief recursive branch-and-bound procedure: aim to find largest DPlex containing S in G[S+C]
     */
    void bnb()
    {
        dfs_cnt++;
        if (S.size() > lb)
            update_lb_S();

        vector<int> vertex_removed; // store the vertices we removed in current branch
        reduce_C(vertex_removed);

        int ub = upper_bound();
        if (ub <= lb)
        {
            rollback(vertex_removed);
            return;
        }

        int pivot = select_pivot(); // pivot in C
        assert(pivot != -1);
        int pd = min(dout[pivot] + k, din[pivot] + l);
        int V_sz = S.size() + C.size();
        if (pd >= V_sz) // check if S+C is DPlex
        {
            bool ok = 1;
            for (int v : S)
            {
                int pd_v = min(dout[v] + k, din[v] + l);
                if (pd_v < V_sz)
                {
                    ok = 0;
                    break;
                }
            }
            if (ok)
            {
                update_lb_S_C();
                rollback(vertex_removed);
                return;
            }
        }

        // generate sub-branches
        // branch 1: add pivot to S
        move_from_C_to_S(pivot);
        if (S_is_DPlex())
            bnb();
        move_from_S_to_C(pivot);

        // branch 2: remove pivot from C
        remove_from_C(pivot);
        bnb();
        add_to_C(pivot);

        // add the removed vertices to C
        rollback(vertex_removed);
    }

    /**
     * @brief S+C is DPlex and |S|+|C|>lb
     */
    void update_lb_S_C()
    {
        assert(S.size() + C.size() > lb);
        lb = S.size() + C.size();
        ground_truth->clear();
        for (int v : S)
            ground_truth->insert(g->vertices[v].origin_id);
        for (int v : C)
            ground_truth->insert(g->vertices[v].origin_id);
        assert(ground_truth->size() == lb);
        printf("Find a larger DPlex (size=%d)\n", lb);
    }

    /**
     * @brief |S|>lb
     */
    void update_lb_S()
    {
        assert(S.size() > lb);
        lb = S.size();
        ground_truth->clear();
        for (int v : S)
            ground_truth->insert(g->vertices[v].origin_id);
        assert(ground_truth->size() == lb);
        printf("Find a larger DPlex (size=%d)\n", lb);
    }

    /**
     * @brief BinaryEstimate(S+s)
     */
    int binary_ub_add_S(vector<int> &s)
    {
        s.insert(s.end(), S.begin(), S.end());
        if (s.size() <= k)
            return s.size();
        // compute degree in subgraph G[s]
        auto &din = int_array1, &dout = int_array2;
        for (int u : s)
        {
            for (int v : s)
            {
                if (v == u)
                    break;
                if (g->exist_edge(u, v))
                {
                    dout[u]++;
                    din[v]++;
                }
                if (g->exist_edge(v, u))
                {
                    dout[v]++;
                    din[u]++;
                }
            }
        }
        auto &pd = int_array1;
        for (int u : s)
            pd[u] = min(dout[u] + k, din[u] + l);
        vector<int> degrees(s.size());
        for (int u : s)
        {
            degrees.push_back(pd[u]);
            dout[u] = din[u] = 0; // clear the array
        }
        degrees.push_back(-1); // serve for binary search
        sort(degrees.begin(), degrees.end());
        // binary search
        int a = k, b = degrees.size() - 1;
        while (a < b)
        {
            int mid = (a + b + 1) >> 1;
            int pos = lower_bound(degrees.begin(), degrees.end(), mid) - degrees.end(); // degrees[pos] >= mid
            // the number of vertices whose pseudo-degrees are not less than mid
            int cnt = degrees.size() - pos;
            if (cnt >= mid)
                a = mid;
            else
                b = mid - 1;
        }
        return a; // there are a vertices whose pd are at least a
    }

    /**
     * @brief partition C to |S|+1 vertex sets: pi_0, pi_1, ..., pi_{|S|}
     */
    int partition_out()
    {
        vector<int> common_out_neighbors; // pi_0
        vector<int> pi_i_size(S.size());
        for (int u : C)
        {
            bool flag = 0;
            for (int i = 0; i < S.size(); i++)
            {
                if (!g->exist_edge(S[i], u))
                {
                    pi_i_size[i]++;
                    flag = 1;
                    break;
                }
            }
            if (!flag)
                common_out_neighbors.push_back(u);
        }
        int ub = 0;
        for (int i = 0; i < S.size(); i++)
            ub += min(pi_i_size[i], k - ((int)S.size() - out_neighbor_in_S[S[i]]));
        if (ub + S.size() + common_out_neighbors.size() <= lb)
            return lb; // pruned
        // invoke binary estimate
        return ub + binary_ub_add_S(common_out_neighbors);
    }

    /**
     * @brief compute UB(S,C)
     */
    int upper_bound()
    {
        if (degree_ub() <= lb)
            return lb;
        if (part_out_ub() <= lb)
            return lb;
        return partition_out();
    }

    /**
     * @brief ub based on partition: we just random select 2 vertices
     */
    int part_out_ub()
    {
        int i = rand() % S.size();
        int j = i + 1;
        if (j == S.size())
            j = 0;
        int u = S[i], v = S[j];
        vector<ui> out_neighbors_u; // out-neighbors of u in C
        for (ui w : g->vertices[u].out_neighbors)
            if (pos_in_C[w] != -1) // w is in C
                out_neighbors_u.push_back(w);
        int non_neighbor_in_C = C.size() - (dout[u] - out_neighbor_in_S[u]);             // size of \Pi_u
        int neighbor_v = intersect_count(out_neighbors_u, g->vertices[v].out_neighbors); // Nout(u) \cap Nout(v)
        int non_neighbor_v = out_neighbors_u.size() - neighbor_v;                        // size of \Pi_v
        int ub = S.size();
        ub += min(non_neighbor_in_C, k - ((int)S.size() - out_neighbor_in_S[u])); // the vertices in \Pi_u: non-out-neighbors of u
        ub += neighbor_v;                                                         // common-out-neighbors
        ub += min(k - ((int)S.size() - out_neighbor_in_S[v]), non_neighbor_v);    // the vertices in \Pi_v: non-out-neighbors of v
        return ub;
    }

    /**
     * @brief ub = min pd[v], for v in S
     */
    int degree_ub()
    {
        int ret = S.size() + C.size();
        for (int v : S)
        {
            int pd = min(dout[v] + k, din[v] + l);
            ret = min(ret, pd);
        }
        return ret;
    }

    /**
     * @brief select a vertex with minimum pseudo-degree in G[S+C]
     * @return pivot in C
     */
    int select_pivot()
    {
        int sel = -1, min_pd = INF;
        for (int u : C)
        {
            int pd = min(dout[u] + k, din[u] + l);
            if (pd < min_pd)
            {
                sel = u;
                min_pd = pd;
            }
        }
        return sel;
    }

    /**
     * @brief for the vertices removed in current branch, we need to rollback before return
     */
    void rollback(vector<int> &vertex_removed)
    {
        for (int u : vertex_removed)
            add_to_C(u);
    }

    /**
     * @brief reduce u from C if 1) S+u is not a DPlex or 2) pd(u)<=lb or 3) diameter > 2
     */
    void reduce_C(vector<int> &vertex_removed)
    {
        for (int i = 0; i < C.size();)
        {
            int u = C[i];
            bool removed = 0;
            if (out_neighbor_in_S[u] + k < S.size() + 1 || in_neighbor_in_S[u] + l < S.size() + 1)
                removed = 1;
            if (dout[u] + k <= lb || din[u] + l <= lb)
                removed = 1;
            if (removed)
            {
                remove_from_C(u);
                vertex_removed.push_back(u);
                continue;
            }
            i++;
        }
    }

    /**
     * @return whether G[S] is a DPlex
     */
    bool S_is_DPlex()
    {
        for (ui v : S)
        {
            int pd = min(out_neighbor_in_S[v] + k, in_neighbor_in_S[v] + l);
            if (pd < S.size())
                return false;
        }
        return true;
    }

    /**
     * @brief move a vertex u from S to C, and update the related array
     */
    void move_from_C_to_S(int u)
    {
        // for (int u : C)
        // {
        //     int pos = pos_in_C[u];
        //     assert(pos != -1);
        //     assert(C[pos] == u);
        // }
        int pos = pos_in_C[u];
        assert(!in_S[u] && pos != -1);
        assert(C[pos] == u);
        pos_in_C[C.back()] = pos;
        swap(C[C.size() - 1], C[pos]);
        C.pop_back();
        pos_in_C[u] = -1;
        in_S[u] = 1;
        S.push_back(u);
        for (ui v : g->vertices[u].out_neighbors)
            in_neighbor_in_S[v]++;
        for (ui v : g->vertices[u].in_neighbors)
            out_neighbor_in_S[v]++;
        // for (int u : C)
        // {
        //     int pos = pos_in_C[u];
        //     assert(pos != -1);
        //     assert(C[pos] == u);
        // }
    }

    /**
     * @brief include a vertex u from C to S, and update the related array
     */
    void move_from_S_to_C(int u)
    {
        assert(S.back() == u && in_S[u]);
        S.pop_back();
        in_S[u] = 0;
        assert(pos_in_C[u] == -1);
        pos_in_C[u] = C.size();
        C.push_back(u);
        for (ui v : g->vertices[u].out_neighbors)
            in_neighbor_in_S[v]--;
        for (ui v : g->vertices[u].in_neighbors)
            out_neighbor_in_S[v]--;
    }

    /**
     * @brief remove a vertex u from C, and update the related degree
     */
    void remove_from_C(int u)
    {
        int pos = pos_in_C[u];
        assert(pos != -1);
        assert(C[pos] == u);
        pos_in_C[C.back()] = pos;
        swap(C[C.size() - 1], C[pos]);
        C.pop_back();
        for (ui v : g->vertices[u].out_neighbors)
            din[v]--;
        for (ui v : g->vertices[u].in_neighbors)
            dout[v]--;
        pos_in_C[u] = -1;
    }

    /**
     * @brief add a vertex u to C, and update the related degree
     */
    void add_to_C(int u)
    {
        assert(!in_S[u] && pos_in_C[u] == -1);
        pos_in_C[u] = C.size();
        C.push_back(u);
        dout[u] = din[u] = 0;
        out_neighbor_in_S[u] = in_neighbor_in_S[u] = 0;
        for (ui v : g->vertices[u].out_neighbors)
        {
            if (in_S[v] || pos_in_C[v] != -1)
            {
                dout[u]++;
                din[v]++;
                if (in_S[v])
                    out_neighbor_in_S[u]++;
            }
        }
        for (ui v : g->vertices[u].in_neighbors)
        {
            if (in_S[v] || pos_in_C[v] != -1)
            {
                dout[v]++;
                din[u]++;
                if (in_S[v])
                    in_neighbor_in_S[u]++;
            }
        }
    }
};

#endif