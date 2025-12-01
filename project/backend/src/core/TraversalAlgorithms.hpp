#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <functional>
#include <tuple>

namespace traversal {

std::optional<std::vector<std::string>> bfs_shortest_hops(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::string& src,
    const std::string& dst);

std::optional<std::vector<std::string>> dijkstra_shortest_delay(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& src,
    const std::string& dst);

std::optional<std::vector<std::string>> dfs_any_path(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::string& src,
    const std::string& dst,
    int maxDepth = 1000);

std::unordered_map<std::string, std::vector<std::string>> multicast_bfs(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::string& src,
    const std::vector<std::string>& destinations);

bool probabilistic_flood(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::string& src,
    const std::string& dst,
    double forwardProb = 0.8,
    unsigned seed = 0);

// Equal‑Cost MultiPath (ECMP)
// Returns next hops (neighbor names) from `node` that are on equal‑cost shortest paths towards `dst`.
std::vector<std::string> equal_cost_next_hops(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& node,
    const std::string& dst);

// Enumerate up to `k` ECMP paths from `src` to `dst` (paths built by following equal‑cost edges).
std::vector<std::vector<std::string>> ecmp_k_paths(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& src,
    const std::string& dst,
    int k = 4);

// Constrained shortest path: minimize delay subject to bandwidth >= minBandwidth on every hop
std::optional<std::vector<std::string>> constrained_shortest_path(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::function<int(const std::string&, const std::string&)>& getLinkBandwidth,
    const std::string& src,
    const std::string& dst,
    int minBandwidth);

// Link‑State routing: compute next‑hop routing table for `src` using global view
// Returns map: destination -> nextHop (empty map entry means unreachable)
std::unordered_map<std::string, std::string> link_state_routing(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& src);

// Multipath flow-aware routing
// Returns up to `k` candidate paths from `src` to `dst` together with a
// weight (fraction of flow) to send on each path. Weights sum to 1.0.
std::vector<std::pair<std::vector<std::string>, double>> multipath_flow_aware(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::function<int(const std::string&, const std::string&)>& getLinkBandwidth,
    const std::string& src,
    const std::string& dst,
    int k = 4);

// Multi-commodity flow (greedy approximate)
// Input: list of commodities (src, dst, demand). k = max paths per commodity.
// Output: map key "src->dst" -> vector of (path, assignedFlow)
std::unordered_map<std::string, std::vector<std::pair<std::vector<std::string>, int>>> multi_commodity_flow(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::function<int(const std::string&, const std::string&)>& getLinkBandwidth,
    const std::vector<std::tuple<std::string, std::string, int>>& commodities,
    int k = 4);

// Reverse Path Forwarding (RPF) check
// Returns true if `incomingNeighbor` is on a shortest-path from `src` to `node`.
bool is_rpf(
    const std::function<std::vector<std::string>(const std::string&)>& getNeighbors,
    const std::function<int(const std::string&, const std::string&)>& getLinkDelay,
    const std::string& node,
    const std::string& incomingNeighbor,
    const std::string& src);

} // namespace traversal