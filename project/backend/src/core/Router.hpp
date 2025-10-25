#pragma once
#include "Node.hpp"
#include <map>
#include <iostream>

class Router : public Node {
    std::map<std::string, Node*> routingTable; // IP → next hop

public:
    using Node::Node;

    Node* getNextHop(const std::string& dst);

    bool forwardPacket(Packet& p) {
        Node* nextHop = getNextHop(p.dest);
        if (nextHop) {
            nextHop->receivePacket(p);
            return true;
        }
        return false; // No route found
    }

    void addRoute(const std::string& ip, Node* next) {
        routingTable[ip] = next;
    }

    Node* getNextHop(const std::string& dst) const {
        auto it = routingTable.find(dst);
        if (it != routingTable.end()) return it->second;
        return nullptr;
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
            if (entry.first != ip) {
                routingTable[entry.first] = nullptr;
            }
        }
    }

    void exchangeRoutingInfo(Router* otherRouter) {
        if (!otherRouter) return;
        for (const auto& entry : routingTable) {
            if (!otherRouter->hasRouteTo(entry.first)) {
                otherRouter->addRoute(entry.first, this);
            }
        }
    }

    bool hasRouteTo(const std::string& dst) const {
        return routingTable.find(dst) != routingTable.end();
    }
    
    // Dynamic Routing - aktualizacja tras
    void updateRoute(const std::string& dst, Node* newNextHop) {
        if (routingTable.find(dst) != routingTable.end()) {
            std::cout << "Updating route to " << dst << " via " << newNextHop->getName() << std::endl;
            routingTable[dst] = newNextHop;
        } else {
            addRoute(dst, newNextHop);
        }
    }
    
    // Load Balancing - wybór następnego węzła na podstawie obciążenia
    std::string getBalancedNextHop(const std::string& dst) {
        // Znajdź wszystkie możliwe trasy do danego celu
        std::vector<Node*> possibleNextHops;
        
        for (const auto& entry : routingTable) {
            // Sprawdź czy trasa prowadzi do sieci docelowej (uproszczona wersja)
            if (entry.first.find(dst.substr(0, dst.find_last_of('.'))) != std::string::npos) {
                if (entry.second != nullptr) {
                    possibleNextHops.push_back(entry.second);
                }
            }
        }
        
        if (possibleNextHops.empty()) {
            return "";
        }
        
        // Wybierz węzeł z najmniejszym obciążeniem (najmniejsza liczba pakietów)
        Node* bestNextHop = possibleNextHops[0];
        int minPackets = bestNextHop->getPacketCount();
        
        for (auto* nextHop : possibleNextHops) {
            if (nextHop->getPacketCount() < minPackets) {
                minPackets = nextHop->getPacketCount();
                bestNextHop = nextHop;
            }
        }
        
        return bestNextHop->getName();
    }
};
