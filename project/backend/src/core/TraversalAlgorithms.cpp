#include "TraversalAlgorithms.hpp"
#include <queue>
#include <stack>
#include <limits>
#include <random>
#include <algorithm>

namespace traversal {

std::optional<std::vector<std::string>> bfs_shortest_hops(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::string& src,
    const std::string& dst)
{
    if (src == dst) return std::vector<std::string>{src};
    std::queue<std::string> q;
    std::unordered_map<std::string, std::string> parent;
    q.push(src);
    parent[src] = "";
    while (!q.empty()) {
        auto u = q.front(); q.pop();
        for (auto v : getNeighbors(u)) {
            if (parent.find(v) == parent.end()) {
                parent[v] = u;
                if (v == dst) {
                    std::vector<std::string> path;
                    for (std::string cur = dst; !cur.empty(); cur = parent[cur]) path.push_back(cur);
                    std::reverse(path.begin(), path.end());
                    return path;
                }
                q.push(v);
            }
        }
    }
    return std::nullopt;
}

std::optional<std::vector<std::string>> dijkstra_shortest_delay(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& src,
    const std::string& dst)
{
    using Pair = std::pair<long long, std::string>;
    const long long INF = std::numeric_limits<long long>::max()/4;
    std::unordered_map<std::string, long long> dist;
    std::unordered_map<std::string, std::string> parent;
    auto cmp = [](const Pair& a, const Pair& b){ return a.first > b.first; };
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);

    dist[src] = 0;
    parent[src] = "";
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        if (d != dist[u]) continue;
        if (u == dst) break;
        for (auto v : getNeighbors(u)) {
            int w = getLinkDelay(u,v);
            if (w < 0) continue;
            long long nd = d + w;
            if (dist.find(v) == dist.end() || nd < dist[v]) {
                dist[v] = nd;
                parent[v] = u;
                pq.push({nd, v});
            }
        }
    }

    if (dist.find(dst) == dist.end()) return std::nullopt;
    std::vector<std::string> path;
    for (std::string cur = dst; !cur.empty(); cur = parent[cur]) path.push_back(cur);
    std::reverse(path.begin(), path.end());
    return path;
}

std::optional<std::vector<std::string>> dfs_any_path(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::string& src,
    const std::string& dst,
    int maxDepth)
{
    std::unordered_map<std::string, bool> visited;
    std::vector<std::string> path;
    std::function<bool(const std::string&, int)> dfs = [&](const std::string& u, int depth)->bool {
        if (depth > maxDepth) return false;
        visited[u] = true;
        path.push_back(u);
        if (u == dst) return true;
        for (auto v : getNeighbors(u)) {
            if (!visited[v]) {
                if (dfs(v, depth+1)) return true;
            }
        }
        path.pop_back();
        return false;
    };
    if (dfs(src, 0)) return path;
    return std::nullopt;
}

std::unordered_map<std::string, std::vector<std::string>> multicast_bfs(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::string& src,
    const std::vector<std::string>& destinations)
{
    std::unordered_map<std::string, std::vector<std::string>> result;
    std::unordered_map<std::string, std::string> parent;
    std::queue<std::string> q;
    std::unordered_set<std::string> destSet(destinations.begin(), destinations.end());
    q.push(src);
    parent[src] = "";

    while (!q.empty() && !destSet.empty()) {
        auto u = q.front(); q.pop();
        for (auto v : getNeighbors(u)) {
            if (parent.find(v) == parent.end()) {
                parent[v] = u;
                if (destSet.find(v) != destSet.end()) {
                    std::vector<std::string> path;
                    for (std::string cur = v; !cur.empty(); cur = parent[cur]) path.push_back(cur);
                    std::reverse(path.begin(), path.end());
                    result[v] = path;
                    destSet.erase(v);
                }
                q.push(v);
            }
        }
    }
    return result;
}

bool probabilistic_flood(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::string& src,
    const std::string& dst,
    double forwardProb,
    unsigned seed)
{
    if (src == dst) return true;
    std::mt19937 rng(seed ? seed : std::random_device{}());
    std::uniform_real_distribution<double> distReal(0.0, 1.0);

    std::queue<std::string> q;
    std::unordered_set<std::string> visited;
    q.push(src);
    visited.insert(src);

    while (!q.empty()) {
        auto u = q.front(); q.pop();
        for (auto v : getNeighbors(u)) {
            if (visited.find(v) != visited.end()) continue;
            if (distReal(rng) <= forwardProb) {
                if (v == dst) return true;
                visited.insert(v);
                q.push(v);
            }
        }
    }
    return false;
}

} // namespace traversal