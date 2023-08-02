#ifndef MY_HASHMAP_H
#define MY_HASHMAP_H

#include"Utility.h"

// used for second order reduction to save edges
struct MyHashMap
{
    struct edge
    {
        ui a, b;
        // ui id;
        ui pre, ne;
    };
    edge *edges;
    ui m;
    ll n;
    ui mask;// used for hash function
    ui idx;
    ui *h;
    ui size;// m won't decrease but size will
    bool *rm;//remove
    void init(ui _m, ui _n)
    {
        m = _m;
        n = _n;
        edges = new edge[m];
        idx = 0;
        mask = 1;
        while (_m)
        {
            mask = (mask << 1) | 1;
            _m >>= 1;
        }
        h = new ui[mask + 1];
        for (ui i = 0; i <= mask; i++)
            h[i] = m;
        rm = new bool[m];
        memset(rm, 0, sizeof(bool) * m);
        size = m;
    }
    void clear(ui _m)
    {
        m=_m;
        memset(rm, 0, sizeof(bool)*m);
        idx=0;
        size=m;
        mask = 1;
        while (_m)
        {
            mask = (mask << 1) | 1;
            _m >>= 1;
        }
        for (ui i = 0; i <= mask; i++)
            h[i] = m;
    }
    void insert(ui key, ui edge_id)
    {
        if (h[key] != m)
        {
            edges[edge_id].ne = h[key];
            edges[h[key]].pre = edge_id;
        }
        h[key] = edge_id;
    }
    void insert_edge(ui a, ui b) // assert each edge is inserted once
    {
        ll val = a * n + b;
        edges[idx] = {a, b, m, m};
        insert(val & mask, idx++);
    }
    ui get(ui a, ui b) // return index of edge
    {
        ui key = (a * n + b) & mask;
        ui p = h[key];
        while (p != m)
        {
            if (edges[p].a == a && edges[p].b == b)
                return p;
            p = edges[p].ne;
        }
        return m;
    }
    bool has(ui a, ui b)
    {
        return get(a, b) != m;
    }
    ui remove_edge(ui a, ui b)
    {
        ui id = get(a, b);
        if (id == m)
            return m;
        remove_edge(id);
        return id;
    }
    bool remove_edge(ui id)
    {
        if(rm[id])  return false;
        rm[id] = 1;
        size--;
        if (edges[id].pre == m)
        {
            ui a=edges[id].a, b=edges[id].b;
            ui key = (a * n + b) & mask;
            h[key] = edges[id].ne;
            if (h[key] != m)
                edges[h[key]].pre = m;
        }
        else
        {
            ui pre = edges[id].pre, ne = edges[id].ne;
            edges[pre].ne = ne;
            if (ne != m)
                edges[ne].pre = pre;
        }
        return true;
    }
};

class Map//map<pii,int> is too slow;regard {u,v}={v,u}
{
protected:
	ll maxv;//make sure that pii.first<maxv 
public:
    Map(){}
	Map(ll v)
	{
		maxv=v;
	}
	~Map(){}
    virtual ll get_ll(const pii &key)=0;
    virtual int get(const pii &key)=0;
    virtual void put(const pii &key, const int value)=0;
    virtual int increment(const pii &key)=0;//i++
    virtual int reduce(const pii &key)=0;//i--
    virtual int count(const pii &key)=0;
	virtual void clear() =0;
};

class ArrayMap:public Map
{
    int *h;
public:
    ArrayMap(ll v)
    {
        maxv=v;
        int sz=(v+1)*(v+1);
        h=new int[sz];
        memset(h,0,sizeof(int)*sz);
    }
    ~ArrayMap()
    {
        delete[] h;
    }
    ll get_ll(const pii &key)
	{
		if(key.first > key.second)
			return key.second * maxv + key.first;
		return key.first * maxv +key.second;
	}
    int get(const pii &key)
	{
		return h[get_ll(key)];
	}
    void put(const pii &key, const int value)
	{
		h[get_ll(key)]=value;
	}
    int increment(const pii &key)//i++
	{
		return ++h[get_ll(key)];
	}
    int reduce(const pii &key)//i--
	{
		return --h[get_ll(key)];
	}
    int count(const pii &key)
	{
		return h[get_ll(key)];
	}
	void clear()
	{
		int sz=(maxv+1)*(maxv+1);
        memset(h,0,sizeof(int)*sz);
	}
};

class HashMapSTL:public Map 
{
    unordered_map<ll,int> h;
public:
    HashMapSTL(ll v)
	{
		maxv=v;
	}
	~HashMapSTL()
	{
		h.clear();
	}
    ll get_ll(const pii &key)
	{
		if(key.first > key.second)
			return key.second * maxv + key.first;
		return key.first * maxv +key.second;
	}
    int get(const pii &key)
	{
		return h[get_ll(key)];
	}
    void put(const pii &key, const int value)
	{
		h[get_ll(key)]=value;
	}
    int increment(const pii &key)//i++
	{
		return ++h[get_ll(key)];
	}
    int reduce(const pii &key)//i--
	{
		return --h[get_ll(key)];
	}
    int count(const pii &key)
	{
		return h.count(get_ll(key));
	}
	void clear()
	{
		h.clear();
	}
};

class HashMap_K:public Map 
{
    struct HMap
    {
        ll m;
        struct node
        {
            ll key;
            int val;
            bool valid;
        };
        node *a;
        void init(ll _m)
        {
            m=_m*3;
			m=m/10*10+7;
            a=new node[m];
            for(ll i=0;i<m;i++)
                a[i].valid=false;
        }
        void put(ll key, int val)
        {
            ll k=key%m;
            while(a[k].valid)
            {
                k++;
                if(k>=m)
                    k=0;
            }
            a[k]={key,val,true};
        }
        int get(ll key)
        {
            ll k=key%m;
            while(a[k].valid)
            {
                if(a[k].key==key)
                    return a[k].val;
                k++;
                if(k>=m)
                    k=0;
            }
            return 0;
        }
        bool count(ll key)
        {
            ll k=key%m;
            while(a[k].valid)
            {
                if(a[k].key==key)
                    return 1;
                k++;
                if(k>=m)
                    k=0;
            }
            return 0;
        }
        int increase(ll key)
        {
            ll k=key%m;
            while(a[k].valid)
            {
                if(a[k].key==key)
                    return ++a[k].val;
                k++;
                if(k>=m)
                    k=0;
            }
			a[k]={key, 1, true};
            return 1;
        }
        int decrease(ll key)
        {
            ll k=key%m;
            while(a[k].valid)
            {
                if(a[k].key==key)
                    return --a[k].val;
                k++;
                if(k>=m)
                    k=0;
            }
			a[k]={key, -1, true};
            return -1;
        }
        void clear()
        {
            for(ll i=0;i<m;i++)
                a[i].valid=0;
        }
    }h;
    // struct HMap_List 
    // {
    //     ll m;
    //     ui idx;
    //     struct node
    //     {
    //         ll key;
    //         int ne;
    //         int val;
    //     };
    //     node *a;
    //     int *h;
    //     void init(ll _m)
    //     {
    //         m=_m*2;
	// 		m=m/1000*1000+7;
    //         h=new int[m];
    //         memset(h,-1,sizeof(int)*m);
    //         a=new node[_m];
    //         idx=0;
    //     }
    //     void put(ll key, int val)
    //     {
    //         ll k=key%m;
    //         a[idx]={key,h[k],val};
    //         h[k]=idx++;
    //     }
    //     int get(ll key)
    //     {
    //         ll k=key%m;
    //         for(int i=h[k];~i;i=a[i].ne)
    //         {
    //             if(a[i].key==key)
    //                 return a[i].val;
    //         }
    //         return 0;
    //     }
    //     bool count(ll key)
    //     {
    //         ll k=key%m;
    //         for(int i=h[k];~i;i=a[i].ne)
    //         {
    //             if(a[i].key==key)
    //                 return 1;
    //         }
    //         return 0;
    //     }
    //     int increase(ll key)
    //     {
    //         ll k=key%m;
    //         for(int i=h[k];~i;i=a[i].ne)
    //         {
    //             if(a[i].key==key)
    //             {
    //                 return ++a[i].val;
    //             }
    //         }
    //         a[idx]={key,h[k],1};
    //         h[k]=idx++;
    //         return 1;
    //     }
    //     int decrease(ll key)
    //     {
    //         ll k=key%m;
    //         for(int i=h[k];~i;i=a[i].ne)
    //         {
    //             if(a[i].key==key)
    //             {
    //                 return --a[i].val;
    //             }
    //         }
    //         a[idx]={key,h[k],-1};
    //         h[k]=idx++;
    //         return -1;
    //     }
    //     void clear()
    //     {
    //         memset(h,-1,sizeof(int)*m);
    //     }
    // }h;
public:
    HashMap_K(ll v)
	{
		maxv=v;
        h.init(1e7);
	}
	HashMap_K(ll v, ll m)
	{
		maxv=v;
		h.init(m);
	}
	~HashMap_K()
	{
        delete h.a;
	}
    ll get_ll(const pii &key)
	{
		if(key.first > key.second)
			return key.second * maxv + key.first;
		return key.first * maxv +key.second;
	}
    int get(const pii &key)
	{
		return h.get(get_ll(key));
	}
    void put(const pii &key, const int value)
	{
		h.put(get_ll(key), value);
	}
    int increment(const pii &key)//i++
	{
		return h.increase(get_ll(key));
	}
    int reduce(const pii &key)//i--
	{
		return h.decrease(get_ll(key));
	}
    int count(const pii &key)
	{
		return h.count(get_ll(key));
	}
	void clear()
	{
		h.clear();
	}
};

//when v is small, we can avoid hash collision by allocing more memory
inline  Map* get_suitable_map(ll v,ll m=1e7)
{
    static const int MAXV=20000;
    if(v<MAXV)  return new ArrayMap(v);
    return new HashMap_K(v,m);
    return new HashMapSTL(v);
}

//avoid push into queue repeatedly
class Queue
{
    bool *in_queue;
    int *q;
    int hh,tt;
public:
    Queue(int n)// n is the max size of queue
    {
        in_queue=new bool[n];
        memset(in_queue,0,sizeof(bool)*n);
        q=new int[n];
        hh=0; tt=-1;
    }
    ~Queue()
    {
        delete[] in_queue;
        delete[] q;
    }
    inline void pop()
    {
        hh++;
    }
    inline int size()
    {
        return tt-hh+1;
    }
    inline void push(int id)
    {
        if(!in_queue[id])
        {
            q[++tt]=id;
            in_queue[id]=1;
        }
    }
    inline int front()
    {
        return q[hh];
    }
    void clear(int n)
    {
        memset(in_queue,0,sizeof(bool)*n);
        hh=0,tt=-1;
    }
};

#endif