#include "Engine.hpp"
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>

Engine::Engine(Network &network) : net(network) {}

bool Engine::ping(const std::string &src, const std::string &dst, std::vector<std::string> &pathOut) {
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

    std::queue<std::string> q;
    std::map<std::string, std::string> parent;
    std::map<std::string, int> ttl;

    q.push(src);
    ttl[src] = 8;  // start TTL

    bool found = false;

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        if (current == dst) {
            found = true;
            break;
        }

        if (ttl[current] <= 0)
            continue; // pakiet wygasł

        for (const auto &neighbor : adj.at(current)) {
            if (!parent.count(neighbor)) { // nieodwiedzony
                parent[neighbor] = current;
                ttl[neighbor] = ttl[current] - 1;
                q.push(neighbor);
            }
        }
    }

    if (!found) {
        std::cout << "Ping failed: no route from " << src << " to " << dst << "\n";
        return false;
    }

    // Odtwórz ścieżkę
    std::vector<std::string> path;
    for (auto at = dst; !at.empty(); at = parent.count(at) ? parent[at] : "") {
        path.push_back(at);
        if (at == src) break;
    }

    std::reverse(path.begin(), path.end());
    pathOut = path;

    std::cout << "Path found: ";
    for (auto &p : path) std::cout << p << " ";
    std::cout << std::endl;

    return true;
}

bool Engine::traceroute(const std::string &src, const std::string &dst, std::vector<std::string> &pathOut) {
    return ping(src, dst, pathOut); // For simplicity, same as ping
}
