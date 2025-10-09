#pragma once
#include "Node.hpp"
#include <map>
#include <iostream>

class Router : public Node {
    std::map<std::string, Node*> routingTable; // IP → next hop

public:
    using Node::Node;

    void addRoute(const std::string& ip, Node* next) {
        routingTable[ip] = next;
    }

    void receivePacket(Packet& p) override;

    void printRoutingTable() const {
        std::cout << "Routing Table for Router " << name << ":\n";
        for (const auto& entry : routingTable) {
            std::cout << "  Destination: " << entry.first << " -> Next Hop: " << entry.second->getName() << "\n";
        }
    }

    void clearRoutingTable() {
        routingTable.clear();
    }

    void routeAll(const std::map<std::string, Node*>& allNodes) {
        for (const auto& entry : allNodes) {
            if (entry.first != ip) { // Avoid routing to self
                routingTable[entry.first] = nullptr; // Placeholder for next hop
            }
        }
    }


    
};
