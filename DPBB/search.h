#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

#include "upperbound.h"
#include "reduce_graph.h"

class Heuristic_search
{
private:
	int n;
	int m;
	int *h, *ht; //,
	int *e, *ne, idx;
	int *din, *dout;
	bool *removed;//
	bool *in_queue;//
	inline void add_edge ( int a, int b )
	{
		e[idx] = b, ne[idx] = h[a], h[a] = idx++;
		e[idx] = a, ne[idx] = ht[b], ht[b] = idx++;
		din[b]++;
		dout[a]++;
	}
	int dout_first()
	{
		//
		memset ( h, -1, n * sizeof ( int ) );
		memset ( ht, -1, n * sizeof ( int ) );
		memset ( removed, 0, n * sizeof ( bool ) );
		memset ( din, 0, n * sizeof ( int ) );
		memset ( dout, 0, n * sizeof ( int ) );
		idx=0;

		for (const DirectedEdge &edge : edges ) //
		{
			add_edge ( edge.from, edge.to );
		}

		//,,,
		priority_queue<pii, vector<pii>, greater<pii> >  q; //
		for ( int i = 0; i < n; i++ )
		{
			q.push ( {dout[i], i} );
		}
		int rest = n; //
		while ( true ) //
		{
			pii top = q.top();
			q.pop();
			int u = top.second;
			if ( removed[u] )
			{
				continue;
			}
			if ( dout[u] >= rest - paramL )
			{
				break;
			}
			removed[u] = 1; //
			rest--;
			//
			for ( int i = ht[u]; ~i; i = ne[i] )
			{
				int j = e[i]; //u->j
				if ( removed[j] )
					continue;
				dout[j]--;
				q.push ( {dout[j], j} );
			}
			for ( int i = h[u]; ~i; i = ne[i] )
			{
				int j = e[i]; //j->u
				din[j]--;
			}
		}

		while ( q.size() ) //
			q.pop();

		for ( int i = 0; i < n; i++ ) //
		{
			if ( removed[i] )
				continue;
			q.push ( {din[i], i} );
		}

		while ( true ) //
		{
			pii top = q.top();
			q.pop();
			int u = top.second;
			if ( removed[u] )
			{
				continue;
			}
			if ( din[u] >= rest - paramK )
			{
				break;
			}
			removed[u] = 1; //
			rest--;
			//,,u
			for ( int i = h[u]; ~i; i = ne[i] )
			{
				int j = e[i]; //j->u
				if ( removed[j] )
					continue;
				din[j]--;
				q.push ( {din[j], j} );
			}
		}
		if(rest > (int)max_KLplex.size())
		{
			for ( int i = 0; i < n; i++ )
			{
				if ( !removed[i] )
				{
					max_KLplex.insert ( i );
				}
			}
		}
		return max(rest, min(paramK,paramL));
	}
	
	int din_first()
	{
		//
		memset ( h, -1, n * sizeof ( int ) );
		memset ( ht, -1, n * sizeof ( int ) );
		memset ( removed, 0, n * sizeof ( bool ) );
		memset ( din, 0, n * sizeof ( int ) );
		memset ( dout, 0, n * sizeof ( int ) );
		idx=0;

		for (const DirectedEdge &edge : edges ) //
		{
			add_edge ( edge.from, edge.to );
		}

		//,,,
		priority_queue<pii, vector<pii>, greater<pii> >  q; //

		for ( int i = 0; i < n; i++ )
		{
			q.push ( {din[i], i} );
		}
		int rest = n; //
		while ( true ) //
		{
			pii top = q.top();
			q.pop();
			int u = top.second;
			if ( removed[u] )
			{
				continue;
			}
			if ( din[u] >= rest - paramL )
			{
				break;
			}
			removed[u] = 1; //
			rest--;
			//
			for ( int i = h[u]; ~i; i = ne[i] )
			{
				int j = e[i]; //u->j
				if ( removed[j] )
					continue;
				din[j]--;
				q.push ( {din[j], j} );
			}
			for ( int i = ht[u]; ~i; i = ne[i] )
			{
				int j = e[i]; //j->u
				dout[j]--;
			}
		}

		while ( q.size() ) //
			q.pop();

		for ( int i = 0; i < n; i++ ) //
		{
			if ( removed[i] )
				continue;
			q.push ( {dout[i], i} );
		}
		while ( true ) //
		{
			pii top = q.top();
			q.pop();
			int u = top.second;
			if ( removed[u] )
			{
				continue;
			}
			if ( dout[u] >= rest - paramK )
			{
				break;
			}
			removed[u] = 1; //
			rest--;
			//,,u
			for ( int i = ht[u]; ~i; i = ne[i] )
			{
				int j = e[i]; //j->u
				if ( removed[j] )
					continue;
				dout[j]--;
				q.push ( {dout[j], j} );
			}
		}
		if(rest > (int)max_KLplex.size())
		{
			for ( int i = 0; i < n; i++ )
			{
				if ( !removed[i] )
				{
					max_KLplex.insert ( i );
				}
			}
		}
		return max(rest, min(paramK,paramL));
	}

public:
	//,KLplex
	int get_lower_bound ( int _n, int _m, const vector<DirectedEdge> &edges, int paramK, int paramL )
	{
		n = _n;
		m = _m << 1; //
		idx = 0; //
		h = new int[n];
		ht = new int[n];
		din = new int[n];
		dout = new int[n];
		e = new int[m];
		ne = new int[m];
		removed = new bool[n];
		
		int in=din_first();
		cout<<""<<in<<endl;
		exit(0);
		int out=dout_first();

		delete[] removed;
		return max(in,out);
	}

	//,;v_remove[]
	void weak_reduce()
	{
		memset ( din, 0, n * sizeof ( int ) );
		memset ( dout, 0, n * sizeof ( int ) );
		in_queue = new bool[n];
		memset ( in_queue, 0, n * sizeof ( bool ) );
		for ( int i = 0; i < idx; i += 2 ) //
		{
			int from = e[i ^ 1];
			int to = e[i];
			din[to]++;
			dout[from]++;
		}

		queue<int> q;
		for ( int i = 0; i < n; i++ )
		{
			if ( din[i] <= lb - paramL || dout[i] <= lb - paramK )
			{
				in_queue[i] = true;
				q.push ( i );
			}
		}
		while ( q.size() )
		{
			int u = q.front();
			q.pop();
			v_remove[u] = true;
			for ( int i = h[u]; ~i; i = ne[i] )
			{
				int j = e[i];
				if ( in_queue[j] )
					continue;
				din[j]--;
				if ( din[j] <= lb - paramL )
				{
					in_queue[j] = true;
					q.push ( j );
				}
			}
			for ( int i = ht[u]; ~i; i = ne[i] )
			{
				int j = e[i];
				if ( in_queue[j] )
					continue;
				dout[j]--;
				if ( dout[j] <= lb - paramK )
				{
					in_queue[j] = true;
					q.push ( j );
				}
			}
		}
		//
		for(int i=0;i<(int)edges.size();i++)
		{
			if(v_remove[edges[i].from] || v_remove[edges[i].to])
				edge_remove[i]=true;
		}
		sort(edges.begin(),edges.end(),cmp_edge);
		int ed=edges.size()-1;
		//0~ed
		while(ed>=0 && edge_remove[edges[ed].id])
		{
			ed--;
		}
		edges.erase(edges.begin()+ed+1,edges.end());
		for(int i=0;i<(int)edges.size();i++)//
		{
			edges[i].id=i;
		}
		vector<int> points;
		for(int i=0;i<tot_vertex_cnt;i++)
			if(!v_remove[i])
				points.push_back(i);
		for(int i=0;i<(int)edges.size();i++)//
		{
			edges[i].from=find(points,edges[i].from);
			edges[i].to=find(points,edges[i].to);
		}
		memset(edge_remove,0,sizeof(bool)*edges.size());
		memset(v_remove,0,sizeof(bool)*points.size());
		tot_vertex_cnt=points.size();

		delete[] h;
		delete[] ht;
		delete[] e;
		delete[] ne;
		delete[] din;
		delete[] dout;
		delete[] in_queue;
	}
};

#endif // SEARCH_H_INCLUDED
