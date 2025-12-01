#include "Engine.hpp"
#include "TraversalAlgorithms.hpp"
#include <iostream>
#include <algorithm>

Engine::Engine(Network &network) : net(network) {}

bool Engine::ping(const std::string &src, const std::string &dst, std::vector<std::string> &pathOut, PingMode mode, int maxDepth) {
    std::cout << "[PING] From " << src << " to " << dst << std::endl;

    const auto &adj = net.getAdjacency();
    if (src == dst) {
        pathOut = {src};
        return true;
    }
    if (!adj.count(src) || !adj.count(dst)) {
        std::cerr << "Either source or destination not found in network.\n";
        return false;
    }

    auto getNeighbors = [this](const std::string &n) -> std::vector<std::string> {
        return net.getNeighbors(n);
    };

    std::optional<std::vector<std::string>> maybePath;

    switch (mode) {
        case PingMode::HOPS:
            maybePath = traversal::bfs_shortest_hops(getNeighbors, src, dst);
            break;
        case PingMode::DELAY: {
            auto getLinkDelay = [this](const std::string &a, const std::string &b) -> int {
                return net.getLinkDelay(a, b);
            };
            maybePath = traversal::dijkstra_shortest_delay(getNeighbors, getLinkDelay, src, dst);
            break;
        }
        case PingMode::DFS:
            maybePath = traversal::dfs_any_path(getNeighbors, src, dst, maxDepth);
            break;
        default:
            maybePath = traversal::bfs_shortest_hops(getNeighbors, src, dst);
    }

    if (!maybePath) {
        std::cout << "Ping failed: no route from " << src << " to " << dst << "\n";
        return false;
    }

    pathOut = *maybePath;
    std::cout << "Path found: ";
    for (const auto &p : pathOut) std::cout << p << " ";
    std::cout << std::endl;

    return true;
}

bool Engine::traceroute(const std::string &src, const std::string &dst, std::vector<std::string> &pathOut) {
    // Use Dijkstra to find path with minimal total delay
    auto getNeighbors = [this](const std::string &n) -> std::vector<std::string> {
        return net.getNeighbors(n);
    };
    auto getLinkDelay = [this](const std::string &a, const std::string &b) -> int {
        return net.getLinkDelay(a, b);
    };

    auto maybePath = traversal::dijkstra_shortest_delay(getNeighbors, getLinkDelay, src, dst);
    if (!maybePath) return false;
    pathOut = *maybePath;
    return true;
}

int Engine::getTotalDelay(const std::vector<std::string>& path) {
    int totalDelay = 0;
    if (path.size() < 2) return 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        totalDelay += net.getLinkDelay(path[i], path[i + 1]);
    }
    return totalDelay;
}

// Multicast - wysyłanie pakietów do wielu odbiorców
bool Engine::multicast(const std::string& srcName, const std::vector<std::string>& destinations) {
    std::cout << "[MULTICAST] From " << srcName << " to " << destinations.size() << " destinations" << std::endl;
    
    if (destinations.empty()) {
        std::cerr << "No destinations specified for multicast\n";
        return false;
    }
    
    // Use single BFS traversal to find paths to all destinations efficiently
    auto getNeighbors = [this](const std::string &n) -> std::vector<std::string> {
        return net.getNeighbors(n);
    };

    auto results = traversal::multicast_bfs(getNeighbors, srcName, destinations);

    bool allSuccessful = true;
    for (const auto &dest : destinations) {
        auto it = results.find(dest);
        if (it == results.end() || it->second.empty()) {
            std::cerr << "Failed to reach destination: " << dest << std::endl;
            allSuccessful = false;
        } else {
            std::cout << "Multicast packet delivered to: " << dest << " via ";
            for (const auto &h : it->second) std::cout << h << " ";
            std::cout << std::endl;
        }
    }

    return allSuccessful;
}

std::vector<std::string> Engine::getEqualCostNextHops(const std::string& node, const std::string& dst) {
    auto getNeighbors = [this](const std::string &n) -> std::vector<std::string> {
        return net.getNeighbors(n);
    };
    auto getLinkDelay = [this](const std::string &a, const std::string &b) -> int {
        return net.getLinkDelay(a, b);
    };
    return traversal::equal_cost_next_hops(getNeighbors, getLinkDelay, node, dst);
}

bool Engine::constrainedShortestPath(const std::string& src,
                                     const std::string& dst,
                                     std::vector<std::string>& pathOut,
                                     int minBandwidth)
{
    auto getNeighbors = [this](const std::string &n) -> std::vector<std::string> {
        return net.getNeighbors(n);
    };
    auto getLinkDelay = [this](const std::string &a, const std::string &b) -> int {
        return net.getLinkDelay(a, b);
    };
    auto getLinkBandwidth = [this](const std::string &a, const std::string &b) -> int {
        return net.getBandwidth(a, b);
    };

    auto maybePath = traversal::constrained_shortest_path(getNeighbors, getLinkDelay, getLinkBandwidth, src, dst, minBandwidth);
    if (!maybePath) return false;
    pathOut = *maybePath;
    return true;
}

std::unordered_map<std::string, std::string> Engine::computeLinkStateRouting(const std::string& node) {
    auto getNeighbors = [this](const std::string &n) -> std::vector<std::string> {
        return net.getNeighbors(n);
    };
    auto getLinkDelay = [this](const std::string &a, const std::string &b) -> int {
        return net.getLinkDelay(a, b);
    };
    return traversal::link_state_routing(getNeighbors, getLinkDelay, node);
}

bool Engine::isRPF(const std::string& node, const std::string& incomingNeighbor, const std::string& src) {
    auto getNeighbors = [this](const std::string &n) -> std::vector<std::string> {
        return net.getNeighbors(n);
    };
    auto getLinkDelay = [this](const std::string &a, const std::string &b) -> int {
        return net.getLinkDelay(a, b);
    };

    bool result = traversal::is_rpf(getNeighbors, getLinkDelay, node, incomingNeighbor, src);
    std::cout << "[RPF] node=" << node << " incoming=" << incomingNeighbor << " src=" << src << " -> " << (result?"ACCEPT":"REJECT") << std::endl;
    return result;
}

std::vector<std::pair<std::vector<std::string>, double>> Engine::multipathFlowAware(const std::string& src,
                                                                                     const std::string& dst,
                                                                                     int k)
{
    auto getNeighbors = [this](const std::string &n) -> std::vector<std::string> {
        return net.getNeighbors(n);
    };
    auto getLinkDelay = [this](const std::string &a, const std::string &b) -> int {
        return net.getLinkDelay(a, b);
    };
    auto getLinkBandwidth = [this](const std::string &a, const std::string &b) -> int {
        return net.getBandwidth(a, b);
    };

    auto paths = traversal::multipath_flow_aware(getNeighbors, getLinkDelay, getLinkBandwidth, src, dst, k);
    std::cout << "[MULTIPATH] Found " << paths.size() << " candidate(s) from " << src << " to " << dst << std::endl;
    for (size_t i = 0; i < paths.size(); ++i) {
        std::cout << "  path[" << i << "] weight=" << paths[i].second << " route:";
        for (auto &h : paths[i].first) std::cout << " " << h;
        std::cout << std::endl;
    }
    return paths;
}

std::unordered_map<std::string, std::vector<std::pair<std::vector<std::string>, int>>> Engine::multiCommodityFlow(
    const std::vector<std::tuple<std::string, std::string, int>>& commodities,
    int k)
{
    auto getNeighbors = [this](const std::string &n) -> std::vector<std::string> {
        return net.getNeighbors(n);
    };
    auto getLinkDelay = [this](const std::string &a, const std::string &b) -> int {
        return net.getLinkDelay(a, b);
    };
    auto getLinkBandwidth = [this](const std::string &a, const std::string &b) -> int {
        return net.getBandwidth(a, b);
    };

    auto alloc = traversal::multi_commodity_flow(getNeighbors, getLinkDelay, getLinkBandwidth, commodities, k);
    std::cout << "[MULTI-COMMODITY] computed allocations for " << alloc.size() << " commodity keys" << std::endl;
    for (const auto &entry : alloc) {
        std::cout << "  commodity=" << entry.first << " allocations=" << entry.second.size() << std::endl;
    }
    return alloc;
}
