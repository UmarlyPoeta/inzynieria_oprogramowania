#pragma once
#include "Network.hpp"
#include "Packet.hpp"
#include <string>
#include <vector>

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

private:
    Network &net;
};
