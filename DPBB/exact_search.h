#ifndef EXACT_SEARCH_H_INCLUDED
#define EXACT_SEARCH_H_INCLUDED

#include "upperbound.h"
#include "reduce_graph.h"

bool cmp_out(int u,int v)
{
	return g.get_out_degree(u) < g.get_out_degree(v) || (g.get_out_degree(u) == g.get_out_degree(v) && g.get_in_degree(u)<g.get_in_degree(v));
}

bool cmp_in(int u,int v)
{
	return g.get_in_degree(u) < g.get_in_degree(v) || (g.get_in_degree(u)==g.get_in_degree(v) && g.get_out_degree(u) < g.get_out_degree(v));
}

bool cmp_min(int u,int v)
{
	int a= min(g.din[u]+paramL,paramK+ g.dout[u]) ;
	int b= min(g.din[v]+paramL,paramK+g.dout[v]);
	return a<b;
}

class Exact_search
{
public:
	int dfs_cnt = 0;
	int vertex_remove_from_U;
	Exact_search()
	{
		dfs_cnt = 0;
		U->copy(g.vertex);
		estimator.init();
		vertex_remove_from_U=-1;
	}

	void Include_Exclude()
	{
		vector<int> V;
		for ( int v : g.vertex->all() )
		{
			V.push_back ( v );
		}
		sort(V.begin(),V.end(),cmp_min);
		for(int v:V)
		{
			S.add_vertex(v,full_g,1111);
			U->erase(v);
			vertex_remove_from_U=-1;
			if(!v_remove[v])
				branch_and_bound(1);
			S.remove_vertex(v);
			if(full_g.has(v))
				full_g.remove_vertex_forever(v);
			if(g.has(v))
				g.remove_vertex_forever(v);
		}
	}
	

	void branch_and_bound ( int depth, int fa = -1 )
	{
		dfs_cnt++;
#ifdef NOT_FINISHED
		// if ( dfs_cnt % 10000 == 0 )
		// {
		// 	printf("%d\n",dfs_cnt);
		// 	fflush(stdout);
		// 	if(dfs_cnt % 50000 ==0)
		// 	{
		// 		printf("srmv=%.4lf saddv=%.4lf grmv=%.4lf grb=%.4lf ub=%.4lf erfen=%.4lf prone=%.4lf jump=%.4lf tot=%.4lf\n",
		// 			Srmv,Saddv,grmv,grb,uptime,erfen,proneTime,jump,(double)(clock()-start_time));
		// 	}
		// }
#endif //NOT_FINISHED

		int cur = dfs_cnt;
		if ( S.vertex_cnt() == g.vertex_cnt() )
			return ;

		int upperbound= vertex_remove_from_U==-1 ? 
						estimator.getNewUpperBound() : 
						estimator.getIncremental(vertex_remove_from_U);
		if ( upperbound <= lb )
		{
			return;
		}

		Reduce_graph reduce;
		vector<int> V_reduce;
		vector<int> E_reduce;
		double st_prone=clock();
		int v=-1;
		bool ok=0;
		if(depth==1)
		{
			reduce.prune2 ( V_reduce, E_reduce );
			bool ret=reduce.reduce_to_2jump_neighbor ( V_reduce, E_reduce );
			if(!ret)
				goto Return_Label;
		}
		if ( dfs_cnt % 3 == 0 ) 
		{
			reduce.prune1_weak_reduce ( g, V_reduce, E_reduce );
		}
		else if ( dfs_cnt % 9 == 1 ) 
			reduce.prune2 ( V_reduce, E_reduce );

		// if ( dfs_cnt % 3 == 2) 
		{
			bool ret=reduce.reduce_to_2jump_neighbor ( V_reduce, E_reduce );
			if(!ret)
				goto Return_Label;
		}
		proneTime+=clock()-st_prone;

		for(int u:U->all())
		{
			if(v==-1 || cmp_min(u,v))
			{
				v=u;
			}
		}
		if ( v == -1 ) 
		{
			update_lower_bound ( g );
			goto Return_Label;
		}
		else
		{
			vector<int> temp(1);
			temp[0]=v;
			vector<int> edges_id = g.remove_vertex ( temp );
			S.changed=false;
			if(!V_reduce.size()) vertex_remove_from_U=v;
			else vertex_remove_from_U=-1;
			branch_and_bound ( depth + 1, cur );
			g.rollback ( temp, edges_id );
			if ( ( S.need_check && S.need_check_again() ) || !full_g.has ( v ) )
			{
				goto Return_Label;
			}
			S.add_vertex ( v, g, 1 );
			if ( S.is_klplex () == false )
			{
				S.remove_vertex ( v );
				U->insert ( v );
				goto Return_Label;
			}
			else
			{
				update_lower_bound ( S );
				if (S.need_check && S.need_check_again() ) 
				{
					S.remove_vertex (v  );
					if ( !v_remove[v] )
						U->insert ( v );
					goto Return_Label;
				}
				vertex_remove_from_U=-1;
				branch_and_bound ( depth + 1, cur );
				S.remove_vertex (v  );
				if ( !v_remove[v] )
					U->insert ( v );
			}
		}
Return_Label:
		g.rollback ( V_reduce, E_reduce );
	}
};

#endif // EXACT_SEARCH_H_INCLUDED
