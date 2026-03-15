/*
 * CSE4502 Operating Systems Lab 9: 1B
 * Deadlock Detection in Single-Instance Resource Systems (RAG Cycle Detection)
 *
 * Input format:
 *   Line 1: N M  (processes 0..N-1, resources 0..M-1)
 *   Line 2: E    (number of edges)
 *   Next E lines: u v type
 *     type "R" -> Process u requests Resource v
 *     type "A" -> Resource u is assigned to Process v
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

// Build a directed graph and detect a cycle using DFS
// Nodes: processes 0..N-1, resources N..N+M-1
// Edge: request  P_i -> R_j  means edge (i, N+j)
// Edge: assigned R_j -> P_i  means edge (N+j, i)

int N, M, E;
vector<vector<int>> adj;
vector<int> color; // 0=white, 1=gray, 2=black

bool dfs(int u) {
    color[u] = 1; // Mark as being visited
    for (int v : adj[u]) {
        if (color[v] == 1) return true;  // Back edge -> cycle
        if (color[v] == 0 && dfs(v)) return true;
    }
    color[u] = 2; // Fully processed
    return false;
}

int main() {
    cin >> N >> M >> E;

    int totalNodes = N + M;
    adj.assign(totalNodes, vector<int>());
    color.assign(totalNodes, 0);

    for (int i = 0; i < E; i++) {
        int u, v;
        string type;
        cin >> u >> v >> type;

        if (type == "R") {
            // Process u requests Resource v
            adj[u].push_back(N + v);
        } else {
            // Resource u assigned to Process v
            adj[N + u].push_back(v);
        }
    }

    bool hasCycle = false;
    for (int i = 0; i < totalNodes; i++) {
        if (color[i] == 0) {
            if (dfs(i)) { hasCycle = true; break; }
        }
    }

    if (hasCycle)
        cout << "DEADLOCK" << endl;
    else
        cout << "NO DEADLOCK" << endl;

    return 0;
}
