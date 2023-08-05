#ifndef REDUCE_GRAPH_H_INCLUDED
#define REDUCE_GRAPH_H_INCLUDED

#include "klplex.h"
#include "map.h"

class Reduce_graph
{
public:
	//(),
	set<int> weak_reduce ( Graph &G )
	{
		queue<int> q;
		set<int> vertex_removed; //
		for ( int u : G.vertex->all() ) //,
		{
			if ( G.get_out_degree ( u ) <= lb - paramK || G.get_in_degree ( u ) <= lb - paramL )
			{
				q.push ( u );
				v_remove[u]=true;
			}
		}
		while ( q.size() )
		{
			int u = q.front();
			q.pop();
#ifdef NOT_FINISHED
			if ( !G.has ( u ) )
			{
				puts ( "v_remove" );
				v_remove[u] = true;
				continue;
			}
#endif // NOT_FINISHED
			if(G.head[u]!=-1)
			{
				for(int i=G.head[u];i!=G.next[u];i++)
				{
					if(G.rm_edge[i])	continue;
					int to=edges[i].to;
					if ( G.get_in_degree ( to ) - 1 <= lb - paramL && !v_remove[to] )
					{
						q.push ( to );
						v_remove[to]=true;
					}
				}
			}
			if(G.reverse_head[u]!=-1)
			{
				for(int p=G.reverse_head[u];p<(int)G.reverse_edges.size() && G.reverse_edges[p].from==u;p++)
				{
					int id=G.reverse_edges[p].id;
					if(G.rm_edge[ id ])	continue;
					int from=G.reverse_edges[p].to;
					if ( G.get_out_degree ( from ) - 1 <= lb - paramK && !v_remove[from] )
					{
						q.push ( from );
						v_remove[from]=true;
					}
				}
			}
			G.remove_vertex_forever ( u );
		}
		return vertex_removed;
	}

	//()
	void strong_reduce ( Graph &g, int max_vertex_cnt = 460000 )
	{
		clock_t st = clock();
		if ( g.vertex_cnt() > max_vertex_cnt )
		{
			puts ( "," );
			return;
		}
		queue<int> q_v;//
		queue<pii> q_e;//
		Map* common_out_neighbor_cnt=get_suitable_map(tot_vertex_cnt);
		Map* common_in_neighbor_cnt=get_suitable_map(tot_vertex_cnt);
		for(int u:g.vertex->all())
		{
			for(int v:g.neighbor_out(u))
			{
				int out_cnt = 0; //
				for ( int w : g.neighbor_out(v) )
				{
					if ( g.exist_edge ( u, w ) )
						out_cnt++;
				}
				common_out_neighbor_cnt->put({u,v},out_cnt);
				int in_cnt = 0; //
				for ( int w : g.neighbor_in(v) )
				{
					if ( g.exist_edge ( w, u ) )
						in_cnt++;
				}
				common_in_neighbor_cnt->put({u,v},in_cnt);

				if ( g.exist_edge ( v, u ) )
				{
					if ( out_cnt <= lb - 2 * paramK || in_cnt <= lb - 2 * paramL )
					{
						q_e.push ( {u, v} );
						q_e.push ( {v, u} );
					}
				}
				else
				{
					if ( out_cnt <= lb - 2 * paramK + 1 || in_cnt <= lb - 2 * paramL + 1 )
					{
						q_e.push ( {u, v} );
					}
				}
			}
			for(int v:g.neighbor_in(u))
			{
				if(g.exist_edge(u,v)) continue;//v
				int out_cnt = 0; //
				for ( int w : g.neighbor_out(v) )
				{
					if ( g.exist_edge ( u, w ) )
						out_cnt++;
				}
				common_out_neighbor_cnt->put({u,v},out_cnt);
				int in_cnt = 0; //
				for ( int w : g.neighbor_in(v) )
				{
					if ( g.exist_edge ( w, u ) )
						in_cnt++;
				}
				common_in_neighbor_cnt->put({u,v},in_cnt);

				if ( out_cnt <= lb - 2 * paramK + 1 || in_cnt <= lb - 2 * paramL + 1 )
				{
					q_e.push ( {v,u} );
				}
			}
		}

		// while(q_e.size())
		// {
		// 	pii hh=q_e.front();
		// 	q_e.pop();
		// 	g.remove_edge_forever(hh.first,hh.second);
		// }
		
		set<int> vertex_removed;

		// for(int v:g.vertex)
		// {
		// 	if(g.get_in_degree(v) <= lb-paramL || g.get_out_degree(v) <= lb-paramK)
		// 		vertex_removed.insert(v);
		// }
		// for(int v:vertex_removed)
		// 	g.remove_vertex_forever(v);

		while ( q_v.size() || q_e.size() )
		{
			while ( q_e.size() )
			{
				pii edge = q_e.front();
				q_e.pop();
				int from = edge.first, to = edge.second;
				if ( !g.remove_edge_forever ( from, to ) ) //false
					continue;
				if ( !g.satisfied ( from ) && !v_remove[from] )
				{
					q_v.push ( from );
					v_remove[from] = 1;
				}
				if ( !g.satisfied ( to ) && !v_remove[to] )
				{
					q_v.push ( to );
					v_remove[to] = 1;
				}

				//
				for ( int w : g.neighbor_out(from) )
				{
					if ( !g.exist_edge ( w, to ) )
						continue;
					common_out_neighbor_cnt->reduce({from, w});
//					int val = common_out_neighbor_cnt[ {w, from}];
					if ( g.exist_edge ( w, from ) )
					{
						if ( common_out_neighbor_cnt->get({from,w}) <= lb - 2 * paramK )
						{
							q_e.push ( {from, w} );
							q_e.push ( {w, from} );
						}
					}
					else
					{
						if ( common_out_neighbor_cnt->get({from,w}) <= lb - 2 * paramK + 1 )
						{
							q_e.push ( {from, w} );
						}
					}
				}
				for ( int w : g.neighbor_in(from) )
				{
					if ( !g.exist_edge ( w, to ) )
						continue;
					if ( g.exist_edge ( from, w ) )
						continue;//,
					common_out_neighbor_cnt->reduce({from, w});
					if ( common_out_neighbor_cnt->get({from,w})<= lb - 2 * paramK + 1 )
					{
						q_e.push ( {w, from} );
					}
				}
				for ( int w : g.neighbor_in(to) )
				{
					if ( !g.exist_edge ( from, w ) )
						continue;
					common_in_neighbor_cnt->reduce({to,w});
					if ( g.exist_edge ( to, w ) )
					{
						if ( common_in_neighbor_cnt->get({to,w}) <= lb - 2 * paramL )
						{
							q_e.push ( {to, w} );
							q_e.push ( {w, to} );
						}
					}
					else
					{
						if ( common_in_neighbor_cnt->get({to,w})<= lb - 2 * paramL + 1 )
						{
							q_e.push ( {w, to} );
						}
					}
				}
				for ( int w : g.neighbor_out(to) )
				{
					if ( !g.exist_edge ( from, w ) )
						continue;
					if ( g.exist_edge ( w, to ) )
						continue;//,
					common_in_neighbor_cnt->reduce({to,w});
					if ( common_in_neighbor_cnt->get({to,w}) <= lb - 2 * paramL + 1 )
					{
						q_e.push ( {to, w} );
					}
				}
			}
			if ( q_v.size() )
			{
				int u = q_v.front();
				q_v.pop();
#ifdef NOT_FINISHED
				if ( vertex_removed.count ( u ) )
				{
					cout << u << "\n";
					continue;
				}
				if ( !g.has ( u ) )
				{
					puts ( "strong reduce v_remove" );
					continue;
				}
#endif // NOT_FINISHED
				for ( int to : g.neighbor_out(u) ) //
				{
					if ( g.get_in_degree ( to ) - 1 <= lb - paramL && !v_remove[to] )
					{
						q_v.push ( to );
						v_remove[to] = 1;
					}
				}
				for ( int from : g.neighbor_in(u) ) //
				{
					if ( g.get_out_degree ( from ) - 1 <= lb - paramK && !v_remove[from] )
					{
						q_v.push ( from );
						v_remove[from] = 1;
					}
				}
				g.remove_vertex_forever ( u );
				vertex_removed.insert ( u );
			}
		}
		cout<<"="<<vertex_removed.size()<<endl;
		strong_reduce_time += clock() - st;
		
		delete common_out_neighbor_cnt;
		delete common_in_neighbor_cnt;
	}

	//1(),V,E
	void prune1_weak_reduce ( Graph &g, vector<int> &V, vector<int> &E )
	{
		vector<int> temp ( 1 );
		int *q=arrayManager->get_n_q();
		int *vis=arrayManager->get_huafen();
		int hh=0,tt=-1;
		vector<int> vertex_U=U->all();
		for ( int u :vertex_U ) //,
		{
			if ( g.get_out_degree ( u ) <= lb - paramK || g.get_in_degree ( u ) <= lb - paramL )
			{
				q[++tt]=u;
				vis[u]=1;
			}
		}
		while ( hh<=tt )
		{
			int u=q[hh++];

			for ( int to : g.neighbor_out(u) ) //
			{
				if ( g.get_in_degree ( to ) - 1 <= lb - paramL && !vis[to] && U->count(to))
				{
					q[++tt]=to;
					vis[to]=1;
				}
			}
			for ( int from : g.neighbor_in(u) ) //
			{
				if ( g.get_out_degree ( from ) - 1 <= lb - paramK && !vis[from] && U->count(from) )
				{
					q[++tt]=from;
					vis[from]=1;
				}
			}
			temp[0] = u;
			vector<int> ret_edge = g.remove_vertex ( temp );
			for ( int edge_id : ret_edge )
				E.push_back ( edge_id );
		}
		for(int i=0;i<=tt;i++)
		{
			int v=q[i];
			vis[v]=0;
			V.push_back ( v );
		}
	}

	void prune2 ( vector<int> &V, vector<int> &E )
	{
		int *din=arrayManager->get_erfen();
		int *dout=arrayManager->get_huafen();
		for(int u:S.vertex)
		{
			for(int v:g.neighbor_out(u))
				din[v]++;
			for(int v:g.neighbor_in(u))
				dout[v]++;
		}
		vector<int> v_removed;
		vector<int> temp=U->all();
		int n=S.vertex_cnt();
		for ( int u :temp )
		{
			int non_in_cnt = n - din[u];
			int non_out_cnt = n - dout[u];
			din[u]=dout[u]=0;
			if ( non_in_cnt + 1 > paramL || non_out_cnt + 1 > paramK )
				v_removed.push_back ( u );
		}
		for(int v:S.vertex)
			din[v]=dout[v]=0;
		for ( int edge_id : g.remove_vertex ( v_removed ) )
			E.push_back ( edge_id );
		for ( int v : v_removed )
			V.push_back ( v );
	}
	
	// void prune3( vector<int> &V, vector<int> &E )
	// {
	// 	int *q_in=arrayManager->get_n_q();
	// 	int hh=0,tt=-1;
	// 	int n=S.vertex_cnt();
	// 	for(int v:S.vertex)
	// 		if(S.get_in_degree(v) == n-paramL)
	// 		{
	// 			q_in[++tt]=v;
	// 		}
		
	// }

	//,SKL;return falseS
	bool reduce_to_2jump_neighbor_in ( vector<int> &V, vector<int> &E, int u )
	{
		int pre=V.size();
		double st=clock();
		int *neighbor=arrayManager->get_n_q();//bfs
		int tt=-1;//
		bool *vis=arrayManager->get_n_vis();//
		for(int v:g.neighbor_in(u))//
		{
			vis[v]=1;
			neighbor[++tt]=v;
		}
		vis[u]=1;
		neighbor[++tt]=u;
		int last_tt=tt;
		for(int i=0;i<=last_tt;i++)
		{
			vector<int> ret=g.neighbor_in(neighbor[i]);
			for(int v:ret)
			{
				if(!vis[v])
				{
					vis[v]=1;
					neighbor[++tt]=v;
				}
			}
		}
		int sz = V.size(); //c++set,,,
		//SS;exact_search
		for(int v:S.vertex)
		{
			if(!vis[v])
			{
				for(int i=0;i<=tt;i++)
					vis[neighbor[i]]=0;
				return false;
			}
		}
		for ( int v : U->all() )
		{
			if(!vis[v])//,
			{
				V.push_back ( v );
			}
		}
		vector<int> temp ( 1 );
		for ( int i = sz; i < ( int ) V.size(); i++ )
		{
			temp[0] = V[i];
			for ( int edge_id : g.remove_vertex ( temp ) )
				E.push_back ( edge_id );
		}
		for(int i=0;i<=tt;i++)
			vis[neighbor[i]]=0;
		jump+=clock()-st;
		return true;
	}

	bool reduce_to_2jump_neighbor ( vector<int> &V, vector<int> &E, int type = -1 )
	{
		// return true;
		if( lb <= paramK + paramL -3)
			return true;
		int u = S.last_add_vertex;
		if ( !S.has ( u ) )
			return true;
		S.last_add_vertex=-1;
		double st=clock();
		int *neighbor=arrayManager->get_n_q();//bfs
		int tt=-1;//
		bool *vis=arrayManager->get_n_vis();//
		for(int v:g.neighbor_out(u))//
		{
			// if(!g.exist_edge(v,u)) continue;
			vis[v]=1;
			neighbor[++tt]=v;
		}
		vis[u]=1;
		neighbor[++tt]=u;
		int last_tt=tt;
		for(int i=0;i<=last_tt;i++)
		{
			vector<int> ret=g.neighbor_out(neighbor[i]);
			for(int v:ret)
			{
				if(!vis[v])
				{
					vis[v]=1;
					neighbor[++tt]=v;
				}
			}
		}
		int sz = V.size(); //c++set,,,
		//SS;exact_search
		for(int v:S.vertex)
		{
			if(!vis[v])
			{
				for(int i=0;i<=tt;i++)
					vis[neighbor[i]]=0;
				return false;
			}
		}
		for ( int v : U->all() )
		{
			if(!vis[v])//,
			{
				V.push_back ( v );
			}
		}
		vector<int> temp ( 1 );
		for ( int i = sz; i < ( int ) V.size(); i++ )
		{
			temp[0] = V[i];
			for ( int edge_id : g.remove_vertex ( temp ) )
				E.push_back ( edge_id );
		}
		for(int i=0;i<=tt;i++)
			vis[neighbor[i]]=0;
		jump+=clock()-st;
		return true;
		return reduce_to_2jump_neighbor_in(V,E,u);
	}

};

#endif // REDUCE_GRAPH_H_INCLUDED
