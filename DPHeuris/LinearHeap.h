#ifndef LINEARHEAP_H
#define LINEARHEAP_H

#include "Utility.h"

// used for degeneracy order
class LinearHeap
{
    struct Node
    {
        ui pre,ne;
    };
    ui *h;
    Node *nodes;
public:
    ui max_range;
    ui min_key;
    ui sz, n;//sz will decrease , but n won't
    LinearHeap(ui _range, ui _n, ui pd[])
    {
        max_range=_range;
        h=new ui[max_range];
        n=_n;
        sz=0;
        for(ui i=0;i<max_range;i++)
            h[i]=n;
        nodes=new Node[n];
        for(ui i=0;i<n;i++)
            nodes[i]={n,n};
        min_key=max_range;
        for(ui i=0;i<n;i++)
            insert(pd[i], i);
    }
    ~LinearHeap()
    {
        delete[] h;
        delete[] nodes;
    }
    void insert(ui key, ui val)
    {
        min_key=min(min_key,key);
        sz++;
        nodes[val].pre=key+n;
        nodes[val].ne=h[key];
        if(h[key]<n)
            nodes[h[key]].pre=val;
        h[key]=val;
    }
    ui get_min_key()
    {
        while(h[min_key]>=n) min_key++;
        return min_key;
    }
    void delete_node(ui id)
    {
        sz--;
        auto &cur=nodes[id];
        if(cur.pre >= n)//key = cur.pre-n
            h[cur.pre-n]=cur.ne;
        else 
            nodes[cur.pre].ne=cur.ne;
        if(cur.ne < n)
            nodes[cur.ne].pre=cur.pre;
    }
    void decrease(ui key_now, ui id)
    {
        delete_node(id);
        insert(key_now, id);
    }
    ui get_min_node()
    {
        return h[get_min_key()];
    }
};

#endif