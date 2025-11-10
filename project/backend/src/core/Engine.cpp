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
    
    bool allSuccessful = true;
    
    // Wyślij pakiet do każdego odbiorcy
    for (const auto& dest : destinations) {
        std::vector<std::string> path;
        bool success = ping(srcName, dest, path);
        
        if (!success) {
            std::cerr << "Failed to reach destination: " << dest << std::endl;
            allSuccessful = false;
        } else {
            std::cout << "Multicast packet delivered to: " << dest << std::endl;
        }
    }
    
    return allSuccessful;
}
