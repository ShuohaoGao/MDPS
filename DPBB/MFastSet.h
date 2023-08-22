#ifndef FAST_LIST_H
#define FAST_LIST_H

#include "../DPHeuris/Utility.h"

#define MASK 63
#define SHIFT_BIT 6
/**
 * A set based on bit operations
 * insert/remove is O(1)
 */
class MFastSet
{
    uint64_t *s;
    int m;          // m+1 = length of s
    int r;          // the last number of s
    int n;          // max size of the set
    int sz;         // current size
    vector<ui> all; // current elements in the set
    bool changed;   // whether we need to update all

public:
    MFastSet()
    {
        s = nullptr;
        m = n = sz = 0;
        changed = false;
    }
    MFastSet(int _n)
    {
        n = _n;
        m = n / MASK;
        r = n & MASK;
        sz = 0;
        changed = false;
        s = new uint64_t[m + 1];
        memset(s, 0, sizeof(uint64_t) * (m + 1));
    }
    ~MFastSet()
    {
        if(s!=nullptr)
            delete[] s;
    }
    void init(int _n)
    {
        n = _n;
        m = n / MASK;
        r = n & MASK;
        sz = 0;
        changed = false;
        s = new uint64_t[m + 1];
        memset(s, 0, sizeof(uint64_t) * (m + 1));
    }
    bool has(int u)
    {
        return s[u >> SHIFT_BIT] & (1ull << (u & MASK));
    }
    void remove(int u)
    {
        assert(has(u));
        double start_erase_time = get_system_time_microsecond();
        changed = true;
        s[u >> SHIFT_BIT] ^= 1ull << (u & MASK);
        sz--;
        assert(!has(u));
        erase_time += get_system_time_microsecond() - start_erase_time;
    }
    void insert(int u)
    {
        assert(!has(u));
        double start_insert_time = get_system_time_microsecond();
        sz++;
        if (!changed)
            all.push_back(u);
        s[u >> SHIFT_BIT] |= 1ull << (u & MASK);
        assert(has(u));
        insert_time += get_system_time_microsecond() - start_insert_time;
    }
    void clear()
    {
        double start_clear_time = get_system_time_microsecond();
        sz = 0;
        changed = false;
        memset(s, 0, sizeof(uint64_t) * (m + 1));
        all.clear();
        clear_time += get_system_time_microsecond() - start_clear_time;
    }
    void erase(int u) // renamed 'remove'
    {
        assert(has(u));
        double start_erase_time = get_system_time_microsecond();
        changed = true;
        s[u >> SHIFT_BIT] ^= 1ull << (u & MASK);
        sz--;
        erase_time += get_system_time_microsecond() - start_erase_time;
    }
    bool count(int u) // renamed 'has'
    {
        return s[u >> SHIFT_BIT] & (1ull << (u & MASK));
    }
    int size()
    {
        return sz;
    }
    vector<ui> &getAll()
    {
        double start_get_time = get_system_time_microsecond();
        if (changed)
        {
            changed = false;
            all.clear();
            for (int i = 0; i < m; i++)
            {
                if (s[i])
                {
                    uint64_t p = s[i];
                    for (int j = 0; j <= MASK; j++)
                    {
                        if (p & 1)
                            all.push_back((i << SHIFT_BIT) + j);
                        p >>= 1;
                    }
                }
            }
            uint64_t p = s[m];
            for (int j = 0; j <= r; j++)
            {
                if (p & 1)
                    all.push_back((m << SHIFT_BIT) + j);
                p >>= 1;
            }
        }
        getall_time += get_system_time_microsecond() - start_get_time;
        return all;
    }
};

/**
 * linked list based on array; used for adjacent list
 * support: insert, check and remove a vertex with T(n)=O(1)
 * memory used : O(n)
 * However, this data struct is not good due to high missing rate of cache, 
 * so we only use it to store vertex set of G instead of adjacent list
 */
class MFastSet_LinkedList
{
    struct node
    {
        int pre, ne;
        int u;
    };
    int n;
    vector<node> node_list;
    int head; // empty node_list head
    vector<int> pos;
    vector<int> stk; // stack, store idx
    int top;
    int sz;

public:
    MFastSet_LinkedList()
    {
        top = sz = 0;
    }
    MFastSet_LinkedList(int _n)
    {
        n = _n;
        head = -1;
        pos.resize(n, -1);
        stk.resize(n);
        top = 0;
        for (int i = n - 1; i >= 0; i--)
            stk[top++] = i;
        sz = 0;
        node_list.resize(n);
    }
    ~MFastSet_LinkedList()
    {
    }
    void init(int _n)
    {
        n = _n;
        head = -1;
        pos.resize(n, -1);
        stk.resize(n);
        top = 0;
        for (int i = n - 1; i >= 0; i--)
            stk[top++] = i;
        sz = 0;
        node_list.resize(n);
    }
    bool has(int u)
    {
        return pos[u] != -1;
    }
    void remove(int u)
    {
        if(pos[u]==-1)
        {
            cout<<u<<endl;
        }
        assert(pos[u] != -1);
        double start_erase_time = get_system_time_microsecond();
        sz--;
        int idx = pos[u];
        pos[u] = -1;
        stk[top++] = idx;
        int pre = node_list[idx].pre, ne = node_list[idx].ne;
        if (head != idx)
        {
            node_list[pre].ne = ne;
        }
        else
        {
            head = ne;
        }
        if (ne != -1)
            node_list[ne].pre = pre;
        erase_time += get_system_time_microsecond() - start_erase_time;
    }
    void insert(int u)
    {
        assert(pos[u] == -1);
        double start_insert_time = get_system_time_microsecond();
        sz++;
        assert(top > 0);
        int idx = stk[--top];
        pos[u] = idx;
        node_list[idx] = {-1, head, u};
        if (head != -1)
            node_list[head].pre = idx;
        head = idx;
        insert_time += get_system_time_microsecond() - start_insert_time;
    }
    void clear()
    {
        double start_clear_time = get_system_time_microsecond();
        for (int i = head; i != -1; i = node_list[i].ne)
        {
            assert(pos[node_list[i].u] != -1);
            pos[node_list[i].u] = -1, stk[top++] = i;
        }
        head = -1;
        sz = 0;
        clear_time += get_system_time_microsecond() - start_clear_time;
    }
    void erase(int u)
    {
        remove(u);
    }
    bool count(int u)
    {
        return pos[u] != -1;
    }
    int size()
    {
        return sz;
    }
    class iterator
    {
    private:
        int idx;                        // current node index
        MFastSet_LinkedList *container; // pointer to the container

    public:
        // constructor
        iterator(int _idx, MFastSet_LinkedList *_container) : idx(_idx), container(_container) {}

        // dereference operator
        ui operator*() const
        {
            return container->node_list[idx].u;
        }

        // prefix increment operator
        iterator &operator++()
        {
            idx = container->node_list[idx].ne; // move to the next node
            return *this;
        }

        // postfix increment operator
        iterator operator++(int)
        {
            iterator tmp = *this; // make a copy of the current iterator
            ++(*this);            // use the prefix increment operator
            return tmp;           // return the copy
        }

        // equality operator
        bool operator==(const iterator &other) const
        {
            return idx == other.idx; // compare the node indices
        }

        // inequality operator
        bool operator!=(const iterator &other) const
        {
            return !(*this == other); // use the equality operator
        }
    };

    // begin function
    iterator begin()
    {
        return iterator(head, this); // return an iterator pointing to the head node
    }

    // end function
    iterator end()
    {
        return iterator(-1, this); // return an iterator pointing to the empty node (-1)
    }
};

#endif