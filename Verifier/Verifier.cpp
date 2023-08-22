#include "Verifier.h"

// directed (k,l)-plex
void checkKLPlex(set<int> &s, string file, int k, int l)
{
    Verifier a(s, file);
    printf("size = %d\n", (int)s.size());
    if (a.is_correct(k, l))
        printf("is a (%d,%d)-plex\n",k,l);
    else
    {
        printf("not a (%d,%d)-plex\n",k,l);
        pii h = a.get();
        printf("is a (%d,%d)-plex\n", h.x, h.y);
    }
}
// undirected k-plex
void checkKPlex(set<int> &s, string file, int k)
{
    Verifier_undirected_kPlex a(s, file);
    printf("size = %d\n", (int)s.size());
    if (a.is_correct(5))
        printf("is a %d-plex\n",k);
    else
    {
        printf("not a %d-plex\n",k);
        int h = a.get_k();
        printf("is a %d-plex\n", h);
    }
}

int main()
{
    set<int> s{
        2, 3, 10, 19, 21, 26, 31, 33, 44, 51, 55, 57, 59, 60, 64, 65, 71, 72, 78, 84, 87, 89, 93, 97, 111, 115, 121, 123, 124, 132, 140, 141, 147, 148, 152, 156, 163, 164, 165, 180, 196, 205, 206, 207, 213, 214, 217, 221, 226, 230, 233, 241, 244, 245, 248, 249, 252, 253, 255, 258, 259, 260, 262, 268, 270, 273, 775, 835, 884, 920, 936, 7722};
    string file = "D:\\Data\\datasets\\socfb-FSU53-directed.txt";
    int k=4;
    int l=4;
    checkKLPlex(s, file, k, l);
    return 0;
}