#include "TraversalAlgorithms.hpp"
#include <queue>
#include <stack>
#include <limits>
#include <random>
#include <algorithm>

namespace traversal {

// ============= Internal Helper Functions =============

// Rekonstrukcja ścieżki z mapy parent (od dst do src, potem odwrócenie)
static std::vector<std::string> reconstruct_path(
    const std::unordered_map<std::string, std::string>& parent,
    const std::string& src,
    const std::string& dst)
{
    std::vector<std::string> path;
    std::string cur = dst;
    while (!cur.empty()) {
        path.push_back(cur);
        auto it = parent.find(cur);
        if (it == parent.end()) break;
        cur = it->second;
        if (cur == src) {
            path.push_back(src);
            break;
        }
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// Komparator dla min-heap Dijkstra (para: distance, node)
static auto make_dijkstra_comparator() {
    return [](const std::pair<long long, std::string>& a,
              const std::pair<long long, std::string>& b) {
        return a.first > b.first;
    };
}

// ============= End Helper Functions =============

std::optional<std::vector<std::string>> bfs_shortest_hops(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::string& src,
    const std::string& dst)
{
    if (src == dst) return std::vector<std::string>{src};
    std::queue<std::string> q;
    std::unordered_map<std::string, std::string> parent;
    q.push(src);
    parent[src] = std::string();
    while (!q.empty()) {
        auto u = q.front(); q.pop();
        for (const auto& v : getNeighbors(u)) {
            if (parent.find(v) == parent.end()) {
                parent[v] = u;
                if (v == dst) {
                    return reconstruct_path(parent, src, dst);
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
    std::unordered_map<std::string, long long> dist;
    std::unordered_map<std::string, std::string> parent;
    auto cmp = make_dijkstra_comparator();
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);
    dist[src] = 0;
    parent[src] = std::string();
    pq.push({0, src});

    while (!pq.empty()) {
        auto top = pq.top(); pq.pop();
        long long d = top.first;
        const std::string u = top.second;
        auto itDist = dist.find(u);
        if (itDist == dist.end() || itDist->second != d) continue;
        if (u == dst) break;
        for (const auto& v : getNeighbors(u)) {
            int w = getLinkDelay(u, v);
            if (w < 0) continue;
            long long nd = d + static_cast<long long>(w);
            auto itV = dist.find(v);
            if (itV == dist.end() || nd < itV->second) {
                dist[v] = nd;
                parent[v] = u;
                pq.push({nd, v});
            }
        }
    }

    if (dist.find(dst) == dist.end()) return std::nullopt;
    std::vector<std::string> path;
    std::string cur = dst;
    while (!cur.empty()) {
        path.push_back(cur);
        auto it = parent.find(cur);
        if (it == parent.end()) break;
        cur = it->second;
    }
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
        for (const auto& v : getNeighbors(u)) {
            auto it = visited.find(v);
            if (it == visited.end() || !it->second) {
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
        for (const auto& v : getNeighbors(u)) {
            if (parent.find(v) == parent.end()) {
                parent[v] = u;
                if (destSet.find(v) != destSet.end()) {
                    result[v] = reconstruct_path(parent, src, v);
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
        for (const auto& v : getNeighbors(u)) {
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


// ---------------- ECMP implementations ----------------
namespace traversal {

std::vector<std::string> equal_cost_next_hops(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& node,
    const std::string& dst)
{
    // Compute distances from every node to dst (run Dijkstra from dst on reversed costs)
    using Pair = std::pair<long long, std::string>;
    std::unordered_map<std::string, long long> dist;
    auto cmp = make_dijkstra_comparator();
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);

    // start from dst
    dist[dst] = 0;
    pq.push({0, dst});

    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        auto it = dist.find(u);
        if (it == dist.end() || it->second != d) continue;
        for (const auto& nei : getNeighbors(u)) {
            // edge nei <-> u: cost = getLinkDelay(nei, u)
            int w = getLinkDelay(nei, u);
            if (w < 0) continue;
            long long nd = d + static_cast<long long>(w);
            auto itn = dist.find(nei);
            if (itn == dist.end() || nd < itn->second) {
                dist[nei] = nd;
                pq.push({nd, nei});
            }
        }
    }

    std::vector<std::string> result;
    auto itNode = dist.find(node);
    if (itNode == dist.end()) return result; // node unreachable to dst
    long long nodeDist = itNode->second;

    // For each neighbor v of node, check if dist[v] + w(node,v) == dist[node]
    for (const auto& v : getNeighbors(node)) {
        int w = getLinkDelay(node, v);
        if (w < 0) continue;
        auto itv = dist.find(v);
        if (itv != dist.end() && itv->second + static_cast<long long>(w) == nodeDist) {
            result.push_back(v);
        }
    }
    return result;
}

std::vector<std::vector<std::string>> ecmp_k_paths(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& src,
    const std::string& dst,
    int k)
{
    std::vector<std::vector<std::string>> paths;
    if (src == dst) {
        paths.push_back({src});
        return paths;
    }

    // First compute equal-cost DAG edges using distances-to-dst (Dijkstra from dst)
    using Pair = std::pair<long long, std::string>;
    std::unordered_map<std::string, long long> dist;
    auto cmp = make_dijkstra_comparator();
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);
    dist[dst] = 0;
    pq.push({0, dst});
    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        auto it = dist.find(u);
        if (it == dist.end() || it->second != d) continue;
        for (const auto& nei : getNeighbors(u)) {
            int w = getLinkDelay(nei, u);
            if (w < 0) continue;
            long long nd = d + static_cast<long long>(w);
            auto itn = dist.find(nei);
            if (itn == dist.end() || nd < itn->second) {
                dist[nei] = nd;
                pq.push({nd, nei});
            }
        }
    }

    if (dist.find(src) == dist.end()) return paths;

    // Build DAG edges: from u to v is allowed if dist[v] + w(u,v) == dist[u]
    std::function<void(const std::string&, std::vector<std::string>&)> dfsBuild;
    dfsBuild = [&](const std::string& cur, std::vector<std::string>& curPath) {
        if ((int)paths.size() >= k) return;
        curPath.push_back(cur);
        if (cur == dst) {
            paths.push_back(curPath);
            curPath.pop_back();
            return;
        }
        auto neighs = getNeighbors(cur);
        for (const auto& v : neighs) {
            int w = getLinkDelay(cur, v);
            if (w < 0) continue;
            auto itv = dist.find(v);
            auto itc = dist.find(cur);
            if (itv != dist.end() && itc != dist.end() && itv->second + static_cast<long long>(w) == itc->second) {
                dfsBuild(v, curPath);
                if ((int)paths.size() >= k) break;
            }
        }
        curPath.pop_back();
    };

    std::vector<std::string> p;
    dfsBuild(src, p);
    return paths;
}

} // namespace traversal


// ---------------- Constrained Shortest Path ----------------
namespace traversal {

std::optional<std::vector<std::string>> constrained_shortest_path(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::function<int(const std::string&, const std::string&)>& getLinkBandwidth,
    const std::string& src,
    const std::string& dst,
    int minBandwidth)
{
    if (src == dst) return std::vector<std::string>{src};

    using Pair = std::pair<long long, std::string>;
    std::unordered_map<std::string, long long> dist;
    std::unordered_map<std::string, std::string> parent;
    auto cmp = make_dijkstra_comparator();
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);

    dist[src] = 0;
    parent[src] = std::string();
    pq.push({0, src});

    while (!pq.empty()) {
        auto top = pq.top(); pq.pop();
        long long d = top.first;
        const std::string u = top.second;
        auto itDist = dist.find(u);
        if (itDist == dist.end() || itDist->second != d) continue;
        if (u == dst) break;

        for (const auto& v : getNeighbors(u)) {
            int bw = getLinkBandwidth(u, v);
            if (bw < minBandwidth) continue; // skip edges that don't meet bandwidth
            int w = getLinkDelay(u, v);
            if (w < 0) continue;
            long long nd = d + static_cast<long long>(w);
            auto itV = dist.find(v);
            if (itV == dist.end() || nd < itV->second) {
                dist[v] = nd;
                parent[v] = u;
                pq.push({nd, v});
            }
        }
    }

    if (dist.find(dst) == dist.end()) return std::nullopt;
    return reconstruct_path(parent, src, dst);
}

} // namespace traversal


// ---------------- Link-State routing (LSA + Dijkstra) ----------------
namespace traversal {

std::unordered_map<std::string, std::string> link_state_routing(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& src)
{
    using Pair = std::pair<long long, std::string>;
    std::unordered_map<std::string, long long> dist;
    std::unordered_map<std::string, std::string> parent;
    auto cmp = make_dijkstra_comparator();
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);

    // Dijkstra from src
    dist[src] = 0;
    parent[src] = std::string();
    pq.push({0, src});

    while (!pq.empty()) {
        auto top = pq.top(); pq.pop();
        long long d = top.first;
        const std::string u = top.second;
        auto itDist = dist.find(u);
        if (itDist == dist.end() || itDist->second != d) continue;
        for (const auto& v : getNeighbors(u)) {
            int w = getLinkDelay(u, v);
            if (w < 0) continue;
            long long nd = d + static_cast<long long>(w);
            auto itV = dist.find(v);
            if (itV == dist.end() || nd < itV->second) {
                dist[v] = nd;
                parent[v] = u;
                pq.push({nd, v});
            }
        }
    }

    // Build next-hop table: for each reachable destination, walk back to src to determine next hop
    std::unordered_map<std::string, std::string> routing;
    for (const auto& entry : dist) {
        const std::string& dst = entry.first;
        if (dst == src) continue;
        // walk back from dst to src
        std::string cur = dst;
        std::string prev = cur;
        bool ok = true;
        while (true) {
            auto it = parent.find(cur);
            if (it == parent.end()) { ok = false; break; }
            const std::string& p = it->second;
            if (p.empty()) { // reached src
                break;
            }
            prev = cur;
            cur = p;
            if (cur == src) break;
        }
        if (!ok) continue;
        // now prev is the node immediately after src on path src -> ... -> dst
        // but if parent[dst] == src then prev == dst, need to find next hop differently
        // Walk from dst back until parent == src
        cur = dst;
        std::string nextHop;
        while (true) {
            auto it = parent.find(cur);
            if (it == parent.end()) { nextHop.clear(); break; }
            const std::string& p = it->second;
            if (p == src) {
                nextHop = cur;
                break;
            }
            cur = p;
            if (cur.empty()) { nextHop.clear(); break; }
        }
        if (!nextHop.empty()) routing[dst] = nextHop;
    }
    return routing;
}

} // namespace traversal

// ---------------- Multipath flow-aware routing ----------------
namespace traversal {

std::vector<std::pair<std::vector<std::string>, double>> multipath_flow_aware(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::function<int(const std::string&, const std::string&)>& getLinkBandwidth,
    const std::string& src,
    const std::string& dst,
    int k)
{
    std::vector<std::pair<std::vector<std::string>, double>> result;
    if (src == dst) {
        result.push_back({{src}, 1.0});
        return result;
    }

    using Pair = std::pair<long long, std::string>;
    auto make_key = [](const std::string &a, const std::string &b){ return a + "->" + b; };
    std::unordered_set<std::string> bannedEdges;

    for (int iter = 0; iter < k; ++iter) {
        // Dijkstra skipping banned edges
        std::unordered_map<std::string, long long> dist;
        std::unordered_map<std::string, std::string> parent;
        auto cmp = make_dijkstra_comparator();
        std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            auto top = pq.top(); pq.pop();
            long long d = top.first;
            const std::string u = top.second;
            auto itDist = dist.find(u);
            if (itDist == dist.end() || itDist->second != d) continue;
            if (u == dst) break;
            for (const auto &v : getNeighbors(u)) {
                if (bannedEdges.find(make_key(u, v)) != bannedEdges.end()) continue;
                int w = getLinkDelay(u, v);
                if (w < 0) continue;
                long long nd = d + static_cast<long long>(w);
                auto itV = dist.find(v);
                if (itV == dist.end() || nd < itV->second) {
                    dist[v] = nd;
                    parent[v] = u;
                    pq.push({nd, v});
                }
            }
        }

        if (dist.find(dst) == dist.end()) break; // no more paths

        // reconstruct path
        auto path = reconstruct_path(parent, src, dst);
        if (path.empty() || path.front() != src) break; // safety

        // compute min bandwidth along path
        int minBw = std::numeric_limits<int>::max();
        for (size_t i = 0; i + 1 < path.size(); ++i) {
            int bw = getLinkBandwidth(path[i], path[i+1]);
            if (bw < 0) { minBw = 0; break; }
            minBw = std::min(minBw, bw);
        }
        if (minBw <= 0) {
            // edge case: path exists but no bandwidth; skip this path
            // mark edges as banned to try to find another
            for (size_t i = 0; i + 1 < path.size(); ++i) bannedEdges.insert(make_key(path[i], path[i+1]));
            continue;
        }

        result.push_back({path, static_cast<double>(minBw)});

        // ban edges of this path to find alternative paths next iteration
        for (size_t i = 0; i + 1 < path.size(); ++i) bannedEdges.insert(make_key(path[i], path[i+1]));
    }

    if (result.empty()) return result;

    // Convert capacities (minBw) to normalized weights
    double totalCap = 0.0;
    for (auto &p : result) totalCap += p.second;
    if (totalCap <= 0.0) {
        // fallback: equal split
        double w = 1.0 / static_cast<double>(result.size());
        for (auto &p : result) p.second = w;
    } else {
        for (auto &p : result) p.second = p.second / totalCap;
    }

    return result;
}

} // namespace traversal


// ---------------- Reverse Path Forwarding (RPF) ----------------
namespace traversal {

bool is_rpf(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& node,
    const std::string& incomingNeighbor,
    const std::string& src)
{
    // Compute distances from src to all nodes (Dijkstra)
    using Pair = std::pair<long long, std::string>;
    std::unordered_map<std::string, long long> dist;
    auto cmp = make_dijkstra_comparator();
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        auto it = dist.find(u);
        if (it == dist.end() || it->second != d) continue;
        for (const auto& v : getNeighbors(u)) {
            int w = getLinkDelay(u, v);
            if (w < 0) continue;
            long long nd = d + static_cast<long long>(w);
            auto itv = dist.find(v);
            if (itv == dist.end() || nd < itv->second) {
                dist[v] = nd;
                pq.push({nd, v});
            }
        }
    }

    auto itNode = dist.find(node);
    auto itNei = dist.find(incomingNeighbor);
    if (itNode == dist.end() || itNei == dist.end()) return false;

    int w = getLinkDelay(incomingNeighbor, node);
    if (w < 0) return false;

    return (itNei->second + static_cast<long long>(w)) == itNode->second;
}

} // namespace traversal

// ---------------- Multi-Commodity Flow (greedy approx) ----------------
namespace traversal {

std::unordered_map<std::string, std::vector<std::pair<std::vector<std::string>, int>>> multi_commodity_flow(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::function<int(const std::string&, const std::string&)>& getLinkBandwidth,
    const std::vector<std::tuple<std::string, std::string, int>>& commodities,
    int k)
{
    std::unordered_map<std::string, std::vector<std::pair<std::vector<std::string>, int>>> allocation;

    // Build initial capacity map for directed edges
    auto make_key = [](const std::string &a, const std::string &b){ return a + "->" + b; };
    std::unordered_map<std::string, int> capacity;

    // Initialize capacities by probing neighbors for each node
    // We simply iterate over each node's neighbors - this may duplicate edges but sets capacity consistently
    std::vector<std::string> seenNodes;
    // gather nodes by calling getNeighbors on nodes discovered via commodities
    for (const auto &c : commodities) {
        const std::string &s = std::get<0>(c);
        const std::string &d = std::get<1>(c);
        seenNodes.push_back(s);
        seenNodes.push_back(d);
    }

    // Also try to expand node set by one-level neighbors
    std::unordered_set<std::string> nodeSet(seenNodes.begin(), seenNodes.end());
    std::vector<std::string> expand = seenNodes;
    for (const auto &n : expand) {
        for (const auto &v : getNeighbors(n)) nodeSet.insert(v);
    }

    for (const auto &n : nodeSet) {
        for (const auto &v : getNeighbors(n)) {
            int bw = getLinkBandwidth(n, v);
            if (bw < 0) bw = 0;
            capacity[make_key(n, v)] = bw;
        }
    }

    // For each commodity, greedily assign up to k paths using residual capacities
    for (const auto &c : commodities) {
        std::string src = std::get<0>(c);
        std::string dst = std::get<1>(c);
        int demand = std::get<2>(c);
        std::string key = src + "->" + dst;
        allocation[key] = {};
        if (demand <= 0) continue;

        for (int iter = 0; iter < k && demand > 0; ++iter) {
            // Dijkstra on edges with positive residual capacity
            using Pair = std::pair<long long, std::string>;
            std::unordered_map<std::string, long long> dist;
            std::unordered_map<std::string, std::string> parent;
            auto cmp = make_dijkstra_comparator();
            std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);

            dist[src] = 0;
            pq.push({0, src});

            while (!pq.empty()) {
                auto top = pq.top(); pq.pop();
                long long d = top.first;
                const std::string u = top.second;
                auto itDist = dist.find(u);
                if (itDist == dist.end() || itDist->second != d) continue;
                if (u == dst) break;
                for (const auto &v : getNeighbors(u)) {
                    auto capIt = capacity.find(make_key(u, v));
                    if (capIt == capacity.end() || capIt->second <= 0) continue; // no residual capacity
                    int w = getLinkDelay(u, v);
                    if (w < 0) continue;
                    long long nd = d + static_cast<long long>(w);
                    auto itV = dist.find(v);
                    if (itV == dist.end() || nd < itV->second) {
                        dist[v] = nd;
                        parent[v] = u;
                        pq.push({nd, v});
                    }
                }
            }

            if (dist.find(dst) == dist.end()) break; // can't satisfy more for this commodity

            // reconstruct path
            auto path = reconstruct_path(parent, src, dst);
            if (path.empty() || path.front() != src) break;

            // find bottleneck
            int bottleneck = std::numeric_limits<int>::max();
            for (size_t i = 0; i + 1 < path.size(); ++i) {
                int cap = capacity[make_key(path[i], path[i+1])];
                bottleneck = std::min(bottleneck, cap);
            }
            if (bottleneck <= 0) break;

            int assign = std::min(bottleneck, demand);
            // apply assignment
            for (size_t i = 0; i + 1 < path.size(); ++i) {
                std::string e = make_key(path[i], path[i+1]);
                capacity[e] -= assign;
                if (capacity[e] < 0) capacity[e] = 0;
            }
            allocation[key].push_back({path, assign});
            demand -= assign;
        }

        // remaining demand could not be satisfied; allocation contains partial fulfilment
    }

    return allocation;
}

} // namespace traversal