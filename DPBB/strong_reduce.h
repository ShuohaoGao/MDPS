#ifndef STRONG_REDUCE_H_INCLUDED
#define STRONG_REDUCE_H_INCLUDED

#include "graph.h"
#include "map.h"
#include "my_queue.h"

class Strong_reduce 
{
private:
    Map *common_out_neighbor_cnt;
    Map *common_in_neighbor_cnt;
    vector<pii> undirected_edges;
    Graph *G;
    bool *rm;//
    //,O(1)
    int n;
    int m;
    int *p;
    int *sz;
    int find(int x)
    {
        return p[x] == x ? x : p[x]=find(p[x]);
    }
    //+
    inline void merge(int a,int b)
    {
        if(sz[a] > sz[b])
            swap(a,b);
        p[a]=b;
        sz[b]+=sz[a];
    }
    //
    inline void init_set()
    {
        for(int i=0;i<n;i++)
            p[i]=i;
        for(int i=0;i<n;i++)
            sz[i]=1;
    }
    //
    int *h;
    int *e;
    int *ne;
    int idx;
    int *fa;
    inline void init_tree()
    {
        idx=0;
        memset(h,-1,sizeof(int)*n);
        memset(fa,-1,sizeof(int)*n);
    }
    //dfs
    void dfs(int u,int father)
    {
        fa[u]=father;
        for(int i=h[u];~i;i=ne[i])
        {
            int j=e[i];
            if(j==father)  continue;
            dfs(j,u);
        }
    }
    inline void build_tree()
    {
        for(int i=0;i<n;i++)
            if( fa[i]==-1)
            {
                dfs(i,-1);
            }
    }
    inline void add_tree_edge(int a,int b)
    {
        e[idx]=b,ne[idx]=h[a],h[a]=idx++;
        e[idx]=a,ne[idx]=h[b],h[b]=idx++;
    }

    //x
    inline bool has(const vector<pii> &s,int sz, pii& x)
    {
        auto pos=lower_bound(s.begin(),s.begin()+sz,x);
        if(pos == s.begin()+sz)	return false;
        return (*pos)==x;
    }
    
    inline void process_triangle(int a,int b,int c)
    {
        bool ab=G->exist_edge(a,b);
        bool ba=G->exist_edge(b,a);
        bool ac=G->exist_edge(a,c);
        bool ca=G->exist_edge(c,a);
        bool bc=G->exist_edge(b,c);
        bool cb=G->exist_edge(c,b);
        bool ok=(ab || ba) && (ac||ca) && (bc||cb);
        if(!ok)
        {
            printf(" %d %d %d\n",a,b,c);
            return;
        }
        //a b
        if(ac && bc) common_out_neighbor_cnt->increment({a,b});
        if(ca && cb) common_in_neighbor_cnt->increment({a,b});
        //a c
        if(ab && cb) common_out_neighbor_cnt->increment({a,c});
        if(ba && bc) common_in_neighbor_cnt->increment({a,c});
        //b c
        if(ca && ba) common_out_neighbor_cnt->increment({c,b});
        if(ab && ac) common_in_neighbor_cnt->increment({c,b});
    }
   
    void list_triangles()
    {
        memset(rm,false,sizeof(bool)*undirected_edges.size());
        init_set();
        while(m>=3)//2√m
        {
            init_tree();
            for(int i=0;i<m;i++)//kruskal
            {
                int a=undirected_edges[i].first;
                int b=undirected_edges[i].second;
                int pa=find(a), pb=find(b);
                if(pa != pb)
                {
                    rm[i]=true;
                    merge(pa,pb);
                    add_tree_edge(a,b);
                }
            }
            
            //
            int l=0,r=m-1;
            while(l<r)
            {
                while(!rm[l]) l++;
                while(r && rm[r]) r--;
                if(l<r)
                {
                    swap(rm[l],rm[r]);
                    swap(undirected_edges[l],undirected_edges[r]);
                    l++; r--;
                }
            }
            build_tree();
            sort(undirected_edges.begin(),undirected_edges.begin()+r+1);
            
            for(int i=0;i<=r;i++)//
            {
                int u=undirected_edges[i].first, v=undirected_edges[i].second;
                if(~fa[u])
                {
                    int a=fa[u],b=v;
                    if(a>b) swap(a,b);
                    pii temp={a,b};
                    if(fa[a]==b || fa[b]==a || has(undirected_edges,r+1,temp))
                    {
                        process_triangle(u,v,fa[u]);
                    }else if(~fa[v])
                    {
                        a=fa[v],b=u;
                        if(a>b) swap(a,b);
                        temp={a,b};
                        if(fa[a]==b || fa[b]==a || has(undirected_edges,r+1,temp))
                        {
                            process_triangle(u,v,fa[v]);
                        }
                    }
                }
            }
            
            for(int i=r+1;i<m;i++)//
            {
                int a=undirected_edges[i].first;
                int b=undirected_edges[i].second;
                p[a]=a; p[b]=b;
                sz[a]=1; sz[b]=1;
            }
            m=r+1;
        }
    }

    void init()
    {
        undirected_edges.clear();
        //
        for(int u:G->vertex->all())
        {
            for(int v:G->neighbor_out(u))
            {
                if(u<v)
                    undirected_edges.push_back({u,v});
                else 
                    undirected_edges.push_back({v,u});
            }
        }
        //+
        unique_vector(undirected_edges);
        m=undirected_edges.size();
    }
public:
    Strong_reduce(Graph *_G)
    {
        G=_G;
        n=tot_vertex_cnt;
        common_in_neighbor_cnt=get_suitable_map(n);
        common_out_neighbor_cnt=get_suitable_map(n);
        p=new int[n];
        sz=new int[n];
        h=new int[n];
        fa=new int[n];
        init();
        rm=new bool[m];
        e=new int[n<<1];//n-1;x2
        ne=new int[n<<1];
    }
    ~Strong_reduce()
    {
        delete common_in_neighbor_cnt;
        delete common_out_neighbor_cnt;
        delete[] p;
        delete[] sz;
        delete[] rm;
        delete[] h;
        delete[] fa;
        delete[] e;
        delete[] ne;
    }
    void strong_reduce(bool first_reduce=false)
    {
        clock_t start_time=clock();

        list_triangles();
		Queue *q_e=new Queue(edges.size());//
        queue<int> q_v;//
        for(int i=0;i<edges.size();i++)
        {
            int u=edges[i].from;
            int v=edges[i].to;
            if(G->exist_edge(v,u))
            {
                if(common_out_neighbor_cnt->get({u,v}) <= lb-2*paramK || common_in_neighbor_cnt->get({u,v}) <= lb-2*paramL)
                {
                    q_e->push(u,v);
                }
            }else 
            {
                if(common_out_neighbor_cnt->get({u,v}) <= lb-2*paramK+1 || common_in_neighbor_cnt->get({u,v}) <= lb-2*paramL+1)
                {
                    q_e->push(u,v);
                }
            }
        }
        if(first_reduce)
        {
            while(q_e->size())
            {
                auto f=q_e->front(); q_e->pop();
                G->remove_edge_forever(f.id);
            }
            init();
            common_in_neighbor_cnt->clear();
            common_out_neighbor_cnt->clear();
            list_triangles();
            for(int i=0;i<edges.size();i++)
            {
                if(edge_remove[i]) continue;
                int u=edges[i].from;
                int v=edges[i].to;
                if(G->exist_edge(v,u))
                {
                    if(common_out_neighbor_cnt->get({u,v}) <= lb-2*paramK || common_in_neighbor_cnt->get({u,v}) <= lb-2*paramL)
                    {
                        q_e->push(u,v);
                    }
                }else 
                {
                    if(common_out_neighbor_cnt->get({u,v}) <= lb-2*paramK+1 || common_in_neighbor_cnt->get({u,v}) <= lb-2*paramL+1)
                    {
                        q_e->push(u,v);
                    }
                }
            }
            for(int i=0;i<tot_vertex_cnt;i++)
                if(!v_remove[i] && (G->get_in_degree(i)<=lb-paramL || G->get_out_degree(i)<=lb-paramK))
                {
                    q_v.push(i);
                    v_remove[i]=1;
                }
        }
        
        while ( q_v.size() || q_e->size() )
		{
			while ( q_e->size() )//
			{
				DirectedEdge edge = q_e->front();
				q_e->pop();
				int from = edge.from, to = edge.to;
				if ( !G->remove_edge_forever (edge.id) ) //false
				{
                    continue;
                }
				if ( !G->satisfied ( from ) && !v_remove[from] )
				{
					q_v.push ( from );
					v_remove[from] = 1;
				}
				if ( !G->satisfied ( to ) && !v_remove[to] )
				{
					q_v.push ( to );
					v_remove[to] = 1;
				}
				//,
				for ( int w : G->neighbor_out(from) )
				{
                    bool w_to=G->exist_edge(w,to);
                    bool to_w=G->exist_edge(to,w);
					if (v_remove[w] ||  !(w_to || to_w) )
						continue;
					bool from_w=true;
                    bool w_from=G->exist_edge(w,from);
                    int val_now=common_in_neighbor_cnt->reduce({w,to});
                    if ( w_to && to_w )
					{
						if (val_now <= lb - 2 * paramL )
						{
                            q_e->push(to,w);
							q_e->push (w,to);
						}
					}
					else 
					{
                        if ( val_now<= lb - 2 * paramL + 1 )
                        {
                            if(w_to)
                            {
                                q_e->push ( w, to );
                            }
                            else 
                            {
                                q_e->push(to,w);
                            }
                        }
					}
                    if(w_to)
                    {
                        int val=common_out_neighbor_cnt->reduce({from, w});
                        if ( w_from )
                        {
                            if ( val <= lb - 2 * paramK )
                            {
                                q_e->push ( from, w );
                                q_e->push ( w, from );
                            }
                        }
                        else
                        {
                            if (val<= lb - 2 * paramK + 1 )
                            {
                                q_e->push ( from, w );
                            }
                        }
                    }
				}
				for ( int w : G->neighbor_in(from) )
				{
					if ( v_remove[w] || !G->exist_edge ( w, to ) )
						continue;
					if ( G->exist_edge ( from, w ) )
						continue;//,
					int val=common_out_neighbor_cnt->reduce({from, w});
					if (val<= lb - 2 * paramK + 1 )
					{
						q_e->push ( w, from );
					}
				}
            }
            
            if ( q_v.size() )//
			{
				int u = q_v.front();
				q_v.pop();
                
				for ( int to : G->neighbor_out(u) ) //
				{
					if ( G->get_in_degree ( to ) - 1 <= lb - paramL && !v_remove[to] )
					{
						q_v.push ( to );
						v_remove[to] = 1;
					}
				}
                
				for ( int from : G->neighbor_in(u) ) //
				{
					if ( G->get_out_degree ( from ) - 1 <= lb - paramK && !v_remove[from] )
					{
						q_v.push ( from );
						v_remove[from] = 1;
					}
				}
                // for(int v:G->neighbor_out(u))
                // {
                //     for(int w:G->neighbor_out(u))
                //     {
                //         if(w==v) break;
                //         bool vw=G->exist_edge(v,w);
                //         bool wv=G->exist_edge(w,v);
                //         if(vw || wv)
                //         {
                //             int val=common_in_neighbor_cnt->reduce({v,w});
                //             if(wv && vw)
                //             {
                //                 if(val <= lb-2*paramL)
                //                 {
                //                     q_e->push(w,v);
                //                     q_e->push(v,w);
                //                 }
                //             }else if(wv)
                //             {
                //                 if(val <= lb-2*paramL+1)
                //                     q_e->push(w,v);
                //             }else 
                //             {
                //                 if(val <= lb-2*paramL+1)
                //                     q_e->push(v,w);
                //             }
                //         }
                //     }
                // }
                // for(int v:G->neighbor_in(u))
                // {
                //     for(int w:G->neighbor_in(u))
                //     {
                //         if(w==v) break;
                //         bool vw=G->exist_edge(v,w);
                //         bool wv=G->exist_edge(w,v);
                //         if(vw || wv)
                //         {
                //             int val=common_out_neighbor_cnt->reduce({v,w});
                //             if(wv && vw)
                //             {
                //                 if(val <= lb-2*paramK)
                //                 {
                //                     q_e->push(w,v);
                //                     q_e->push(v,w);
                //                 }
                //             }else if(wv)
                //             {
                //                 if(val <= lb-2*paramK+1)
                //                     q_e->push(w,v);
                //             }else 
                //             {
                //                 if(val <= lb-2*paramK+1)
                //                     q_e->push(v,w);
                //             }
                //         }
                //     }
                // }
                G->remove_vertex_forever ( u );
			}
            
        }
        delete q_e;

        strong_reduce_time+=clock()-start_time;
    }
};

#endif // STRONG_REDUCE_H_INCLUDED