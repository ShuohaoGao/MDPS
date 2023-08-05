#ifndef UPPERBOUND_H_INCLUDED
#define UPPERBOUND_H_INCLUDED

#include "klplex.h"

class UpperBound
{
private:
	int N;
	int *pos_in,*pos_out;
	int rm_cnt_in;//π0>=2ub
	int rm_cnt_out;
	int *h_in;
	int *h_out;
	int ub_in;
	int ub_out;
	int du_ub;//
	//
	int color ( Graph &g )
	{
		vector<set<int>> p;
		set<int> st;
		int cnt = 0;
		//cnt
		for ( int v : g.vertex->all() )
		{
			bool found = false;
			for ( int i = 0; i < cnt; i++ )
			{
				bool ok = true;
				for ( int u : p[i] )
				{
					if ( g.exist_edge ( u, v ) || g.exist_edge ( v, u ) )
					{
						ok = false;
						break;
					}
				}
				if ( ok )
				{
					p[i].insert ( v );
					found = true;
					break;
				}
			}
			if ( !found )
			{
				cnt++;
				set<int> temp;
				temp.insert ( v );
				p.push_back ( temp );
			}
		}
		int ret = 0;
		int k = min ( paramK, paramL ); //k
		for ( int i = 0; i < cnt; i++ )
			ret += min ( k, ( int ) p[i].size() );
		return ret;
	}
	//g|S|+1,π0S,πivi
	int partition_in_pre ( KLplex &S, Graph &g )
	{
		int *h=arrayManager->get_huafen();//vi∈S,h[s]πi
		vector<int> temp;
		int *vertex=arrayManager->get_n_q();
		int n=0;
		for(int v:S.vertex)
			vertex[n++]=v;
		for ( int u : U->all() )
		{
			bool found=false;
			for (int i=0;i<n;i++)
			{
				int v=vertex[i];
				if ( g.exist_edge ( u, v ) == false ) //
				{
					h[v]++;
					found=true;
					break;
				}
			}
			if(!found)
				temp.push_back(u);
		}
		int sz=temp.size();
	#ifdef ER_FEN	
		// if(sz)
		// {
		// 	double aaa=clock();
		// 	int *vis=arrayManager->get_erfen();
		// 	for(int i=0;i<sz;i++)
		// 		vis[temp[i]]=i+1;
		// 	vector<int> din(sz);
		// 	vector<int> dout(sz);
		// 	int cnt=0;
		// 	for(int i=0;i<sz;i++)//temp
		// 	{
		// 		int u=temp[i];
		// 		if(g.head[u] !=-1)
		// 		{
		// 			for(int j=g.head[u]; j!= g.next[u]; j++)
		// 			{
		// 				cnt++;
		// 				if(g.rm_edge[j]) continue;
		// 				int v=edges[j].to;
		// 				if(vis[v])
		// 				{
		// 					dout[i]++;
		// 					din[vis[v]-1]++;
		// 				}
		// 			}
		// 		}
		// 	}
		// 	din.push_back(-INF);
		// 	din.push_back(INF);
		// 	dout.push_back(-INF);
		// 	dout.push_back(INF);
		// 	sort(din.begin(),din.end());
		// 	sort(dout.begin(),dout.end());
		// 	int l=min(paramK,paramL),r=sz;
		// 	while(l<r)
		// 	{
		// 		int mid=(l+r+1)>>1;
		// 		int cnt_in=sz-get_cnt_lower_than(din,mid-paramL);
		// 		int cnt_out=sz-get_cnt_lower_than(dout,mid-paramK);
		// 		if(min(cnt_in,cnt_out) <mid)	r=mid-1;
		// 		else l=mid;
		// 	}
		// 	// rgb[sz-r]++;
		// 	sz=r;
		// 	int ttt=temp.size();
		// 	for(int i=0;i<ttt;i++)
		// 		vis[temp[i]]=0;
		// 	erfen+=clock()-aaa;
		// }
		if(sz)
		{
			double aaa=clock();
			for(int i=0;i<n;i++)
			{
				temp.push_back(vertex[i]);
			}
			sz=temp.size();
			
			int *vis=arrayManager->get_erfen();//π0
			for(int i=0;i<sz;i++)
				vis[temp[i]]=i+1;
			//szklplex
			vector<int> din(sz);
			vector<int> dout(sz);
			
			for(int i=0;i<sz;i++)//temp
			{
				int u=temp[i];
				if(g.head[u] !=-1)
				{
					for(int j=g.head[u]; j!= g.next[u]; j++)
					{
						if(g.rm_edge[j]) continue;
						int v=edges[j].to;
						if(vis[v])
						{
							dout[i]++;
							din[vis[v]-1]++;
						}
					}
				}
			}
			//
			din.push_back(-INF);
			din.push_back(INF);
			dout.push_back(-INF);
			dout.push_back(INF);
			sort(din.begin(),din.end());
			sort(dout.begin(),dout.end());
			int l=min(paramK,paramL),r=sz;
			
			while(l<r)
			{
				int mid=(l+r+1)>>1;
				int cnt_in=sz-get_cnt_lower_than(din,mid-paramL);
				int cnt_out=sz-get_cnt_lower_than(dout,mid-paramK);
				if(min(cnt_in,cnt_out) <mid)	r=mid-1;
				else l=mid;
			}
			sz=r- n;
			int ttt=temp.size();
			for(int i=0;i<ttt;i++)
				vis[temp[i]]=0;
			erfen+=clock()-aaa;
		}
	#endif
		int ret = n + sz;
		for ( int i=0;i<n;i++ )
		{
			int v=vertex[i];
			ret += min ( h[v], paramL - n + S.get_in_degree ( v ) );
		}
		for(int i=0;i<n;i++)
			h[vertex[i]]=0;
		return ret;
	}
	//g|S|+1,π0S,πivi;π0
	int partition_out_pre ( KLplex &S, Graph &g )
	{
		int *h=arrayManager->get_huafen();//vi∈S,h[s]πi
		vector<int> temp;//:u∈temp,uS
		int n=0;
		int *vertex=arrayManager->get_n_q();
		for(int v:S.vertex)
			vertex[n++]=v;
		for ( int u : U->all() )
		{
			bool found=false;
			for(int i=0;i<n;i++)
			{
				int v=vertex[i];
				if ( g.exist_edge ( v, u ) == false ) //
				{
					h[v]++;
					found=true;
					break;
				}
			}
			if(!found)
				temp.push_back(u);
		}
		int sz=temp.size();
		// bool right=sz==(int)out->s->vertex.size();
		// if(!right)
		// {
		// 	cout<<"PI0"<<endl;
		// 	cout<<sz<<' '<<out->s->vertex.size()<<endl;
		// 	for(int u:U->all())
		// 	{
		// 		int pos=out->pos[u];
		// 		if(out->pos[u]==-1)
		// 		{
		// 			cout<<"U"<<endl;
		// 		}else if(pos ==tot_vertex_cnt)
		// 		{
		// 			cout<<"PI0"<<endl;
		// 		}else if(g.exist_edge(pos,u))
		// 		{
		// 			cout<<""<<endl;
		// 		}
		// 	}
		// 	exit(1);
		// }
		// for(int u:temp)
		// 	right &= out->s->has(u);
		// if(!right)
		// {
		// 	cout<<"PI0"<<endl;
		// 	exit(1);
		// }
		// int cnt1=0,cnt2=0;
		// for(int v:S.vertex)
		// {
		// 	cnt1+=h[v];
		// 	cnt2+=out->PI[v].size();
		// }
		// right &= cnt1==cnt2;
		// if(!right)
		// {
		// 	cout<<"PI"<<endl;
		// 	exit(1);
		// }
		// for(int u:U->all())
		// {
		// 	int pos=out->pos[u];
		// 	if( pos!= -1 && pos !=tot_vertex_cnt)
		// 	{
		// 		right &= !g.exist_edge(pos,u);
		// 	}
		// }
		// if(!right)
		// {
		// 	cout<<""<<endl;
		// 	exit(1);
		// }
		// if(!right)
		// {
		// 	cout<<""<<endl;
		// 	for(int v:S.vertex)
		// 	{
		// 		cout<<v<<' '<<out->PI[v].size()<<' '<<h[v]<<endl;
		// 	}
		// 	for(int u:U->all())
		// 	{
		// 		if(out->pos[u]==-1)
		// 		{
		// 			cout<<"U"<<endl;
		// 		}
		// 	}
		// 	for(int i=0;i<tot_vertex_cnt;i++)
		// 		if(out->pos[i]!=-1 && out->pos[i]!=tot_vertex_cnt)
		// 		{
		// 			cout<<i<<' '<<U->count(i)<<endl;
		// 		}
		// 	exit(1);
		// }else cout<<""<<endl;
	#ifdef ER_FEN
		// if(sz)
		// {
		// 	double aaa=clock();
		// 	int *vis=arrayManager->get_erfen();//π0
		// 	for(int i=0;i<sz;i++)
		// 		vis[temp[i]]=i+1;
		// 	//szklplex
		// 	vector<int> din(sz);
		// 	vector<int> dout(sz);
		// 	double st=clock();
		// 	for(int i=0;i<sz;i++)//temp
		// 	{
		// 		int u=temp[i];
		// 		if(g.head[u] !=-1)
		// 		{
		// 			for(int j=g.head[u]; j!= g.next[u]; j++)
		// 			{
		// 				if(g.rm_edge[j]) continue;
		// 				int v=edges[j].to;
		// 				if(vis[v])
		// 				{
		// 					dout[i]++;
		// 					din[vis[v]-1]++;
		// 				}
		// 			}
		// 		}
		// 	}
		// 	//
		// 	din.push_back(-INF);
		// 	din.push_back(INF);
		// 	dout.push_back(-INF);
		// 	dout.push_back(INF);
		// 	sort(din.begin(),din.end());
		// 	sort(dout.begin(),dout.end());
		// 	int l=min(paramK,paramL),r=sz;
		// 	while(l<r)
		// 	{
		// 		int mid=(l+r+1)>>1;
		// 		int cnt_in=sz-get_cnt_lower_than(din,mid-paramL);
		// 		int cnt_out=sz-get_cnt_lower_than(dout,mid-paramK);
		// 		if(min(cnt_in,cnt_out) <mid)	r=mid-1;
		// 		else l=mid;
		// 	}
		// 	// rgb[sz-r]++;
		// 	sz=r;
		// 	for(int x:temp)
		// 		vis[x]=0;
		// 	erfen+=clock()-aaa;
		// }
		if(sz)
		{
			for(int i=0;i<n;i++)
			{
				temp.push_back(vertex[i]);
			}
			sz=temp.size();
			double aaa=clock();
			int *vis=arrayManager->get_erfen();//π0
			for(int i=0;i<sz;i++)
				vis[temp[i]]=i+1;
			//szklplex
			vector<int> din(sz);
			vector<int> dout(sz);
			for(int i=0;i<sz;i++)//temp
			{
				int u=temp[i];
				if(g.head[u] !=-1)
				{
					for(int j=g.head[u]; j!= g.next[u]; j++)
					{
						if(g.rm_edge[j]) continue;
						int v=edges[j].to;
						if(vis[v])
						{
							dout[i]++;
							din[vis[v]-1]++;
						}
					}
				}
			}
			//
			din.push_back(-INF);
			din.push_back(INF);
			dout.push_back(-INF);
			dout.push_back(INF);
			sort(din.begin(),din.end());
			sort(dout.begin(),dout.end());
			int l=min(paramK,paramL),r=sz;
			while(l<r)
			{
				int mid=(l+r+1)>>1;
				int cnt_in=sz-get_cnt_lower_than(din,mid-paramL);
				int cnt_out=sz-get_cnt_lower_than(dout,mid-paramK);
				if(min(cnt_in,cnt_out) <mid)	r=mid-1;
				else l=mid;
			}
			sz=r-n;
			int ttt=temp.size();
			for(int i=0;i<ttt;i++)
				vis[ temp[i] ]=0;
			erfen+=clock()-aaa;
		}
	
	#endif
		int ret = n + sz;
		for(int i=0;i<n;i++)
		{
			int v=vertex[i];
			ret += min ( h[v], paramK - n + S.get_out_degree ( v ) ) ;
		}
		for(int i=0;i<n;i++)
		{
			int v=vertex[i];
			h[v]=0;
		}
		return ret;
	}
	int partition_out ( KLplex &S, Graph &g )
	{
		int *h=h_out;//vi∈S,h[s]πi
		vector<int> temp;//:u∈temp,uS
		int n=0;
		int *vertex=arrayManager->get_n_q();
		for(int v:S.vertex)
		{
			vertex[n++]=v;
			h[v]=0;
		}
		for ( int u : U->all() )
		{
			bool found=false;
			for(int i=0;i<n;i++)
			{
				int v=vertex[i];
				if ( g.exist_edge ( v, u ) == false ) //
				{
					h[v]++;
					pos_out[u]=v;
					found=true;
					break;
				}
			}
			if(!found)
			{
				temp.push_back(u);
				pos_out[u]=N;
			}
		}
		int sz=temp.size();
	#ifdef ER_FEN
		// if(sz)
		// {
		// 	double aaa=clock();
		// 	int *vis=arrayManager->get_erfen();//π0
		// 	for(int i=0;i<sz;i++)
		// 		vis[temp[i]]=i+1;
		// 	//szklplex
		// 	vector<int> din(sz);
		// 	vector<int> dout(sz);
		// 	double st=clock();
		// 	for(int i=0;i<sz;i++)//temp
		// 	{
		// 		int u=temp[i];
		// 		if(g.head[u] !=-1)
		// 		{
		// 			for(int j=g.head[u]; j!= g.next[u]; j++)
		// 			{
		// 				if(g.rm_edge[j]) continue;
		// 				int v=edges[j].to;
		// 				if(vis[v])
		// 				{
		// 					dout[i]++;
		// 					din[vis[v]-1]++;
		// 				}
		// 			}
		// 		}
		// 	}
		// 	//
		// 	din.push_back(-INF);
		// 	din.push_back(INF);
		// 	dout.push_back(-INF);
		// 	dout.push_back(INF);
		// 	sort(din.begin(),din.end());
		// 	sort(dout.begin(),dout.end());
		// 	int l=min(paramK,paramL),r=sz;
		// 	while(l<r)
		// 	{
		// 		int mid=(l+r+1)>>1;
		// 		int cnt_in=sz-get_cnt_lower_than(din,mid-paramL);
		// 		int cnt_out=sz-get_cnt_lower_than(dout,mid-paramK);
		// 		if(min(cnt_in,cnt_out) <mid)	r=mid-1;
		// 		else l=mid;
		// 	}
		// 	// rgb[sz-r]++;
		// 	sz=r;
		// 	for(int x:temp)
		// 		vis[x]=0;
		// 	erfen+=clock()-aaa;
		// }
		if(sz)
		{
			double aaa=clock();
			for(int i=0;i<n;i++)
			{
				temp.push_back(vertex[i]);
			}
			sz=temp.size();
			int *vis=arrayManager->get_erfen();//π0
			for(int i=0;i<sz;i++)
				vis[temp[i]]=i+1;
			//szklplex
			vector<int> din(sz);
			vector<int> dout(sz);
			for(int i=0;i<sz;i++)//temp
			{
				int u=temp[i];
				if(g.head[u] !=-1)
				{
					for(int j=g.head[u]; j!= g.next[u]; j++)
					{
						if(g.rm_edge[j]) continue;
						int v=edges[j].to;
						if(vis[v])
						{
							dout[i]++;
							din[vis[v]-1]++;
						}
					}
				}
			}
			for(int i=0;i<sz;i++)
				din[i]=min(din[i]+paramL,dout[i]+paramK);
			//
			din.push_back(-INF);
			din.push_back(INF);
			// dout.push_back(-INF);
			// dout.push_back(INF);
			sort(din.begin(),din.end());
			// sort(dout.begin(),dout.end());
			int l=min(paramK,paramL),r=sz;
			while(l<r)
			{
				int mid=(l+r+1)>>1;
				// int cnt_in=sz-get_cnt_lower_than(din,mid-paramL);
				// int cnt_out=sz-get_cnt_lower_than(dout,mid-paramK);
				// if(min(cnt_in,cnt_out) <mid)	r=mid-1;
				if(sz-get_cnt_lower_than(din,mid) < mid) r=mid-1;
				else l=mid;
			}
			sz=r-n;
			int ttt=temp.size();
			for(int i=0;i<ttt;i++)
				vis[ temp[i] ]=0;
			erfen+=clock()-aaa;
		}
	
	#endif
		int ret = n + sz;
		for(int i=0;i<n;i++)
		{
			int v=vertex[i];
			ret += min ( h[v], paramK - n + S.get_out_degree ( v ) ) ;
		}
		ub_out=ret;
		rm_cnt_out=0;
		return ret;
	}
	int partition_in ( KLplex &S, Graph &g )
	{
		int *h=h_in;//vi∈S,h[s]πi
		vector<int> temp;//π0
		int *vertex=arrayManager->get_n_q();
		int n=0;
		for(int v:S.vertex)
		{
			vertex[n++]=v;
			h[v]=0;
		}
		for ( int u : U->all() )
		{
			bool found=false;
			for (int i=0;i<n;i++)
			{
				int v=vertex[i];
				if ( g.exist_edge ( u, v ) == false ) //
				{
					h[v]++;
					pos_in[u]=v;
					found=true;
					break;
				}
			}
			if(!found)
			{
				temp.push_back(u);
				pos_in[u]=N;
			}
		}
		int sz=temp.size();
	#ifdef ER_FEN	
		// if(sz)
		// {
		// 	double aaa=clock();
		// 	int *vis=arrayManager->get_erfen();
		// 	for(int i=0;i<sz;i++)
		// 		vis[temp[i]]=i+1;
		// 	vector<int> din(sz);
		// 	vector<int> dout(sz);
		// 	int cnt=0;
		// 	for(int i=0;i<sz;i++)//temp
		// 	{
		// 		int u=temp[i];
		// 		if(g.head[u] !=-1)
		// 		{
		// 			for(int j=g.head[u]; j!= g.next[u]; j++)
		// 			{
		// 				cnt++;
		// 				if(g.rm_edge[j]) continue;
		// 				int v=edges[j].to;
		// 				if(vis[v])
		// 				{
		// 					dout[i]++;
		// 					din[vis[v]-1]++;
		// 				}
		// 			}
		// 		}
		// 	}
		// 	din.push_back(-INF);
		// 	din.push_back(INF);
		// 	dout.push_back(-INF);
		// 	dout.push_back(INF);
		// 	sort(din.begin(),din.end());
		// 	sort(dout.begin(),dout.end());
		// 	int l=min(paramK,paramL),r=sz;
		// 	while(l<r)
		// 	{
		// 		int mid=(l+r+1)>>1;
		// 		int cnt_in=sz-get_cnt_lower_than(din,mid-paramL);
		// 		int cnt_out=sz-get_cnt_lower_than(dout,mid-paramK);
		// 		if(min(cnt_in,cnt_out) <mid)	r=mid-1;
		// 		else l=mid;
		// 	}
		// 	// rgb[sz-r]++;
		// 	sz=r;
		// 	int ttt=temp.size();
		// 	for(int i=0;i<ttt;i++)
		// 		vis[temp[i]]=0;
		// 	erfen+=clock()-aaa;
		// }
		if(sz)
		{
			double aaa=clock();
			for(int i=0;i<n;i++)
			{
				temp.push_back(vertex[i]);
			}
			sz=temp.size();
			int *vis=arrayManager->get_erfen();//π0
			for(int i=0;i<sz;i++)
				vis[temp[i]]=i+1;
			//szklplex
			vector<int> din(sz);
			vector<int> dout(sz);
			for(int i=0;i<sz;i++)//temp
			{
				int u=temp[i];
				if(g.head[u] !=-1)
				{
					for(int j=g.head[u]; j!= g.next[u]; j++)
					{
						if(g.rm_edge[j]) continue;
						int v=edges[j].to;
						if(vis[v])
						{
							dout[i]++;
							din[vis[v]-1]++;
						}
					}
				}
			}
			for(int i=0;i<sz;i++)
				din[i]=min(din[i]+paramL,dout[i]+paramK);
			//
			din.push_back(-INF);
			din.push_back(INF);
			// dout.push_back(-INF);
			// dout.push_back(INF);
			sort(din.begin(),din.end());
			// sort(dout.begin(),dout.end());
			int l=min(paramK,paramL),r=sz;
			while(l<r)
			{
				int mid=(l+r+1)>>1;
				// int cnt_in=sz-get_cnt_lower_than(din,mid-paramL);
				// int cnt_out=sz-get_cnt_lower_than(dout,mid-paramK);
				// if(min(cnt_in,cnt_out) <mid)	r=mid-1;
				if(sz-get_cnt_lower_than(din,mid) < mid) r=mid-1;
				else l=mid;
			}
			sz=r- n;
			int ttt=temp.size();
			for(int i=0;i<ttt;i++)
				vis[temp[i]]=0;
			erfen+=clock()-aaa;
		}
	#endif
		int ret = n + sz;
		for ( int i=0;i<n;i++ )
		{
			int v=vertex[i];
			ret += min ( h[v], paramL - n + S.get_in_degree ( v ) );
		}
		ub_in=ret;
		rm_cnt_in=0;
		return ret;
	}
	inline int partition_estimate ( KLplex &S, Graph &g )
	{
		return min ( partition_in ( S, g ), partition_out ( S, g ) );
	}

	int bi_partition(Graph &g)
	{
		int sz=g.vertex_cnt();
		vector<int> din(sz);
		vector<int> dout(sz);
		int i=0;
		for(int v:g.vertex->all())
		{
			din[i]=g.get_in_degree(v);
			dout[i]=g.get_out_degree(v);
			i++;
		}
		din.push_back(-INF);
		din.push_back(INF);
		dout.push_back(-INF);
		dout.push_back(INF);
		sort(din.begin(),din.end());
		sort(dout.begin(),dout.end());
		int l=min(paramK,paramL),r=sz;
		while(l<r)
		{
			int mid=(l+r+1)>>1;
			int cnt_in=sz-get_cnt_lower_than(din,mid-paramL);
			int cnt_out=sz-get_cnt_lower_than(dout,mid-paramK);
			if(min(cnt_in,cnt_out) <mid)	r=mid-1;
			else l=mid;
		}
		return r;
	}
public:
	void init()
	{
		int n=tot_vertex_cnt;
		N=n;
		pos_in=new int[n];
		pos_out=new int[n];
		memset(pos_in,-1,sizeof(int)*n);
		memset(pos_out,-1,sizeof(int)*n);
		rm_cnt_in=0;
		rm_cnt_out=0;
		h_in=new int[n];
		h_out=new int[n];
		memset(h_in,0,sizeof(int)*n);
		memset(h_out,0,sizeof(int)*n);
	}
	inline int first_estimate(Graph &g)
	{
		return bi_partition(g);
	}
	//
	int simple_estimate ( KLplex &S, Graph &g )
	{
		int min_dout = INF; //S
		int min_din = INF; //S
		for ( int u : S.vertex )
		{
			min_dout = min ( min_dout, g.get_out_degree ( u ) );
			min_din = min ( min_din, g.get_in_degree ( u ) );
		}
		return du_ub = min ( min_dout + paramK, min_din + paramL );
	}
	//plexKLplex
	// inline int get_upper_bound ( KLplex &S, Graph &g )
	// {
	// 	if ( !S.vertex_cnt() )
	// 	{
	// 		if ( g.vertex_cnt() > 500 )
	// 			return g.vertex_cnt() ;
	// 		else
	// 			return bi_partition ( g );
	// 	}
	// 	else
	// 	{
	// 		// int a= partition_estimate ( S, g );
	// 		// int b=RGB(S,g);
	// 		// // rgb[b-a]++;
	// 		// rgb[a]++;
	// 		// return min(a,b);

	// 		// return partition_estimate(S,g);
	// 		// return simple_estimate(S,g);
	// 		double st=clock();
	// 		int a=simple_estimate(S,g);
	// 		int b=partition_estimate(S,g);
	// 		// rgb[a-b]++;
	// 		uptime+=clock()-st;
	// 		return min(a,b);
	// 	}
	// }
	inline int getNewUpperBound()
	{
		double st=clock();
		int a=simple_estimate(S,g);
		int b=partition_estimate(S,g);
		uptime+=clock()-st;
		return min(a,b);
	}
	inline int getIncremental(int vertex_remove_from_U)
	{
		int p_in=pos_in[vertex_remove_from_U];
		if(p_in==N)
		{
			rm_cnt_in++;
			if(ub_in - rm_cnt_in <=lb)
			{
				ub_in=partition_in(S,g);
			}
		}else 
		{
			int already=paramL-S.get_non_in_neighbor(p_in,g);
			int &val=h_in[p_in];
			ub_in-=min(already,val);
			val--;
			ub_in+=min(already,val);
		}

		int p_out=pos_out[vertex_remove_from_U];
		if(p_out==N)//π0
		{
			rm_cnt_out++;
			if(ub_out - rm_cnt_out <= lb)
			{
				ub_out=partition_out(S,g);
			}
		}else 
		{
			int already=paramK-S.get_non_out_neighbor(p_out,g);
			int &val=h_out[p_out];
			ub_out-=min(already,val);
			val--;
			ub_out+=min(already,val);
		}
		return min(du_ub,min(ub_in,ub_out));
	}
}estimator;


#endif // UPPERBOUND_H_INCLUDED
