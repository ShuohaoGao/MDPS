#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "define.h"
#include "fast_set.h"

class Graph
{
private:
	int n;
	int m;
	
public:
	int *din;
	int *dout;
	int *head;
	int *next;
	vector<DirectedEdge> reverse_edges;
	int *reverse_head;

	bool *rm_edge;
	bool *rm_vertex;
	ArraySet* vertex;

	void copy(Graph &G)
	{
		n=tot_vertex_cnt;
		m=edges.size();
		din=new int[n];
		dout=new int[n];
		head=new int[n];
		next=new int[n];
		reverse_head=new int[n];
		rm_edge=new bool[m];
		rm_vertex=new bool[n];
		memcpy(din,G.din,n*sizeof(int));
		memcpy(dout,G.dout,n*sizeof(int));
		memcpy(head,G.head,n*sizeof(int));
		memcpy(next,G.next,n*sizeof(int));
		memcpy(reverse_head,G.reverse_head,n*sizeof(int));
		memcpy(rm_edge,G.rm_edge,m*sizeof(bool));
		memcpy(rm_vertex, G.rm_vertex,n*sizeof(bool));
		for(const DirectedEdge &e:G.reverse_edges)
			reverse_edges.push_back(e);
		vertex=new ArraySet(G.vertex);
		n=vertex->size();
		m=G.edge_cnt();
	}
	void clear()
	{
		delete[] din;
		delete[] dout;
		delete[] head;
		delete[] next;
		delete[] reverse_head;
		delete[] rm_edge;
		delete[] rm_vertex;
		reverse_edges.clear();
		delete vertex;
	}
	
	void init ( int tot_vertex_cnt, const vector<DirectedEdge> &edges )
	{
		din=new int[tot_vertex_cnt];
		dout=new int[tot_vertex_cnt];
		head=new int[tot_vertex_cnt];
		next=new int[tot_vertex_cnt];
		reverse_head=new int[tot_vertex_cnt];
		rm_vertex=new bool[tot_vertex_cnt];
		rm_edge=new bool[edges.size()];
		memset(din,0,sizeof(int)*tot_vertex_cnt);
		memset(dout,0,sizeof(int)*tot_vertex_cnt);
		memset(head,-1,sizeof(int)*tot_vertex_cnt);
		memset(next,-1,sizeof(int)*tot_vertex_cnt);
		memset(reverse_head,-1,sizeof(int)*tot_vertex_cnt);
		memset(rm_vertex,0,sizeof(bool)*tot_vertex_cnt);
		memset(rm_edge,0,sizeof(bool)*edges.size());
		reverse_edges.resize(edges.size());
		n = 0;
		m = 0;
		vertex=new ArraySet(tot_vertex_cnt);
		for ( int i = 0; i < tot_vertex_cnt; i++ )
		{
			if ( v_remove[i] )
			{
				rm_vertex[i]=true;
				continue;
			}
			vertex->insert ( i );
		}
		n = vertex->size();

		for ( int i=0;i<(int)edges.size();i++ )
		{
			int edge_id = i;
			int from = edges[i].from;
			int to = edges[i].to;
			if ( v_remove[from] || v_remove[to] )
			{
				edge_remove[edge_id] = true;
				rm_edge[edge_id]=true;
				continue;
			}
			if(edge_remove[edge_id])
			{
				rm_edge[edge_id]=true;
				continue;
			}
			if(head[from]==-1)
				head[from]=edge_id;
			next[from]=i+1;
			din[to]++; 
			dout[from]++;
			reverse_edges[m++]={i,to,from};
		}
		reverse_edges.erase(reverse_edges.begin()+m,reverse_edges.end());
		sort(reverse_edges.begin(),reverse_edges.end());
		for ( int i=0;i<m;i++ )
		{
			int edge_id = reverse_edges[i].id;
			int from = reverse_edges[i].from;
			int to = reverse_edges[i].to;
			if(reverse_head[from]==-1)
				reverse_head[from]=i;
		}
	}
	inline int vertex_cnt()
	{

#ifdef NOT_FINISHED
		if ( n != ( int ) vertex->size() )
		{
			printf ( "!!!G.vertex_cnt not right: n=%d vertex->sz=%d\n", n, ( int ) vertex->size() );
			n = vertex->size();
		}
#endif // NOT_FINISHED

		return n;
	}
	inline int edge_cnt()
	{
		return m;
	}

	inline bool exist_edge ( int a, int b ) 
	{
		if(head[a]==-1)	return false;
		int l=head[a],r=next[a]-1;
		if(edges[r].to < b)
			return false;
		while(l<r)
		{
			int mid=(l+r)>>1;
			if(edges[mid].to>=b)
				r=mid;
			else l=mid+1;
		}
		return edges[l].to == b && !rm_edge[l];
	}
	inline int get_edge_id(int a,int b)
	{
		if(head[a]==-1)	return -1;
		int l=head[a],r=next[a]-1;
		if(edges[r].to < b)
			return -1;
		while(l<r)
		{
			int mid=(l+r)>>1;
			if(edges[mid].to>=b)
				r=mid;
			else l=mid+1;
		}
		bool has_edge= (edges[l].to == b) && !rm_edge[l];
		if(!has_edge)	return -1;
		return l;
	}
	vector<int> neighbor_out(int u)
	{
		vector<int> ret;
		if(head[u]!=-1)
		{
			for(int i=head[u];i!=next[u];i++)
			{
				if(rm_edge[i])	continue;
				ret.push_back(edges[i].to);
			}
		}
		return ret;
	}
	vector<int> neighbor_in(int v)
	{
		vector<int> ret;
		if(reverse_head[v]!=-1)
		{
			for(int p=reverse_head[v];p<(int)reverse_edges.size() && reverse_edges[p].from==v;p++)
			{
				int id=reverse_edges[p].id;
				if(rm_edge[ id ])	continue;
				ret.push_back(reverse_edges[p].to);
			}
		}
		return ret;
	}
	vector<int> edges_out(int u)
	{
		vector<int> ret;
		if(head[u]!=-1)
		{
			for(int i=head[u];i!=next[u];i++)
			{
				if(rm_edge[i])	continue;
				ret.push_back(i);
			}
		}
		return ret;
	}
	vector<int> edges_in(int v)
	{
		vector<int> ret;
		if(reverse_head[v]!=-1)
		{
			for(int p=reverse_head[v];p<(int)reverse_edges.size() && reverse_edges[p].from==v;p++)
			{
				int id=reverse_edges[p].id;
				if(rm_edge[ id ])	continue;
				ret.push_back(id);
			}
		}
		return ret;
	}
	inline int get_in_degree ( int v )
	{
		return din[v];
	}
	inline int get_out_degree ( int v )
	{
		return dout[v];
	}
	inline bool k_satisfied ( int u, int lb )
	{
		return dout[u] >= lb - paramK;
	}
	inline bool l_satisfied ( int u, int lb )
	{
		return din[u] >= lb - paramL;
	}
	inline bool satisfied ( int u ) 
	{
		return k_satisfied ( u, lb + 1 ) && l_satisfied ( u, lb + 1 );
	}
	void output_to_file(string path)
	{
		FILE *file;
		if((file=fopen(path.c_str(),"w"))==NULL)
		{
			printf("%s open file failed\n",path.c_str());
			exit(1);
		}
		vector<int> v;
		vector<pii> e;
		
		for ( const DirectedEdge &edge : edges )
		{
			int edge_id = edge.id;
			int from = edge.from;
			int to = edge.to;
			if ( v_remove[from] || v_remove[to] || edge_remove[edge_id])
			{
				continue;
			}
			v.push_back(from);
			v.push_back(to);
			e.push_back({from,to});
			
		}
		unique_vector(v);
		unique_vector(e);
		if((int)v.size()== lb)
		{
			fprintf(file,"0 0 %d\n",lb);
			fflush(file);
			fclose(file);
			vertex->clear();
			n=0;
			m=0;
			printf("reduced graph has been written to %s \n",path.c_str());
			return;
		}
		fprintf(file,"%d %d %d\n",(int)v.size(),(int)e.size(),lb);
		for(const pii &edge:e)
			fprintf(file,"%d %d\n",re_map(v,edge.first),re_map(v,edge.second));
		fflush(file);
		fclose(file);
		printf("reduced graph has been written to %s \n",path.c_str());
		fflush(stdout);
	}

	vector<int> remove_vertex ( const vector<int> &V )
	{
		clock_t st=clock();
		n -= V.size();
		vector<int> ret;
		for ( int v : V )
		{
#ifdef NOT_FINISHED
			if ( !has ( v ) )
			{
				n++;
				continue;
			}
#endif //NOT_FINISHED
			vertex->erase ( v );
			U->erase ( v );
			rm_vertex[v]=true;
			if(head[v]!=-1)
			{
				for(int i=head[v];i!=next[v];i++)
				{
					if(rm_edge[i])	continue;
					rm_edge[i]=true;
					dout[v]--;
					din[edges[i].to]--;
					ret.push_back(i);
				}
			}
			if(reverse_head[v]!=-1)
			{
				for(int p=reverse_head[v];p<(int)reverse_edges.size() && reverse_edges[p].from==v;p++)
				{
					if(rm_edge[ reverse_edges[p].id ])
						continue;
					rm_edge[ reverse_edges[p].id ]=true;
					din[v]--;
					dout[ reverse_edges[p].to ]--;
					ret.push_back(reverse_edges[p].id);
				}
			}
		}
		m -= ret.size();

		grmv+=clock()-st;
		return ret;
	}
	void remove_vertex_forever ( int v )
	{
		v_remove[v]=true;
#ifdef NOT_FINISHED
		if ( !has ( v ) )
		{
			return;
		}
#endif //NOT_FINISHED
		vertex->erase ( v );
		U->erase ( v );
		if(!rm_vertex[v])
		{
			n--;
			rm_vertex[v]=true;
		}
		if(head[v]!=-1)
		{
			for(int i=head[v];i!=next[v];i++)
			{
				edge_remove[i]=true;
				if(rm_edge[i])	continue;
				rm_edge[i]=true;
				m--;
				dout[v]--;
				din[edges[i].to]--;
			}
		}
		if(reverse_head[v]!=-1)
		{
			for(int p=reverse_head[v];p<(int)reverse_edges.size() && reverse_edges[p].from==v;p++)
			{
				int id=reverse_edges[p].id;
				edge_remove[id]=true;
				if(rm_edge[ id ])	continue;
				rm_edge[ id ]=true;
				m--;
				din[v]--;
				dout[ reverse_edges[p].to ]--;
			}
		}
	}
	void rollback ( const vector<int> &V, const vector<int> &E )
	{
		double st=clock();
		for ( int v : V )
		{
			if(v_remove[v])
				continue;
			rm_vertex[v]=false;
			vertex->insert ( v );
			n++;
		}
		for ( int edge_id : E )
		{
			int from = edges[edge_id].from, to = edges[edge_id].to;
			if ( !has ( from ) || !has ( to ) )
				continue;
			if ( edge_remove[edge_id] )
				continue;
			if(!rm_edge[edge_id])
				continue;
			rm_edge[edge_id]=false;
			m++;
			din[to]++;
			dout[from]++;
		}
		for(int v:V)
			if(!v_remove[v])
			U->insert(v);
		grb+=clock()-st;
	}
	bool remove_edge_forever ( int a, int b )
	{
		if(head[a]==-1)	return false;
		int l=head[a],r=next[a]-1;
		if(edges[r].to < b)	return false;
		while(l<r)
		{
			int mid=(l+r)>>1;
			if(edges[mid].to>=b)	r=mid;
			else l=mid+1;
		}
		if(edges[l].to != b)	return false;
		int &edge_id = l;
		if ( edge_remove[edge_id] )	return false;
		if ( !has ( a) || !has ( b ) )
		{
			exit ( 1 );
		}
		if(!rm_edge[edge_id])
		{
			m--;
			rm_edge[edge_id]=true;
			dout[a]--;
			din[b]--;
		}
		edge_remove[edge_id] = true;
		return true;
	}
	bool remove_edge_forever ( int edge_id )
	{
		if ( edge_remove[edge_id] )	return false;
		int a=edges[edge_id].from, b= edges[edge_id].to;
		if ( !has ( a) || !has ( b ) )
		{
			exit ( 1 );
		}
		if(!rm_edge[edge_id])
		{
			m--;
			rm_edge[edge_id]=true;
			dout[a]--;
			din[b]--;
		}
		edge_remove[edge_id] = true;
		return true;
	}
	bool is_klplex ()
	{
		for ( int v : vertex->all() )
		{
			if ( din[v]< n-paramL || dout[v] < n-paramK )
				return false;
		}
		return true;
	}
	inline bool has ( int v )
	{
		return !rm_vertex[v];
	}
};

Graph full_g;
Graph g;

#endif // GRAPH_H_INCLUDED
