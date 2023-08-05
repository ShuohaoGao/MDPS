#pragma GCC optimize(2)

#include "exact_search.h"
#include "strong_reduce.h"

#define NAME_LEN 100


inline void after_lb_changed()
{
	printf ( "================= update lb=%d =======================\n", lb );
	Strong_reduce strong(&full_g);
	strong.strong_reduce();
	if(lb >= last_lb)
	{
		for(int i=0;i<tot_vertex_cnt;i++)
			if(!v_remove[i])
				full_g.remove_vertex_forever(i);
	}
	printf ( "after reduction: n= %d ,m= %d\n", full_g.vertex_cnt(), full_g.edge_cnt() );
	plex.need_check = true; 
	set<int> vertex_remove;
	for ( int v : U->all() )
	{
		if ( !full_g.has ( v ) )
			vertex_remove.insert ( v );
	}
	Sub ( U, vertex_remove );
	puts ( "" );
	set<int> temp;
	copy(temp,g.vertex->all());
	for ( int v : temp ) 
	{
		if ( v_remove[v] )
			g.remove_vertex_forever ( v );
	}
}

inline bool update_lower_bound ( KLplex &plex ) 
{
	if ( lb < plex.vertex_cnt() )
	{
		lb = plex.vertex_cnt();
		max_KLplex.clear();
		copy_set ( plex.vertex, max_KLplex );
		after_lb_changed();
		return true;
	}
	return false;
}
inline bool update_lower_bound ( Graph &plex )
{
	if ( lb < plex.vertex_cnt() )
	{
		lb = plex.vertex_cnt();
		max_KLplex.clear();
		copy(max_KLplex,plex.vertex->all());
		after_lb_changed();
		return true;
	}
	return false;
}

char file_path[NAME_LEN];
string ori_file_name;

void print_usage()
{
}

int main ( int argc, char **argv )
{
	if ( argc != 4 )
	{
		puts ( "wrong param!" );
		print_usage();
		exit ( 1 );
	}
	else
	{
		memcpy ( file_path, argv[1], NAME_LEN * sizeof ( char ) );
		ori_file_name=get_file_name(file_path);
		string log_file_name="./log/log_"+ori_file_name+".log";

		paramK = atoi ( argv[2] );
		paramL = atoi ( argv[3] );
		printf ( "param k=%d l=%d\n", paramK, paramL );
		read_from_file ( file_path );
		cout << "read ok,n=" << tot_vertex_cnt <<" m="<<edges.size()<<" lb="<<lb<< endl;
		arrayManager=new ArrayManager(tot_vertex_cnt,edges.size());
	}
	g.init ( tot_vertex_cnt,edges ); 
	full_g.init ( tot_vertex_cnt,edges ); 

	U=new ArraySet(tot_vertex_cnt,U_TYPE);
	Exact_search e_search;
	e_search.Include_Exclude();
	printf("ground truth= %d \n",lb);
	printf("DPBB time= %.2lf\n",(double)(clock()-start_time)/CLOCKS_PER_SEC);


	return 0;
}
