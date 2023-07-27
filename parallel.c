#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#define INF 1000000000

int n=10, m, s=0;
int dist[1000], prev[1000];
double graph[10][10] = {{INF, 0.1083, 0.0108, 4.8517, 0.0903, 10.7679, 11.0991, 1.9617, 1.6840, 0.0240},
                    {-0.0899, INF, 0.1222, 5.0664, 0.1906, 10.7225, 11.0509, 2.3364, 2.0591, 0.1478},
                    {-0.0109, -0.1215, INF, 4.7185, -0.0295, 10.8898, 11.2201, 1.9217, 1.6441, 0.0083},
                    {-0.2068, -0.3190, -0.1831, INF, -0.2109, 9.8209, 10.1298, -6.1915, -6.2257, -0.4170},
                    {-0.1093, -0.2215, -0.0857, 4.5990, INF, 10.3471, 10.6780, 1.8316, 1.5539, -0.0325},
                    {-4.6598, -4.7720, -4.6361, -1.6092, -4.7193, INF, 0.5135, -11.5165, -11.4845, -4.6932},
                    {-4.5713, -4.6835, -4.5477, -1.6401, -4.6573, -0.6783, INF, -12.0749, -12.0429, -4.6058},
                    {4.9698, 4.8576, 4.9935, 13.0125, 5.0275, 16.4334, 16.7654, INF, 16.8019, 4.8576},
                    {-0.0006, -0.1128, 0.0230, 7.0520, -0.1171, 9.9892, 10.3200, -6.9983, INF, -0.2029},
                    {-0.0233, -0.1355, -0.0016, 4.9415, 0.0482, 10.9445, 11.2746, 1.8835, 1.6058, INF}};


bool bellman_ford() {
    for (int i = 0; i < n; i++) {
        dist[i] = INF;// current shortest distance from source s to vertex i
        prev[i] = -1;//predecessor of vertex i on the shortest path from s
    }
    dist[s] = 0;

    for (int i = 0; i < n-1; i++) {
//distribute the iterations of the loop across multiple threads
#pragma omp parallel for
        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                if (graph[u][v] != INF && dist[u] + graph[u][v] < dist[v]) {
                    dist[v] = dist[u] + graph[u][v];
                    prev[v] = u;
                }
            }
        }
    }

    // for (int i=0;i<n;i++){
    //     printf("%d",dist[i]);
    // }
    // printf("\n");

    bool negative_cycle = false;
    int u = -1;
    int visited[1000] = {0};
//collapse the two nested loops into a single loop, which can increase the granularity of parallelism and reduce overhead
#pragma omp parallel for collapse(2) shared(negative_cycle,u)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (graph[i][j] != INF && dist[i] + graph[i][j] < dist[j]) {
#pragma omp critical
                {
                    negative_cycle = true;
                    u = i;
                }
                // Mark all vertices in the cycle
                int x = j;
                while (!visited[x]) {
                    visited[x] = 1;
                    x = prev[x];
                }
                visited[x] = 1;
            }
        }
    }

    if (negative_cycle) {
        printf("Negative cycle found: ");
        // Print the vertices in the cycle in reverse order
        int x = u;
        int c=0;
        while (!visited[x]) {
            x = prev[x];
        }
        int y = x;
        do {
            printf("<-%d ", y);
            y = prev[y];
        } while (y != x);
        printf("<-%d\n", x);
        return true;
    }

    return false;
}

int main() {

    double start_time = omp_get_wtime();
    if (!bellman_ford()) {
        printf("No negative cycles found.\n");
    }
    double end_time = omp_get_wtime();
    double elapsed_time = end_time - start_time;
    printf("Elapsed time: %f seconds\n", elapsed_time);

    return 0;
}
