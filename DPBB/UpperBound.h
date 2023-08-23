#ifndef UPPER_BOUND_H
#define UPPER_BOUND_H

#include "Graph.h"

class UpperBound
{
private:
    Graph *g;
    Plex *S;
    int N;
    int pi0_sz_in, pi0_sz_out;
    int ub_pi0_in, ub_pi0_out;
    int rm_from_pi0_in, rm_from_pi0_out;
    vector<int> h_in, h_out;
    vector<int> din, dout, cnt;
    vector<int> pos_in, pos_out;
    vector<int> set_pi0_in, set_pi0_out; // store Pi[0], common out neighbors of S
public:
    double binary_time;
    UpperBound(Graph *_g, Plex *_S)
    {
        g = _g;
        S = _S;
        int n = g->get_n();
        binary_time = 0;
        N = g->N;
        vector<vector<int> *> temp{&h_in, &h_out, &din,
                                   &dout, &pos_in, &pos_out,
                                   &set_pi0_in, &set_pi0_out};
        for (auto s : temp)
            s->resize(N);
        cnt.resize(N + min(paramK, paramL));
    }
    ~UpperBound()
    {
    }
    /**
     * pi[] store S and Pi_0
     * @return at least |S|
     */
    int binaryEstimateUB(vector<int> &pi, int n)
    {
#ifdef NO_ERFEN
        return n;
#endif
        ll start_binary_time = get_system_time_microsecond();
        fill(din.begin(), din.begin() + n, 0);
        fill(dout.begin(), dout.begin() + n, 0);
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
            {
                int u = pi[i], v = pi[j];
                if (g->exist_edge(u, v))
                    dout[i]++, din[j]++;
                if (g->exist_edge(v, u))
                    dout[j]++, din[i]++;
            }
        for (int i = 0; i < n; i++)
        {
            cnt[min(dout[i] + paramK, din[i] + paramL)]++;
        }
        int ub_idx = n + min(paramK, paramL);
        cnt[ub_idx] = 0;
        for (int i = ub_idx - 1; i; i--)
            cnt[i] += cnt[i + 1];
        int l = min(paramK, paramL), r = n;
        while (l < r)
        {
            int mid = l + r + 1 >> 1;
            if (cnt[mid] >= mid)
                l = mid;
            else
                r = mid - 1;
        }
        fill(cnt.begin(), cnt.begin() + n + min(paramK, paramL), 0);
        binary_time += get_system_time_microsecond() - start_binary_time;
        return r;
    }
    int partIn()
    {
        pi0_sz_in = 0;
        // h_in[u] : the count of vertices {v} which v->u is not in G
        for (int v : S->vertex)
            set_pi0_in[pi0_sz_in++] = v, h_in[v] = 0;
        int s_n=S->get_n();
        vector<vector<int>> Pi(s_n);
        for (int v : g->vertex) // traversal candidate set C
        {
            if (S->has(v))
                continue;
            bool ok = 0;
            for(int i=0;i<s_n;i++)
            {
                int u=S->vertex[i];
                if (!g->exist_edge(v, u))
                {
                    h_in[u]++;
                    pos_in[v] = u;
                    ok = 1;
                    Pi[i].push_back(v);
                    break;
                }
            }
            if (!ok) // v is inserted into Pi_0
            {
                pos_in[v] = pi0_sz_in + N;
                set_pi0_in[pi0_sz_in++] = v;
            }
        }
    #ifndef NO_REFINE_ORDER
        // if Pi_i is small, then we tear down it and insert the vertices into other Pi_j
        for(int i=0;i<s_n;i++)
        {
            int u=S->vertex[i];
            if (h_in[u] <= paramL - (s_n - S->get_in_degree(u)))
            {
                h_in[u]=0;
                auto &temp = Pi[i];
                for (int v : temp)
                {
                    bool ok = 0;
                    for(int j=i+1;j<s_n;j++)
                    {
                        int w=S->vertex[j];
                        if (h_in[w] >= paramL - (s_n - S->get_in_degree(w)) && !g->exist_edge(v, w))
                        {
                            h_in[w]++;
                            pos_in[v] = w;
                            ok = 1;
                            Pi[j].push_back(v);
                            break;
                        }
                    }
                    if (!ok) // v is inserted into Pi_0
                    {
                        pos_in[v] = pi0_sz_in + N;
                        set_pi0_in[pi0_sz_in++] = v;
                    }
                }
            }
        }
    #endif
        int ret = 0;
        for (int u : S->vertex)
            ret += min(h_in[u], paramL - (S->get_n() - S->get_in_degree(u)));
        if (ret + pi0_sz_in <= lb)
            return lb;
        ub_pi0_in = binaryEstimateUB(set_pi0_in, pi0_sz_in);
        rm_from_pi0_in = 0;
        return ret + ub_pi0_in;
    }
    int partOut()
    {
        pi0_sz_out = 0;
        for (int v : S->vertex)
            set_pi0_out[pi0_sz_out++] = v, h_out[v] = 0;
        int s_n=S->get_n();
        vector<vector<int>> Pi(s_n);
        for (int v : g->vertex)
        {
            if (S->has(v))
                continue;
            bool ok = 0;
            for(int i=0;i<s_n;i++)
            {
                int u=S->vertex[i];
                if (!g->exist_edge(u, v))
                {
                    h_out[u]++;
                    pos_out[v] = u;
                    ok = 1;
                    Pi[i].push_back(v);
                    break;
                }
            }
            if (!ok)
            {
                pos_out[v] = N + pi0_sz_out;
                set_pi0_out[pi0_sz_out++] = v;
            }
        }
    #ifndef NO_REFINE_ORDER
        // if Pi_i is small, then we tear down it and insert the vertices into other Pi_j
        for(int i=0;i<s_n;i++)
        {
            int u=S->vertex[i];
            if (h_out[u] <= paramK - (s_n - S->get_out_degree(u)))
            {
                h_out[u]=0;
                auto &temp = Pi[i];
                for (int v : temp)
                {
                    bool ok = 0;
                    for(int j=i+1;j<s_n;j++)
                    {
                        int w=S->vertex[j];
                        if (h_out[w] >= paramK - (s_n - S->get_out_degree(w)) && !g->exist_edge(w, v))
                        {
                            h_out[w]++;
                            pos_out[v] = w;
                            ok = 1;
                            Pi[j].push_back(v);
                            break;
                        }
                    }
                    if (!ok) // v is inserted into Pi_0
                    {
                        pos_out[v] = pi0_sz_out + N;
                        set_pi0_out[pi0_sz_out++] = v;
                    }
                }
            }
        }
    #endif
        int ret = 0;
        for (int u : S->vertex)
            ret += min(h_out[u], paramK - (S->get_n() - S->get_out_degree(u)));
        if (ret + pi0_sz_out <= lb)
            return lb;
        ub_pi0_out = binaryEstimateUB(set_pi0_out, pi0_sz_out);
        rm_from_pi0_out = 0;
        return ret + ub_pi0_out;
    }
    int degreeUB()
    {
        int ub = g->get_n();
        for (ui v : S->vertex)
            ub = min(ub, g->get_pd(v));
        return ub;
    }
    int partUB()
    {
        int a = partOut();
        if (a <= lb)
            return a;
        int b = partIn();
        return b;
    }
    int get_upper_bound()
    {
        return partUB();
        int a = degreeUB();
        int b = partUB();
        return min(a, b);
    }
    /**
     * compared with parent branch, current branch:
     * removed u;
     * v_removed are reduced from candidate set C.
     * So we can use the old information to get ub
     */
    int get_old_upper_bound(int u, vector<int> &v_removed)
    {
        if (get_old_in_UB(u, v_removed) <= lb)
            return lb;
        return get_old_out_UB(u, v_removed);
    }
    int get_old_in_UB(int u, vector<int> &v_removed)
    {
        if (pos_in[u] >= N) // u in Pi0
        {
            assert(pi0_sz_in > 0);
            int idx = pos_in[u] - N;
            swap(set_pi0_in[idx], set_pi0_in[--pi0_sz_in]);
            rm_from_pi0_in++;
        }
        else
        {
            h_in[pos_in[u]]--;
        }
        for (int u : v_removed)
        {
            if (pos_in[u] >= N) // u in Pi0
            {
                assert(pi0_sz_in > 0);
                int idx = pos_in[u] - N;
                swap(set_pi0_in[idx], set_pi0_in[--pi0_sz_in]);
                rm_from_pi0_in++;
            }
            else
            {
                h_in[pos_in[u]]--;
            }
        }
        int ub_in = 0;
        for (int v : S->vertex)
        {
            ub_in += min(h_in[v], paramL - (S->get_n() - S->get_in_degree(v)));
            assert(h_in[v] >= 0);
        }
        if (ub_in + ub_pi0_in <= lb)
            return lb;
        if (ub_in + ub_pi0_in - rm_from_pi0_in > lb)
            return ub_in + ub_pi0_in;
        rm_from_pi0_in = 0;
        ub_pi0_in = binaryEstimateUB(set_pi0_in, pi0_sz_in);
        ub_in += ub_pi0_in;
        return ub_in;
    }
    int get_old_out_UB(int u, vector<int> &v_removed)
    {
        if (pos_out[u] >= N) // u in Pi0
        {
            assert(pi0_sz_out > 0);
            int idx = pos_out[u] - N;
            swap(set_pi0_out[idx], set_pi0_out[--pi0_sz_out]);
            rm_from_pi0_out++;
        }
        else
        {
            h_out[pos_out[u]]--;
        }
        for (int u : v_removed)
        {
            if (pos_out[u] >= N) // u in Pi0
            {
                assert(pi0_sz_out > 0);
                int idx = pos_out[u] - N;
                swap(set_pi0_out[idx], set_pi0_out[--pi0_sz_out]);
                rm_from_pi0_out++;
            }
            else
            {
                h_out[pos_out[u]]--;
            }
        }
        int ub_out = 0;
        for (int v : S->vertex)
        {
            assert(h_out[v] >= 0);
            ub_out += min(h_out[v], paramK - (S->get_n() - S->get_out_degree(v)));
        }
        if (ub_out + ub_pi0_out <= lb)
            return lb;
        if (ub_out + ub_pi0_out - rm_from_pi0_out > lb)
            return ub_out + ub_pi0_out;
        rm_from_pi0_out = 0;
        ub_pi0_out = binaryEstimateUB(set_pi0_out, pi0_sz_out);
        ub_out += ub_pi0_out;
        return ub_out;
    }
};

#endif