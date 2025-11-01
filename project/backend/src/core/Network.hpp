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

// Struktura dla statystyk ruchu sieciowego
struct TrafficStats {
    std::map<std::string, int> nodePacketsSent;      // węzeł -> liczba wysłanych pakietów
    std::map<std::string, int> nodePacketsReceived;  // węzeł -> liczba odebranych pakietów
    std::map<std::pair<std::string, std::string>, int> linkTraffic; // para węzłów -> ruch między nimi
    int totalPackets = 0;                            // całkowita liczba pakietów w sieci
    double averagePacketsPerNode = 0.0;              // średnia liczba pakietów na węzeł
};

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
    void disconnect(const std::string& nameA, const std::string& nameB);
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
    
    // links
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

    // Time-Based Simulation
    void advanceTime(int ms);
    void schedulePacketDelivery(const Packet& pkt, int delay);
    bool hasPacketArrived(const std::string& node) const;

    void connectWirelessRange(const std::string& nameA, const std::string& nameB, int range);
    void connectWireless(const std::string& nameA, const std::string& nameB);
    void setWirelessRange(const std::string& name, int range);
    bool isWirelessConnected(const std::string& nameA, const std::string& nameB) const;
    void simulateInterference(const std::string& name, double lossProb);

    // Network Statistics
    void recordPacketSent(const std::string& nodeName);
    void recordPacketReceived(const std::string& nodeName);
    int getPacketsSent(const std::string& nodeName) const;
    int getPacketsReceived(const std::string& nodeName) const;
    int getTotalPacketsSent() const;
    int getTotalPacketsReceived() const;
    void resetNodeStatistics(const std::string& nodeName);
    void resetAllStatistics();
    std::string getMostActiveNode() const;
    
    // Traffic Monitoring
    TrafficStats getTrafficStats() const;
    void recordLinkTraffic(const std::string& nodeA, const std::string& nodeB);
    
    // Cloud Integration
    void addCloudNode(const std::string& name, const std::string& ip);
    std::vector<std::string> getCloudNodes() const;
    void scaleUp(const std::string& cloudName);
    void scaleDown(const std::string& cloudName);
    
    // IoT Devices
    void addIoTDevice(const std::string& name, const std::string& ip);
    bool hasIoTDevice(const std::string& name) const;
    void simulateBatteryDrain(const std::string& name, int percent);
    int getBatteryLevel(const std::string& name) const;
    
    // Performance Metrics
    int getLatency(const std::string& nameA, const std::string& nameB) const;
    int getThroughput(const std::string& nameA, const std::string& nameB) const;
    double getPacketLossRate(const std::string& nameA, const std::string& nameB) const;

    // Database Persistence
    void enablePersistence(const std::string& dbHost = "127.0.0.1", 
                          int dbPort = 3306,
                          const std::string& dbUser = "root",
                          const std::string& dbPassword = "DevPassword123!",
                          const std::string& dbName = "netsim");
    bool saveTopologyToDB();
    bool loadTopologyFromDB();
    void disablePersistence();
    bool isPersistenceEnabled() const;

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
    int currentTime = 0; // current simulation time
    std::map<int, std::vector<Packet>> scheduledPackets; // time -> packets to deliver
    std::map<std::string, bool> arrivedPackets; // node -> has packet arrived
    std::map<std::pair<std::string, std::string>, int> wirelessRanges; // link -> wireless range
    
    // Network Statistics
    std::map<std::string, int> packetsSent; // node -> number of packets sent
    std::map<std::string, int> packetsReceived; // node -> number of packets received
    std::map<std::pair<std::string, std::string>, int> linkTrafficCount; // link traffic counter
    
    // Wireless Networks
    std::map<std::string, int> wirelessNodeRanges; // node -> wireless range
    std::map<std::string, double> interferenceLevel; // node -> interference loss probability
    
    // Cloud Integration
    std::set<std::string> cloudNodes; // cloud node names
    std::map<std::string, std::vector<std::string>> cloudGroups; // cloud group -> instances
    int cloudInstanceCounter = 0; // for unique instance names
    
    // IoT Devices
    std::set<std::string> iotDevices; // IoT device names
    std::map<std::string, int> iotBatteries; // device -> battery level (0-100)
    
    // Database Persistence
    bool persistenceEnabled = false;
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
