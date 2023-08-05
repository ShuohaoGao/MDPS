#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

#include "fast_set.h"
#include "array_manager.h"
#include<set>
#include<map>
#include<cmath>
#include<queue>
#include<ctime>
#include<cstdio>
#include<vector>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<unordered_map>
#include<unordered_set>

#define S plex

#define NOT_FINISHED 1
#define ER_FEN 1
#define MAX_K 5

using namespace std;
const int INF = 0x3f3f3f3f; 
typedef pair<int, int> pii;

map<int,int> rgb;

int read() 
{
	int ret = 0;
	char ch;
	while ( ( ch = getchar() ) > '9' || ch < '0' );
	while ( ch <= '9' && ch >= '0' )
	{
		ret = ret * 10 + ch - '0';
		ch = getchar();
	}
	return ret;
}

string operator+(string &content, int number) {
	string temp = "";
	char t = 0;
	while (true) {
        t = number % 10 + '0';
        temp = t + temp;
        number /= 10;
        if (number == 0) {
            return content + temp;
        }
	}
}
string&  operator+=(string &content, int number) {
	return content = content + number;
}

//=============================================
//global var
//=============================================
ArrayManager* arrayManager;
double erfen;
clock_t start_time;
double jump;
double Srmv;
double Saddv;
double grmv;
double grb;
double uptime;
double proneTime;
int paramK;
int paramL;
int lb;//lower_bound
int last_lb=INF;
set<int> max_KLplex;
ArraySet *U;
// SegTree *U;
bool *v_remove;
bool *edge_remove;

int tot_vertex_cnt;

struct DirectedEdge 
{
	int id;
	int from;
	int to;
	bool operator < (const DirectedEdge &E)const 
	{
		return from<E.from || (from==E.from && to<E.to);
	}
	bool operator == (const DirectedEdge &E)const 
	{
		return id==E.id;
	}
};
vector<DirectedEdge> edges;
bool cmp_edge(const DirectedEdge &a,const DirectedEdge &b)
{
	return edge_remove[a.id] < edge_remove[b.id] || (edge_remove[a.id] == edge_remove[b.id] && a<b);
}

inline void remove_from_S(int v);
inline void add_to_S(int v);

void print_set ( set<int> &max_KLplex )
{
	if ( max_KLplex.size() == 0 )
	{
		puts ( "" );
		return;
	}
	auto it = max_KLplex.begin();
	printf ( "%d", *it );
	it++;
	for ( ; it != max_KLplex.end(); it++ )
		printf ( ",%d", *it );
	puts ( "" );
}

inline void print_ans()
{
	printf ( "ground truth= %d:\n", lb );
	print_set ( max_KLplex );
}

inline void unique_vector ( vector<int> &v )
{
	sort ( v.begin(), v.end() );
	v.erase ( unique ( v.begin(), v.end() ), v.end() );
}

inline void unique_vector ( vector<pii> &v )
{
	sort ( v.begin(), v.end() );
	v.erase ( unique ( v.begin(), v.end() ), v.end() );
}
inline int re_map(const vector<int> &s,int x)
{
	return lower_bound(s.begin(),s.end(),x)-s.begin();
}

void print_time ( clock_t st, clock_t ed )
{
	printf ( "use time %.4lf s\n\n", ( double ) ( ed - st ) / CLOCKS_PER_SEC );
	fflush(stdout);
}

inline void copy_set ( set<int> &src, set<int> &dst )
{
	for ( int v : src )
		dst.insert ( v );
}

inline void Sub ( set<int> &a, set<int> &b )
{
	for ( int v : b )
		a.erase ( v );
}

// inline void Sub(SegTree *a,set<int> &b)
// {
// 	for(int v:b)
// 		a->erase(v);
// }

inline void Sub ( ArraySet *a, set<int> &b )
{
	for ( int v : b )
		a->erase ( v );
}

inline void copy(set<int> &dst,const vector<int> &src)
{
	for(int v:src)
		dst.insert(v);
}

inline void copy(set<int> &dst,set<int> &src)
{
	for(int v:src)
		dst.insert(v);
}

inline bool has(const vector<int> &s, int x)
{
	auto pos=lower_bound(s.begin(),s.end(),x);
	if(pos == s.end())	return false;
	return (*pos)==x;
}

string get_file_name(char *path)
{
	int len=strlen(path);
	char *str=path+len-1;
	while(str != path )
	{
		char ch=*str;
		if(ch=='\\' || ch=='/')
			break;
		str--;
	}
	string ret(str+1);
	while(ret.size() && ret[ret.size()-1]!='.')
		ret.pop_back();
	if(ret.size())
		ret.pop_back();
	return ret;
}

inline int get_cnt_lower_than(const vector<int> &s,int x)
{
	int l=0,r=s.size()-1;
	while(l<r)
	{
		int mid=(l+r+1)>>1;
		if(s[mid]>=x)	r=mid-1;
		else l=mid;
	}
	return l;
}

inline int find(const vector<int> &s, int x)
{
	return lower_bound(s.begin(),s.end(),x)-s.begin();
}

inline int find_edge_id(const vector<DirectedEdge> &s, int a,int b)
{
	DirectedEdge x={0,a,b};
	return lower_bound(s.begin(),s.end(),x)-s.begin();
}

void read_from_file ( const char *path ,bool pre=false)
{
	if ( freopen ( path, "r", stdin ) == NULL ) 
	{
		printf ( "!\n" );
		exit ( 1 );
	}
	int edge_cnt;
	if(!pre)
		cin>>tot_vertex_cnt>>edge_cnt>>lb;
	else cin>>tot_vertex_cnt>>edge_cnt;
	edges.resize ( edge_cnt );
	v_remove = new bool[tot_vertex_cnt];
	edge_remove = new bool[edge_cnt];
	for ( int i = 0; i < tot_vertex_cnt; i++ )
	{
		v_remove[i] = false;
	}
	for ( int i = 0; i < edge_cnt; i++ )
	{
		edge_remove[i] = false;
		int from = read();
		int to = read();
		edges[i] =  {i,from, to} ;
	}
}
clock_t strong_reduce_time = 0;

#endif // DEFINE_H_INCLUDED
