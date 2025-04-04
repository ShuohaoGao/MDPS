#include <bits/stdc++.h>

using namespace std;

using ui = unsigned int;

const int N = 505;
int adj[N][N];

int main()
{
    ui n = 200;
    double density = 0.1;
    char txt_file_path[100];
    char bin_file_path[100];

    sprintf(txt_file_path, "dense-%d-%.1lf.txt", n, density);
    sprintf(bin_file_path, "dense-%d-%.1lf.bin", n, density);

    vector<pair<int, int>> edges;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
                continue;
            edges.push_back({i, j});
        }
    }
    random_shuffle(edges.begin(), edges.end());

    ui m = edges.size() * density;
    for (int i = 0; i < m; i++)
    {
        adj[edges[i].first][edges[i].second] = 1;
    }

    {
        ofstream output(txt_file_path);
        output << n << ' ' << m << endl;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (adj[i][j])
                    output << i << ' ' << j << '\n';
    }
    {
        ui *dout = new ui[n];
        ui *edge_to = new ui[m];
        ui idx = 0;
        for (int i = 0; i < n; i++)
        {
            dout[i] = 0;
            for (int j = 0; j < n; j++)
                if (adj[i][j])
                {
                    dout[i]++;
                    edge_to[idx++] = j;
                }
        }
        assert(idx == m);
        FILE *bin_f = fopen(bin_file_path, "wb");
        ui sz = sizeof(ui);
        fwrite(&sz, sizeof(ui), 1, bin_f);
        fwrite(&n, sizeof(ui), 1, bin_f);
        fwrite(&m, sizeof(ui), 1, bin_f);
        fwrite(dout, sizeof(ui), n, bin_f);
        fwrite(edge_to, sizeof(ui), m, bin_f);
        fclose(bin_f);
    }
    return 0;
}