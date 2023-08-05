#ifndef ARRAY_MANAGER_H_INCLUDED
#define ARRAY_MANAGER_H_INCLUDED

#include <cstring>

class ArrayManager
{
private:
    int n;
    int m;
    int *a_erfen;
    int *h_huafen;
    int *n_q;
    int *m_q;
    bool *n_vis;
    bool *m_vis;
public:
    ArrayManager(int _n,int _m)
    {
        n=_n; m=_m;
        a_erfen=nullptr;
        h_huafen=nullptr;
        n_q=nullptr;
        m_q=nullptr;
        n_vis=nullptr;
        m_vis=nullptr;
    }
    inline int *get_erfen()
    {
        if(a_erfen == nullptr)
        {
            a_erfen=new int[n];
            memset(a_erfen,0,sizeof(int)*n);
        }
        return a_erfen;
    }
    inline int *get_huafen()
    {
        if(h_huafen == nullptr)
        {
            h_huafen=new int[n];
            memset(h_huafen,0,sizeof(int)*n);
        }
        return h_huafen;
    }
    inline int *get_n_q()
    {
        if(n_q == nullptr)
        {
            n_q=new int[n];
            memset(n_q,0,sizeof(int)*n);
        }
        return n_q;
    }
    inline int *get_m_q()
    {
        if(m_q == nullptr)
        {
            m_q=new int[m];
            memset(m_q,0,sizeof(int)*m);
        }
        return m_q;
    }
    inline bool* get_n_vis()
    {
        if(n_vis==nullptr)
        {
            n_vis=new bool[n];
            memset(n_vis,0,sizeof(bool)*n);
        }
        return n_vis;
    }
    inline bool* get_m_vis()
    {
        if(m_vis==nullptr)
        {
            m_vis=new bool[m];
            memset(m_vis,0,sizeof(bool)*m);
        }
        return m_vis;
    }
};

#endif // ARRAY_MANAGER_H_INCLUDED