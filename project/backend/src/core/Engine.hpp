#pragma once
#include "Network.hpp"
#include "Packet.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

class Engine {
public:
    explicit Engine(Network& net);
    enum class PingMode { HOPS, DELAY, DFS };

    // ping now supports selecting traversal algorithm via PingMode.
    // - HOPS: shortest number of hops (BFS) [default]
    // - DELAY: shortest total link delay (Dijkstra)
    // - DFS: any path found by DFS (useful for testing)
    // maxDepth is used only for DFS mode.
    bool ping(const std::string &srcName,
               const std::string &dstName,
               std::vector<std::string> &path,
               PingMode mode = PingMode::HOPS,
               int maxDepth = 1000);
    bool traceroute(const std::string &srcName,
                    const std::string &dstName,
                    std::vector<std::string> &path);
    int getTotalDelay(const std::vector<std::string>& path);
    
    // Multicast - wysyłanie do wielu odbiorców
    bool multicast(const std::string& srcName, const std::vector<std::string>& destinations);

    // ECMP helper: list equal-cost next hops from `node` towards `dst`
    std::vector<std::string> getEqualCostNextHops(const std::string& node, const std::string& dst);
    // Find shortest path minimizing delay but only using links with bandwidth >= minBandwidth
    bool constrainedShortestPath(const std::string& src,
                                 const std::string& dst,
                                 std::vector<std::string>& path,
                                 int minBandwidth);
    // Compute link-state (next-hop) routing table for `node`
    std::unordered_map<std::string, std::string> computeLinkStateRouting(const std::string& node);

    // Reverse Path Forwarding (RPF) check: returns true if `incomingNeighbor` is on
    // a shortest-path from `src` to `node` (used by forwarding/routing logic).
    bool isRPF(const std::string& node, const std::string& incomingNeighbor, const std::string& src);

    // Multipath flow-aware: return k paths and per-path weight (fraction)
    std::vector<std::pair<std::vector<std::string>, double>> multipathFlowAware(const std::string& src,
                                                                                const std::string& dst,
                                                                                int k = 4);

    // Multi-commodity flow planner (greedy approximate)
    // Input: vector of (src,dst,demand). Returns map key "src->dst" -> list of (path, assignedFlow)
    std::unordered_map<std::string, std::vector<std::pair<std::vector<std::string>, int>>> multiCommodityFlow(
        const std::vector<std::tuple<std::string, std::string, int>>& commodities,
        int k = 4);

private:
    Network &net;
};
