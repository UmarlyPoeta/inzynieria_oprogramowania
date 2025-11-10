#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <functional>

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

} // namespace traversal