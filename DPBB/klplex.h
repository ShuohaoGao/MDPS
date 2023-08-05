#ifndef KLPLEX_H_INCLUDED
#define KLPLEX_H_INCLUDED

#include "graph.h"

class KLplex
{
private:
	int n;//
	int m;//
	// inline bool add_edge ( int edge_id ) //
	inline bool add_edge ( int from,int to ) //
	{

#ifdef NOT_FINISHED
		if ( neighbor_out[from].count ( to ) )
			printf ( "S,neighbor_out[%d]%d\n", from, to );
		if ( neighbor_in[to].count ( from ) )
			printf ( "S,neighbor_in[%d]%d\n", to, from );
#endif // NOT_FINISHED

		m++;
		neighbor_in[to].insert ( from );
		neighbor_out[from].insert ( to );
		return true;
	}
public:
	set<int> vertex;//
	unordered_map<int, set<int> > neighbor_out;//u
	unordered_map<int, set<int> > neighbor_in;//u
	KLplex()
	{
		need_check = false;
		changed = false;
	}
	void clear()
	{
		vertex.clear();
		neighbor_out.clear();
		neighbor_in.clear();
		n=m=0;
	}

	int last_add_vertex;//
	bool changed;
	bool need_check;//lb,,S
	inline int vertex_cnt()
	{
#ifdef NOT_FINISHED
		if ( n != ( int ) vertex.size() )
		{
			printf ( "!!!S.vertex_cnt n=%d vertex.sz=%d\n", n, ( int ) vertex.size() );
			n = vertex.size();
		}
#endif // NOT_FINISHED
		return n;
	}
	inline int edge_cnt()
	{
		return m;
	}

	//a->b
	inline bool exist_edge ( int a, int b )
	{
		return neighbor_out[a].count ( b ) > 0;
	}
	//
	inline int get_in_degree ( int v )
	{
		return ( int ) neighbor_in[v].size();
	}
	//
	inline int get_out_degree ( int v )
	{
		return ( int ) neighbor_out[v].size();
	}
	inline bool k_satisfied ( int u, int lb )
	{
		return get_out_degree ( u ) >= lb - paramK;
	}
	inline bool l_satisfied ( int u, int lb )
	{
		return get_in_degree ( u ) >= lb - paramL;
	}
	// inline bool satisfied ( int u ) //
	// {
	// 	return k_satisfied ( u, lb + 1 ) && l_satisfied ( u, lb + 1 );
	// }
	inline bool satisfied ( int u, int lb )
	{
		return k_satisfied ( u, lb ) && l_satisfied ( u, lb );
	}
	//gu
	void add_vertex ( int u, Graph &g, int type = 0 )
	{
		clock_t st=clock();
		if ( vertex.count ( u ) )
		{
			printf ( "!!! S.add_vertex  u=%d type=%d U->count=%d\n", u, type, ( int ) U->count ( u ) );
			exit ( 1 );
			// return ;
		}
		changed = true;
		last_add_vertex = u;
		n++;
		vertex.insert ( u );
		U->erase ( u );
		//
		for(int v:g.neighbor_out(u))
		{
			if(has(v))
			{
				add_edge(u,v);
			}
		}
		for(int v:g.neighbor_in(u))
		{
			if(has(v))
				add_edge(v,u);
		}
		Saddv+=clock()-st;
		// add_to_S(u);
	}
	void remove_vertex ( int v ) //v
	{
		clock_t st=clock();
#ifdef NOT_FINISHED
		if ( !has ( v ) )
		{
			printf ( "remove_vertex Sv=%d\n", v );
			return;
		}
#endif //NOT_FINISHED
		n--;
		changed = true;
		m -= get_in_degree(v) +get_out_degree(v);
		vertex.erase ( v );
		for(int u:neighbor_out[v])
			neighbor_in[u].erase(v);
		for(int u:neighbor_in[v])
			neighbor_out[u].erase(v);
		neighbor_in.erase ( v );
		neighbor_out.erase ( v );
		Srmv+=clock()-st;
		// remove_from_S(v);
	}
	//Su
	int get_non_out_neighbor ( int u, Graph &g )
	{
		int ret = vertex_cnt();
		for ( int v : vertex )
		{
			if ( g.exist_edge ( u, v ) )
				ret--;
		}
		return ret;
	}
	//Su
	int get_non_in_neighbor ( int u, Graph &g )
	{
		int ret = vertex_cnt();
		for ( int v : vertex )
		{
			if ( g.exist_edge ( v, u ) )
				ret--;
		}
		return ret;
	}
	//k-l
	bool is_klplex ()
	{
		for ( int v : vertex )
		{
			if ( !satisfied ( v, n ) )
				return false;
		}
		return true;
	}
	inline bool has ( int v )
	{
		return vertex.count ( v ) > 0;
	}
	bool need_check_again() //
	{
		for ( int v : vertex )
		{
#ifdef NOT_FINISHED
			if ( !v_remove[v] && !full_g.has ( v ) )
			{
				puts ( "v_remove" );
				v_remove[v] = true;
			}
#endif // NOT_FINISHED
			if ( v_remove[v] ) //
			{
				need_check = true;
				return true;
			}
		}
		//,
		need_check = false;
		return false;
	}
};

KLplex plex;

inline bool update_lower_bound ( KLplex &plex );
inline bool update_lower_bound ( Graph &plex );

#endif // KLPLEX_H_INCLUDED
