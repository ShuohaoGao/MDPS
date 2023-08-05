#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "define.h"

const int MAXV=20000;

class Map//map<pii,int> {u,v}={v,u}
{
protected:
	long long maxv;//piifirst<maxv 
public:
    Map(){}
	Map(long long v)
	{
		maxv=v;
	}
	~Map(){}
    virtual long long get_ll(const pii &key)=0;
    virtual int get(const pii &key)=0;
    virtual void put(const pii &key, const int value)=0;
    virtual int increment(const pii &key)=0;//i++
    virtual int reduce(const pii &key)=0;//i--
    virtual int count(const pii &key)=0;
	virtual void clear() =0;
};

class ArrayMap:public Map
{
    int *h;
public:
    ArrayMap(long long v)
    {
        maxv=v;
        int sz=(v+1)*(v+1);
        h=new int[sz];
        memset(h,0,sizeof(int)*sz);
    }
    ~ArrayMap()
    {
        delete[] h;
    }
    long long get_ll(const pii &key)
	{
		if(key.first > key.second)
			return key.second * maxv + key.first;
		return key.first * maxv +key.second;
	}
    int get(const pii &key)
	{
		return h[get_ll(key)];
	}
    void put(const pii &key, const int value)
	{
		h[get_ll(key)]=value;
	}
    int increment(const pii &key)//i++
	{
		return ++h[get_ll(key)];
	}
    int reduce(const pii &key)//i--
	{
		return --h[get_ll(key)];
	}
    int count(const pii &key)
	{
		return h[get_ll(key)];
	}
	void clear()
	{
		int sz=(maxv+1)*(maxv+1);
        memset(h,0,sizeof(int)*sz);
	}
};

class HashMap:public Map 
{
    unordered_map<long long,int> h;
public:
    HashMap(long long v)
	{
		maxv=v;
	}
	~HashMap()
	{
		h.clear();
	}
    long long get_ll(const pii &key)
	{
		if(key.first > key.second)
			return key.second * maxv + key.first;
		return key.first * maxv +key.second;
	}
    int get(const pii &key)
	{
		return h[get_ll(key)];
	}
    void put(const pii &key, const int value)
	{
		h[get_ll(key)]=value;
	}
    int increment(const pii &key)//i++
	{
		return ++h[get_ll(key)];
	}
    int reduce(const pii &key)//i--
	{
		return --h[get_ll(key)];
	}
    int count(const pii &key)
	{
		return h.count(get_ll(key));
	}
	void clear()
	{
		h.clear();
	}
};

class PIIMap
{
	long long maxv;
	unordered_map<long long,int> h;
public:
    PIIMap(long long v)
	{
		maxv=v;
	}
	~PIIMap()
	{
		h.clear();
	}
    inline long long get_ll(const pii &key)
	{
		return key.first * maxv +key.second;
	}
    inline int get(const pii &key)
	{
		return h[get_ll(key)];
	}
    inline void put(const pii &key, const int value)
	{
		h[get_ll(key)]=value;
	}
    inline int increment(const pii &key)//i++
	{
		return ++h[get_ll(key)];
	}
    inline int reduce(const pii &key)//i--
	{
		return --h[get_ll(key)];
	}
    inline int count(const pii &key)
	{
		return h.count(get_ll(key));
	}
};

//v
inline  Map* get_suitable_map(long long v)
{
    if(v<MAXV)  return new ArrayMap(v);
    return new HashMap(v);
}

#endif // MAP_H_INCLUDED