#ifndef MY_QUEUE_H_INCLUDED
#define MY_QUEUE_H_INCLUDED

#include "graph.h"

//
class Queue
{
    bool *in_queue;
    int *q;
    int hh,tt;
public:
    Queue(int n)
    {
        in_queue=new bool[n];
        memset(in_queue,0,sizeof(bool)*n);
        q=new int[n];
        hh=0; tt=-1;
    }
    ~Queue()
    {
        delete[] in_queue;
        delete[] q;
    }
    inline void pop()
    {
        hh++;
    }
    inline int size()
    {
        return tt-hh+1;
    }
    inline void push(int id)
    {
        if(!in_queue[id])
        {
            q[++tt]=id;
            in_queue[id]=1;
        }
    }
    inline void push(int u,int v)
    {
        push(find_edge_id(edges,u,v));
    }
    inline DirectedEdge front()
    {
        return edges[q[hh]];
    }
    // inline bool inqueue(int u,int v)
    // {
    //     return in_queue[find_edge_id(edges,u,v)];
    // }
};

#endif // MY_QUEUE_H_INCLUDED