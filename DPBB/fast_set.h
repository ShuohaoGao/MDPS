#ifndef FAST_SET_H_INCLUDED
#define FAST_SET_H_INCLUDED

#include <vector>
#include <cstring>

#define U_TYPE 0x7e

using ui=unsigned long long;

inline void remove_from_U(int u);
inline void add_to_U(int u);

class ArraySet
{
public:
	int n;
	int sz;
	ui *st;
	int type;
	ArraySet(int _n)
	{
		n=(_n>>5)+1;
		st=new ui[n];
		memset(st,0,sizeof(ui)*n);
		sz=0;
	}
	ArraySet(int _n,int _type)
	{
		n=(_n>>5)+1;
		st=new ui[n];
		memset(st,0,sizeof(ui)*n);
		sz=0;
		type=_type;
	}
	ArraySet(ArraySet* s)
	{
		n=s->n;
		sz=s->sz;
		st=new ui[n];
		memcpy(st,s->st,sizeof(ui)*n);
	}
	~ArraySet()
	{
		delete[] st;
	}
	inline void copy(ArraySet *s)
	{
		if(n!=s->n)
		{
			delete[] st;
			n=s->n;
			st=new ui[n];
		}
		sz=s->sz;
		memcpy(st,s->st,sizeof(ui)*n);
	}
	inline void insert(ui x)
	{
		int id=x>>6,sh=x&0x3F;
		ui temp=(st[id] >> sh)&1;
		if(!temp)
		{
			sz++;
			st[id] |= 1ll << sh;
			// if(type == U_TYPE)
			// 	add_to_U(x);
		} 
	}
	inline void erase(ui x)
	{
		int id=x>>6,sh=x&0x3F;
		ui temp=(st[id] >> sh)&1;
		if(temp)
		{
			sz--;
			st[id] ^= 1ll << sh;
			// if(type == U_TYPE)
			// 	remove_from_U(x);
		}
	}
	std::vector<int> all()
	{
		std::vector<int> ret(sz);
		int cnt=0;
		for(int i=0;i<n;i++)
			if(st[i])
			{
				ui temp=st[i];
				for(int j=0;j<64;j++)
				{
					if(temp &1)
						ret[cnt++]=i*64+j;
					temp >>=1;
				}
			}
		return ret;
	}
	inline int count(int x)
	{
		int id=x>>6,sh=x&0x3F;
		return (st[id] >> sh)&1;
	}
	inline void clear()
	{
		sz=0;
		memset(st,0,sizeof(ui)*n);
	}
	inline int size()
	{
		return sz;
	}
};


struct SegTree
{
private:
    int n;
	struct node 
	{
		int l,r;
		int cnt;
	};
	int *leaf;
    node *tr;
	inline void pushup(int u)
	{
		tr[u].cnt= tr[u<<1].cnt + tr[u<<1|1].cnt;
	}
	void build(int u,int l,int r)
	{
		tr[u]={l,r,0};
		if(l==r)	
		{
			leaf[l]=u;
			return;
		}
		int mid=(l+r)>>1;
		build(u<<1,l,mid);
		build(u<<1|1,mid+1,r);
	}
    void get_all(int u,std::vector<int> &s)
    {
        if(!tr[u].cnt)  return;
        if(tr[u].l==tr[u].r)
        {
            s.push_back(tr[u].l);
            return;
        }
        get_all(u<<1,s);
        get_all(u<<1|1,s);
    }
	std::vector<int> ret;
	bool changed;
public:
    SegTree(int _n){
		changed=true;
        n=_n;
        leaf=new int[n];
        tr=new node[n*4];
        build(1,0,n-1);
    }
    SegTree(SegTree *s){
		changed=true;
        n=s->get_n();
        leaf=new int[n];
        tr=new node[n*4];
        memcpy(leaf,s->get_leaf(),sizeof(int)*n);
        memcpy(tr,s->get_tr(),sizeof(node)*n*4);
    }
    ~SegTree(){
        delete[] leaf;
        delete[] tr;
    }
	inline void copy(SegTree *s)
	{
		changed=true;
		if(n!=s->get_n())
		{
			delete[] leaf;
			delete[] tr;
			n=s->get_n();
			leaf=new int[n];
			tr=new node[n*4];
		}
		memcpy(leaf,s->get_leaf(),sizeof(int)*n);
        memcpy(tr,s->get_tr(),sizeof(node)*n*4);
	}
    inline int get_n()
    {
        return n;
    }
    inline int* get_leaf()
    {
        return leaf;
    }
    inline node* get_tr()
    {
        return tr;
    }
	int query(int u,int l,int r)
	{
		if(tr[u].l>=l && tr[u].r<=r)
			return tr[u].cnt;
		int mid=(tr[u].l+tr[u].r)>>1;
		int ret=0;
		if(l<=mid) ret=query(u<<1,l,r);
		if(r>mid) ret+=query(u<<1|1,l,r);
		return ret;
	}
	int query(int u,int p)
	{
		if(tr[u].l == p && tr[u].r == p)
		{
			return tr[u].cnt;
		}
		int mid=(tr[u].l+tr[u].r)>>1;
		if(p<=mid) return query(u<<1,p);
		else return query(u<<1|1,p);
	}
	void modify(int u,int p,int c)
	{
		if(tr[u].l == p && tr[u].r == p)
		{
			tr[u].cnt+=c;
			return;
		}
		int mid=(tr[u].l+tr[u].r)>>1;
		if(p<=mid) modify(u<<1,p,c);
		else modify(u<<1|1,p,c);
		pushup(u);
		return;
	}
    inline void insert(int x)
	{
		int p=leaf[x];
		if(tr[p].cnt) return;
		changed=true;
		do
		{
			tr[p].cnt++;
			p>>=1;
		}while(p);
	}
	inline void erase(int x)
	{
		int p=leaf[x];
		if(!tr[p].cnt) return;
		changed=true;
		do
		{
			tr[p].cnt--;
			p>>=1;
		}while(p);
	}
	inline int count(int x)
	{
		return tr[leaf[x]].cnt;
	}
	inline int size()
	{
		return tr[1].cnt;
	}
    std::vector<int> all()
    {
		if(!changed)	return ret;
        ret.clear();
		get_all(1,ret);
		changed=false;
        return ret;
    }
	inline void clear()
	{
		for(int i=0;i<n*4;i++)
			tr[i].cnt=0;
	}
};

class Splay
{
private:
	struct SplayNode
	{
		int s[2];
		int p;
		int v;
		void init ( int _v, int _p )
		{
			v = _v, p = _p;
			s[0]=s[1]=0;
		}
	};
	inline int get_idx()
	{
		return stk[--top];
	}
	inline void rotate ( int x )
	{
		int y = tr[x].p, z = tr[y].p;
		int k = ( tr[y].s[1] == x );
		tr[z].s[tr[z].s[1] == y] = x, tr[x].p = z;
		tr[y].s[k] = tr[x].s[k ^ 1], tr[tr[x].s[k ^ 1]].p = y;
		tr[x].s[k ^ 1] = y, tr[y].p = x;
	}
	inline void splay ( int x, int p )
	{
		while ( tr[x].p != p )
		{
			int y = tr[x].p, z = tr[y].p;
			if ( z != p )
			{
				if ( ( tr[y].s[1] == x ) ^ ( tr[z].s[1] == y ) )
					rotate ( x );
				else
					rotate ( y );
			}
			rotate ( x );
		}
		if ( !p )
			root = x;
	}
	void pre_dfs(int u,std::vector<int> &s)
	{
		if(tr[u].s[0])
			pre_dfs(tr[u].s[0],s);
		if(tr[u].v != -0x3f3f3f3f)	
			s.push_back(tr[u].v);
		if(tr[u].s[1])
			pre_dfs(tr[u].s[1],s);
	}
	void clear(int u)
	{
		stk[top++]=u;
		if(tr[u].s[0])
			clear(tr[u].s[0]);
		if(tr[u].s[1])
			clear(tr[u].s[1]);
	}
public:
	int root;
	SplayNode *tr;
	int *stk;
	int top;
	int n;
	Splay(int _n)
	{
		n=_n+5;
		stk=new int[n];
		top=0;
		root=0;
		for(int i=0;i<n-1;i++)
			stk[top++]=n-1-i;//1~n-1
		tr=new SplayNode[n];
		tr[0]={{0,0},0,0};
		insert(-0x3f3f3f3f);
		insert(0x3f3f3f3f);
	}
	Splay(Splay* s)
	{
		root=s->root;
		n=s->n;
		tr=new SplayNode[n];
		stk=new int[n];
		top=s->top;
		memcpy(stk,s->stk,sizeof(int)*top);
		memcpy(tr,s->tr,sizeof(SplayNode)*n);
	}
	~Splay()
	{
		delete[] stk;
		delete[] tr;
	}
	inline void copy(Splay* s)
	{
		root=s->root;
		if(n!=s->n)
		{
			delete[] stk;
			delete[] tr;
			n=s->n;
			tr=new SplayNode[n];
			stk=new int[n];
		}
		top=s->top;
		memcpy(stk,s->stk,sizeof(int)*top);
		memcpy(tr,s->tr,sizeof(SplayNode)*n);
	}

	inline void insert ( int v )
	{
		int u = root, p = 0;
		while ( u )
		{
			if ( tr[u].v == v )
			{
				splay ( u, 0 );
				return;
			}
			p = u, u = tr[u].s[v > tr[u].v];
		}
		u = get_idx();
		if ( p )
			tr[p].s[v > tr[p].v] = u;
		tr[u].init ( v, p );
		splay ( u, 0 );
	}

	inline void erase ( int x )
	{
		int u = root;
		while ( u )
		{
			if ( tr[u].v == x )
				break;
			u = tr[u].s[x > tr[u].v];
		}
		if ( u )
		{
			splay ( u, 0 );
			int l = tr[u].s[0];
			int r = tr[u].s[1];
			while ( tr[l].s[1] )
				l = tr[l].s[1];
			while ( tr[r].s[0] )
				r = tr[r].s[0];
			splay ( l, 0 );
			splay ( r, l );
			stk[top++]=u;
			tr[r].s[0] = 0;
		}
	}

	inline int count(int x)
	{
		int u = root;
		while ( u )
		{
			if ( tr[u].v == x )
				return 1;
			u = tr[u].s[x > tr[u].v];
		}
		return 0;
	}

	inline std::vector<int> all()
	{
		std::vector<int> ret;
		pre_dfs(root,ret);
		ret.pop_back();
		return ret;
	}

	inline void clear()
	{
		clear(root);
		root;
	}

	inline int size()
	{
		return n-1-top-2;
	}
};

#endif // FAST_SET_H_INCLUDED