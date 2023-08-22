#ifndef UTILITY_H
#define UTILITY_H

#include <bits/stdc++.h>
#include <sys/time.h> // gettimeofday
#include <unistd.h>
#define x first
#define y second

using namespace std;
using ll = long long;
using ui = unsigned int;
using pii = pair<ui, ui>;

const int INF = 0x3f3f3f3f;

int paramK, paramL;
int lb;
double insert_time, clear_time, getall_time, erase_time;
inline ll get_system_time_microsecond()
{
    static struct timeval timestamp = {};
    gettimeofday(&timestamp, NULL);
    return timestamp.tv_sec * 1000000 + timestamp.tv_usec;
    if (0 == gettimeofday(&timestamp, NULL))
        return timestamp.tv_sec * 1000000 + timestamp.tv_usec;
    else
        return 0;
}

string get_file_name(string str)
{
    string ret = "";
    for (char ch : str)
    {
        ret += ch;
        if (ch == '\\' || ch == '/')
            ret = "";
    }
    return ret;
}

string get_file_name_without_suffix(string name)
{
    name = get_file_name(name);
    string ret = "";
    for (char ch : name)
    {
        if (ch == '.')
            break;
        ret += ch;
    }
    return ret;
}

// faster than std::sort
void countingSort(vector<pii> &a, int k)
{
    vector<vector<pii>> p;
    p.resize(k);
    for (pii &h : a)
        p[h.y].push_back(h);
    vector<vector<pii>> q;
    q.resize(k);
    for (int i = 0; i < k; i++)
        for (pii &h : p[i])
            q[h.x].push_back(h);
    int sz = 0;
    for (int i = 0; i < k; i++)
        for (pii &h : q[i])
            a[sz++] = h;
}

void unique_pii(vector<pii> &a, int n)
{
    countingSort(a, n);
    a.erase(unique(a.begin(), a.end()), a.end());
}

template <typename T>
void unique_vector(vector<T> &a)
{
    sort(a.begin(), a.end());
    a.erase(unique(a.begin(), a.end()), a.end());
}

template <typename T>
bool has(vector<T> &a, T x)
{
    auto it = lower_bound(a.begin(), a.end(), x);
    return it != a.end() && *it == x;
}

template <typename T>
int find(vector<T> &a, T x)
{
    return lower_bound(a.begin(), a.end(), x) - a.begin();
}

void print_progress_bar(double percentage)
{
#ifdef NO_PROGRESS_BAR
    return;
#endif
    const int bar_length = 50;
    const char fill_char = '#';
    const char empty_char = '-';
    int filled_length = (int)(percentage * bar_length);
    int empty_length = bar_length - filled_length;
    char buffer[bar_length + 10];
    sprintf(buffer, "\r%.2lf%% [", percentage * 100);
    int len = strlen(buffer);
    for (int i = 0; i < filled_length; i++)
    {
        buffer[len + i] = fill_char;
    }
    for (int i = 0; i < empty_length; i++)
    {
        buffer[len + filled_length + i] = empty_char;
    }
    buffer[bar_length + len] = ']';
    write(STDOUT_FILENO, buffer, bar_length + len + 1);
    fflush(stdout);
}

void print_set(set<int> &s)
{
    auto it = s.begin();
    printf("%d", *it);
    it++;
    while (it != s.end())
    {
        printf(", %d", *it);
        it++;
    }
    puts("");
}

#endif