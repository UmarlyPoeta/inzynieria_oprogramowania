#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <set>
#include <stdexcept>
#include <tuple>
#include "Node.hpp"
#include <algorithm>

// DummyNode for testing
class DummyNode : public Node {
public:
    bool received = false;
    DummyNode(const std::string& n, const std::string& ip) : Node(n, ip) {}
    void receivePacket(Packet& pkt) override { received = true; }
};

 // Reprezentuje całą sieć jako graf (node'y + połączenia)
class Network {
public:
    // Tworzy i dodaje nowy węzeł dowolnego typu (Host, Router, itp.)
    template<typename T, typename... Args>
    std::shared_ptr<T> addNode(Args&&... args);

    // Połączenie dwóch node’ów po wskaźnikach
    void connect(std::shared_ptr<Node> a, std::shared_ptr<Node> b);

    // Połączenie dwóch node’ów po nazwie (REST-friendly)
    void connect(const std::string& nameA, const std::string& nameB);

    // Znajdź node po nazwie
    std::shared_ptr<Node> findByName(const std::string& name) const;

    // Lista sąsiadów danego node’a
    std::vector<std::string> getNeighbors(const std::string& name) const;

    // Lista wszystkich node’ów w sieci
    std::vector<std::string> getAllNodes() const;

    // Getter do grafu połączeń
    const std::map<std::string, std::set<std::string>>& getAdjacency() const;

    void removeNode(const std::string& name);
    void disconnect(const std::string& nameA, const std::string& nameB);

    void setLinkDelay(const std::string& nameA, const std::string& nameB, int delayMs);
    int getLinkDelay(const std::string& nameA, const std::string& nameB) const;
    void removeLinkDelay(const std::string& nameA, const std::string& nameB);

    void checkConnectivity(const std::string& nameA, const std::string& nameB) const;
    void getLinkDelays(std::map<std::pair<std::string, std::string>, int>& outDelays) const;

    int getPacketCount(const std::string& nameA, const std::string& nameB);
    void incrementPacketCount(const std::string& nameA, const std::string& nameB);

    // VLAN
    void assignVLAN(const std::string& name, int vlanId);
    bool canCommunicate(const std::string& nameA, const std::string& nameB) const;

    // Bandwidth
    void setBandwidth(const std::string& nameA, const std::string& nameB, int bw);
    int getBandwidth(const std::string& nameA, const std::string& nameB) const;
    void consumeBandwidth(const std::string& nameA, const std::string& nameB, int amount);

    // Firewall
    void addFirewallRule(const std::string& src, const std::string& dst, const std::string& protocol, bool allow);
    bool isAllowed(const std::string& src, const std::string& dst, const std::string& protocol) const;

    // Node failure
    void failNode(const std::string& name);
    bool isFailed(const std::string& name) const;
    void sendPacket(const Packet& pkt);

    // Export/Import
    std::string exportToJson() const;
    void importFromJson(const std::string& jsonStr);

    // Congestion Control
    void setQueueSize(const std::string& name, int size);
    void enqueuePacket(const std::string& name, const Packet& pkt);
    void dequeuePacket(const std::string& name);
    bool isCongested(const std::string& name) const;

    // Packet Loss
    void setPacketLoss(const std::string& nameA, const std::string& nameB, double lossProb);

    // TCP Simulation
    bool initiateTCPConnection(const std::string& client, const std::string& server);
    bool sendTCPPacket(const std::string& src, const std::string& dst, Packet pkt);

    // UDP Simulation
    bool sendUDPPacket(const std::string& src, const std::string& dst, Packet pkt);

private:
    std::vector<std::shared_ptr<Node>> nodes;                // wszystkie węzły
    std::map<std::string, std::shared_ptr<Node>> nodesByName; // szybki lookup
    std::map<std::string, std::set<std::string>> adj;         // graf połączeń
    std::map<std::pair<std::string, std::string>, int> linkDelays; // opóźnienia łączy
    std::map<std::string, int> vlans; // VLAN dla węzłów
    std::map<std::pair<std::string, std::string>, int> bandwidths; // bandwidth między węzłami
    std::map<std::tuple<std::string, std::string, std::string>, bool> firewallRules; // src, dst, protocol -> allow
    std::set<std::string> failedNodes; // failed węzły
    std::map<std::pair<std::string, std::string>, double> packetLoss; // link -> loss probability

};

// Implementacja szablonu w headerze
template<typename T, typename... Args>
std::shared_ptr<T> Network::addNode(Args&&... args) {
    auto node = std::make_shared<T>(std::forward<Args>(args)...);
    const auto& name = node->getName();
    if (nodesByName.count(name))
        throw std::runtime_error("Node already exists: " + name);
    nodes.push_back(node);
    nodesByName[name] = node;
    return node;
}
