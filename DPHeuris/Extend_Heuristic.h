#ifndef EXTEND_HEURISTIC_H_INCLUDED
#define EXTEND_HEURISTIC_H_INCLUDED

#include "Utility.h"

struct point
{
    int id;
    int pd;  // pseudo-degree
    int cnt; // 邻居个数
    bool operator<(const point &P) const
    {
        return cnt > P.cnt || (cnt == P.cnt && pd > P.pd);
    }
};
point *p;

class Heuristic_add
{
private:
    ui n;
    ui m;
    ll *h, *ht; // 邻接表,正向边与反向边
    ll *ne;
    ui *e;
    ui idx;
    int *din, *dout;
    // bool *removed; // 标记点是否被删除
    int *cnt; // 扩张时用到
    inline void add_edge(int a, int b)
    {
        e[idx] = b, ne[idx] = h[a], h[a] = idx++;
        e[idx] = a, ne[idx] = ht[b], ht[b] = idx++;
        din[b]++;
        dout[a]++;
    }

    int add_first()
    {
        // 初始化邻接表
        // memset(removed, 0, n * sizeof(bool));
        memset(din, 0, n * sizeof(int));
        memset(dout, 0, n * sizeof(int));

        for (int u = 0; u < n; u++)
        {
            for (int i = h[u]; ~i; i = ne[i])
            {
                int j = e[i];
                din[j]++;
                dout[u]++;
            }
        }

        vector<pii> p(n);
        for (int i = 0; i < n; i++)
            p[i] = {min(din[i] + paramL, dout[i] + paramK), i};
        sort(p.begin(), p.end());
        set<int> temp;
        int ret = min(paramL, paramK);
        int max_cnt = n;
        for (int i = 0; i < max_cnt; i++)
        {
            temp.clear();
            temp.insert(p[i].y);
            int tt = extend(temp);
            if (tt > ret)
            {
                ret = tt;
                if (ret > lb)
                {
                    cout << "枚举扩张后" << ret << " 枚举点数 " << i + 1 << endl;
                    return ret;
                }
            }
        }
        // cout << "枚举扩张后" << ret << endl;
        return ret;
    }

    struct Seg
    {
        struct node
        {
            int l, r;
            point p;
        };
        node *tr;
        int *leaf;
        inline void init(int n)
        {
            tr = new node[n * 4];
            leaf = new int[n];
            build(1, 0, n - 1);
        }
        inline void pushup(int u)
        {
            auto &fa = tr[u], &l = tr[u << 1], &r = tr[u << 1 | 1];
            if (l.p < r.p)
                fa.p = l.p;
            else
                fa.p = r.p;
        }
        void build(int u, int l, int r)
        {
            tr[u] = {l, r, p[l]};
            if (l == r)
            {
                leaf[l] = u;
                return;
            }
            int mid = (l + r) >> 1;
            build(u << 1, l, mid);
            build(u << 1 | 1, mid + 1, r);
            pushup(u);
        }
        void remove(int u, int x)
        {
            modify(x, -INF);
        }
        void add(int u, int pos)
        {
            modify(pos, 1);
        }
        inline void modify(int p, int c)
        {
            p = leaf[p];
            tr[p].p.cnt += c;
            while (p > 1)
            {
                p >>= 1;
                pushup(p);
            }
        }
    } tr;

    int extend_seg(set<int> &ori)
    {
        set<int> s(ori);
        int *vis = new int[n];
        memset(vis, 0, sizeof(int) * n);
        for (int v : s)
        {
            vis[v] = 1;
        }

        memset(cnt, 0, sizeof(int) * n);
        for (int u : s)
        {
            for (int i = h[u]; ~i; i = ne[i])
            {
                int j = e[i];
                cnt[j]++;
            }
            for (int i = ht[u]; ~i; i = ne[i])
            {
                int j = e[i];
                cnt[j]++;
            }
        }
        for (int i = 0; i < n; i++)
            p[i].cnt = cnt[i];
        for (int u : s)
            p[u].cnt = -INF;
        tr.build(1, 0, n - 1);
        // 现在，din dout存的是plex中的度数
        memset(din, 0, n * sizeof(int));
        memset(dout, 0, n * sizeof(int));
        for (int u : s)
        {
            for (int i = h[u]; ~i; i = ne[i])
            {
                int j = e[i];
                if (!vis[j])
                    continue;
                din[j]++;
                dout[u]++;
            }
        }
        while (true)
        {
            int u = tr.tr[1].p.id; // 最适合加入S的点
            if (tr.tr[1].p.cnt < 0)
                break;
            tr.remove(1, u); // 确保以后不会再拿出u点
            // 尝试插入S，判断是否还是plex
            for (int i = h[u]; ~i; i = ne[i])
            {
                int j = e[i];
                if (vis[j] == 1)
                    din[j]++, dout[u]++;
            }
            for (int i = ht[u]; ~i; i = ne[i])
            {
                int j = e[i];
                if (vis[j] == 1)
                    din[u]++, dout[j]++;
            }
            int lb = s.size() + 1;
            // 检查度数
            bool ok = true;
            for (int v : s)
                if (din[v] < lb - paramL || dout[v] < lb - paramK)
                {
                    ok = false;
                    break;
                }
            if (!ok || din[u] < lb - paramL || dout[u] < lb - paramK) // 不能插入，回滚
            {
                for (int i = h[u]; ~i; i = ne[i])
                {
                    int j = e[i];
                    if (vis[j] == 1)
                        din[j]--;
                }
                dout[u] = 0;
                for (int i = ht[u]; ~i; i = ne[i])
                {
                    int j = e[i];
                    if (vis[j] == 1)
                        dout[j]--;
                }
                din[u] = 0;
            }
            else
            {
                vis[u] = 1;
                s.insert(u);
                for (int i = h[u]; ~i; i = ne[i])
                {
                    int j = e[i];
                    if (!vis[j])
                        tr.add(1, j);
                }
                for (int i = ht[u]; ~i; i = ne[i])
                {
                    int j = e[i];
                    if (!vis[j])
                        tr.add(1, j);
                }
            }
        }

        return s.size();
    }

    int extend_brute(set<int> &ori)
    {
        set<int> s(ori);
        pii *p = new pii[n];
        for (int i = 0; i < n; i++)
            p[i] = {min(din[i] + paramL, dout[i] + paramK), i};
        sort(p, p + n);
        reverse(p, p + n);
        // 初始化度数
        memset(din, 0, n * sizeof(int));
        memset(dout, 0, n * sizeof(int));
        bool *vis = new bool[n]; // 发挥另一种作用
        memset(vis, 0, n * sizeof(bool));
        for (int v : s)
            vis[v] = 1;
        for (int u : s)
        {
            for (int i = h[u]; ~i; i = ne[i])
            {
                int j = e[i];
                if (vis[j])
                    din[j]++, dout[u]++;
            }
        }
        for (int x = 0; x < n; x++) // 枚举点，加入
        {
            int u = p[x].second;
            if (vis[u])
                continue;
            for (int i = h[u]; ~i; i = ne[i])
            {
                int j = e[i];
                if (vis[j])
                    din[j]++, dout[u]++;
            }
            for (int i = ht[u]; ~i; i = ne[i])
            {
                int j = e[i];
                if (vis[j])
                    din[u]++, dout[j]++;
            }
            int lb = s.size() + 1;
            bool ok = true;
            for (int v : s)
                if (din[v] < lb - paramL || dout[v] < lb - paramK)
                {
                    ok = false;
                    break;
                }
            if (!ok || din[u] < lb - paramL || dout[u] < lb - paramK)
            {
                for (int i = h[u]; ~i; i = ne[i])
                {
                    int j = e[i];
                    if (vis[j])
                        din[j]--;
                }
                dout[u] = 0;
                for (int i = ht[u]; ~i; i = ne[i])
                {
                    int j = e[i];
                    if (vis[j])
                        dout[j]--;
                }
                din[u] = 0;
            }
            else
            {
                vis[u] = true;
                s.insert(u);
            }
        }
        delete vis;
        return s.size();
    }

    int extend(set<int> &ori)
    {
        #ifdef BRUTE_EXTEND
            return extend_brute(ori);
        #endif
        int a = extend_seg(ori);
        return a;
    }

public:
    Heuristic_add()
    {
        h = ht = ne = nullptr;
        cnt = din = dout = nullptr;
        e = nullptr;
        // removed = nullptr;
        p = nullptr;
    }
    ~Heuristic_add()
    {
    }
    // 启发式搜索,返回一个KLplex的下界解
    int get_lower_bound(ui _n, ui _m, pii *edges, int paramK, int paramL)
    {
        n = _n;
        m = _m << 1; // 多建立了反向边
        idx = 0;     // 边的编号
        if (h == nullptr)
            h = new ll[n];
        if (ht == nullptr)
            ht = new ll[n];
        if (din == nullptr)
            din = new int[n];
        if (dout == nullptr)
            dout = new int[n];
        if (e == nullptr)
            e = new ui[m];
        if (ne == nullptr)
            ne = new ll[m];
        // if(removed==nullptr)
        //     removed = new bool[n];

        // 初始化邻接表
        memset(h, -1, n * sizeof(ll));
        memset(ht, -1, n * sizeof(ll));
        // memset(removed, 0, n * sizeof(bool));
        memset(din, 0, n * sizeof(int));
        memset(dout, 0, n * sizeof(int));
        idx = 0;
        for (ui i = 0; i < _m; i++)
        {
            add_edge(edges[i].x, edges[i].y);
        }
        // 初始化线段树
        if (p == nullptr)
            p = new point[n];
        for (int i = 0; i < n; i++)
            p[i] = {i, min(din[i] + paramL, dout[i] + paramK), 0};
        tr.init(n);
        if (cnt == nullptr)
            cnt = new int[n];
        int ret = add_first();
        return ret;
    }
};

#endif // HEURISTIC_H_INCLUDED