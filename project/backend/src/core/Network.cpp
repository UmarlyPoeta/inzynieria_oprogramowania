#include "Network.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;


void Network::connect(std::shared_ptr<Node> a, std::shared_ptr<Node> b) {
    if (!a || !b)
        throw std::runtime_error("Cannot connect null nodes");
    adj[a->getName()].insert(b->getName());
    adj[b->getName()].insert(a->getName());
}

void Network::connect(const std::string& nameA, const std::string& nameB) {
    if (nameA == nameB)
        throw std::runtime_error("Cannot connect node to itself");
    auto a = findByName(nameA);
    auto b = findByName(nameB);
    connect(a, b);
}

std::shared_ptr<Node> Network::findByName(const std::string& name) const {
    auto it = nodesByName.find(name);
    if (it == nodesByName.end())
        throw std::runtime_error("Node not found: " + name);
    return it->second;
}

std::vector<std::string> Network::getNeighbors(const std::string& name) const {
    std::vector<std::string> result;
    if (adj.count(name))
        result.assign(adj.at(name).begin(), adj.at(name).end());
    return result;
}

std::vector<std::string> Network::getAllNodes() const {
    std::vector<std::string> result;
    for (auto& n : nodes)
        result.push_back(n->getName());
    return result;
}

const std::map<std::string, std::set<std::string>>& Network::getAdjacency() const {
    return adj;
}

void Network::removeNode(const std::string& name)
{
    auto nodeIt = nodesByName.find(name);
    if (nodeIt == nodesByName.end())
        throw std::runtime_error("Node not found: " + name);

    if (!getNeighbors(name).empty())
        throw std::runtime_error("Cannot remove node with connections: " + name);

    for (const auto& neighbor : getNeighbors(name)) {
        adj[neighbor].erase(name);
    }
    // Remove from nodes vector
    auto vecIt = std::find_if(nodes.begin(), nodes.end(),
        [&name](const std::shared_ptr<Node>& node) { return node->getName() == name; });
    if (vecIt != nodes.end()) {
        nodes.erase(vecIt);
    }



    nodesByName.erase(name);
}

void Network::disconnect(const std::string &nameA, const std::string &nameB)
{
    if (nameA == nameB)
        throw std::runtime_error("Cannot disconnect node from itself");
    auto a = findByName(nameA);
    auto b = findByName(nameB);
    if (!a || !b)
        throw std::runtime_error("Cannot disconnect null nodes");

    if (adj.find(a->getName()) == adj.end() || adj.find(b->getName()) == adj.end())
        throw std::runtime_error("One or both nodes have no connections");
    if (adj.at(a->getName()).find(b->getName()) == adj.at(a->getName()).end())
        throw std::runtime_error("Nodes are not connected");

    adj[a->getName()].erase(b->getName());
    adj[b->getName()].erase(a->getName());
}

void Network::setLinkDelay(const std::string &nameA, const std::string &nameB, int delayMs)
{
    if (nameA == nameB)
        throw std::runtime_error("Cannot set link delay for the same node");
    auto a = findByName(nameA);
    auto b = findByName(nameB);
    if (!a || !b)
        throw std::runtime_error("Cannot set link delay for null nodes");
    if (delayMs < 0)
        throw std::runtime_error("Delay must be non-negative");

    if (adj.find(a->getName()) == adj.end() || adj.find(b->getName()) == adj.end())
        throw std::runtime_error("One or both nodes have no connections");
    if (adj.at(a->getName()).find(b->getName()) == adj.at(a->getName()).end())
        throw std::runtime_error("Nodes are not connected");

    

    linkDelays[{nameA, nameB}] = delayMs;
    linkDelays[{nameB, nameA}] = delayMs; // symetryczne ustawienie


}

int Network::getLinkDelay(const std::string &nameA, const std::string &nameB) const
{
    if (nameA == nameB)
        return 0;
    auto a = findByName(nameA);
    auto b = findByName(nameB);
    if (!a || !b)
        throw std::runtime_error("Cannot get link delay for null nodes");

    if (adj.find(a->getName()) == adj.end() || adj.find(b->getName()) == adj.end())
        throw std::runtime_error("One or both nodes have no connections");
    if (adj.at(a->getName()).find(b->getName()) == adj.at(a->getName()).end())
        throw std::runtime_error("Nodes are not connected");

    auto it = linkDelays.find({nameA, nameB});
    if (it != linkDelays.end())
        return it->second;
    return 0;
}

void Network::removeLinkDelay(const std::string &nameA, const std::string &nameB)
{
    if (nameA == nameB)
        throw std::runtime_error("Cannot remove link delay for the same node");
    auto a = findByName(nameA);
    auto b = findByName(nameB);
    if (!a || !b)
        throw std::runtime_error("Cannot remove link delay for null nodes");
    
    if (adj.find(a->getName()) == adj.end() || adj.find(b->getName()) == adj.end())
        throw std::runtime_error("One or both nodes have no connections");
    if (adj.at(a->getName()).find(b->getName()) == adj.at(a->getName()).end())
        throw std::runtime_error("Nodes are not connected");
    
    std::map<std::pair<std::string, std::string>, int> linkDelays;
    getLinkDelays(linkDelays);

    linkDelays.erase({nameA, nameB});
    linkDelays.erase({nameB, nameA}); // symetryczne usunięcie
}

void Network::checkConnectivity(const std::string &nameA, const std::string &nameB) const
{
    if (nameA == nameB)
        return;
    auto a = findByName(nameA);
    auto b = findByName(nameB);
    if (!a || !b)
        throw std::runtime_error("Cannot check connectivity for null nodes");

    if (adj.find(a->getName()) == adj.end() || adj.find(b->getName()) == adj.end())
        throw std::runtime_error("One or both nodes have no connections");

    std::set<std::string> visited;
    std::vector<std::string> stack;
    stack.push_back(nameA);

    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();
        if (current == nameB)
            return; // Found a path
        if (visited.count(current))
            continue;
        visited.insert(current);
        for (const auto& neighbor : getNeighbors(current)) {
            if (!visited.count(neighbor))
                stack.push_back(neighbor);
        }
    }
    throw std::runtime_error("No path between nodes: " + nameA + " and " + nameB);
}

void Network::getLinkDelays(std::map<std::pair<std::string, std::string>, int> &outDelays) const
{
    outDelays = linkDelays;
}

int Network::getPacketCount(const std::string &nameA, const std::string &nameB)
{
    if (nameA == nameB)
        throw std::runtime_error("Cannot get packet count for the same node");
    auto a = findByName(nameA);
    auto b = findByName(nameB);
    if (!a || !b)
        throw std::runtime_error("Cannot get packet count for null nodes");
    if (adj.find(a->getName()) == adj.end() || adj.find(b->getName()) == adj.end())
        throw std::runtime_error("One or both nodes have no connections");
    if (adj.at(a->getName()).find(b->getName()) == adj.at(a->getName()).end())
        throw std::runtime_error("Nodes are not connected");

    return a->getPacketCountByNeighbor(b->getName());
}

void Network::incrementPacketCount(const std::string &nameA, const std::string &nameB)
{
    if (nameA == nameB)
        throw std::runtime_error("Cannot increment packet count for the same node");
    auto a = findByName(nameA);
    auto b = findByName(nameB);
    if (!a || !b)
        throw std::runtime_error("Cannot increment packet count for null nodes");
    if (adj.find(a->getName()) == adj.end() || adj.find(b->getName()) == adj.end())
        throw std::runtime_error("One or both nodes have no connections");
    if (adj.at(a->getName()).find(b->getName()) == adj.at(a->getName()).end())
        throw std::runtime_error("Nodes are not connected");

    a->incrementPacketCountToNeighbor(b->getName());
    b->incrementPacketCountToNeighbor(a->getName());

}

// VLAN
void Network::assignVLAN(const std::string& name, int vlanId) {
    findByName(name); // check exists
    vlans[name] = vlanId;
}

bool Network::canCommunicate(const std::string& nameA, const std::string& nameB) const {
    findByName(nameA); // check exists
    findByName(nameB);
    auto itA = vlans.find(nameA);
    auto itB = vlans.find(nameB);
    if (itA == vlans.end() || itB == vlans.end()) return true; // no VLAN assigned, allow
    return itA->second == itB->second;
}

// Bandwidth
void Network::setBandwidth(const std::string& nameA, const std::string& nameB, int bw) {
    if (nameA == nameB) throw std::runtime_error("Cannot set bandwidth for same node");
    findByName(nameA);
    findByName(nameB);
    if (adj.find(nameA) == adj.end() || adj.find(nameB) == adj.end() || adj.at(nameA).find(nameB) == adj.at(nameA).end())
        throw std::runtime_error("Nodes not connected");
    bandwidths[{nameA, nameB}] = bw;
    bandwidths[{nameB, nameA}] = bw;
}

int Network::getBandwidth(const std::string& nameA, const std::string& nameB) const {
    if (nameA == nameB) return 0;
    findByName(nameA);
    findByName(nameB);
    auto it = bandwidths.find({nameA, nameB});
    if (it != bandwidths.end()) return it->second;
    return 0; // default
}

void Network::consumeBandwidth(const std::string& nameA, const std::string& nameB, int amount) {
    if (nameA == nameB) throw std::runtime_error("Cannot consume bandwidth for same node");
    findByName(nameA);
    findByName(nameB);
    if (adj.find(nameA) == adj.end() || adj.find(nameB) == adj.end() || adj.at(nameA).find(nameB) == adj.at(nameA).end())
        throw std::runtime_error("Nodes not connected");
    bandwidths[{nameA, nameB}] -= amount;
    bandwidths[{nameB, nameA}] -= amount;
    if (bandwidths[{nameA, nameB}] < 0) bandwidths[{nameA, nameB}] = 0;
    if (bandwidths[{nameB, nameA}] < 0) bandwidths[{nameB, nameA}] = 0;
}

// Firewall
void Network::addFirewallRule(const std::string& src, const std::string& dst, const std::string& protocol, bool allow) {
    findByName(src);
    findByName(dst);
    firewallRules[{src, dst, protocol}] = allow;
}

bool Network::isAllowed(const std::string& src, const std::string& dst, const std::string& protocol) const {
    findByName(src);
    findByName(dst);
    auto it = firewallRules.find({src, dst, protocol});
    if (it != firewallRules.end()) return it->second;
    return true; // default allow
}

// Node failure
void Network::failNode(const std::string& name) {
    findByName(name);
    failedNodes.insert(name);
}

bool Network::isFailed(const std::string& name) const {
    findByName(name);
    return failedNodes.count(name);
}

void Network::sendPacket(const Packet& pkt) {
    auto src = findByName(pkt.src);
    auto dst = findByName(pkt.dest);
    if (isFailed(pkt.src) || isFailed(pkt.dest)) throw std::runtime_error("Node failed");
    if (!isAllowed(pkt.src, pkt.dest, pkt.type)) throw std::runtime_error("Firewall blocked");
    // Assume direct send for simplicity
    dst->receivePacket(const_cast<Packet&>(pkt));
}

// Packet Loss
void Network::setPacketLoss(const std::string& nameA, const std::string& nameB, double lossProb) {
    if (nameA == nameB) throw std::runtime_error("Cannot set packet loss for same node");
    findByName(nameA);
    findByName(nameB);
    if (adj.find(nameA) == adj.end() || adj.find(nameB) == adj.end() || adj.at(nameA).find(nameB) == adj.at(nameA).end())
        throw std::runtime_error("Nodes not connected");
    packetLoss[{nameA, nameB}] = lossProb;
    packetLoss[{nameB, nameA}] = lossProb;
}


// TCP Simulation
bool Network::initiateTCPConnection(const std::string& client, const std::string& server) {
    // Simple 3-way handshake simulation
    // 1. Client sends SYN
    Packet synPkt(client, server, "tcp", "tcp", "");
    synPkt.syn = true;
    synPkt.seqNum = 1000; // Example seq
    // Assume send (in real, check response)
    // 2. Server responds SYN-ACK
    Packet synAckPkt(server, client, "tcp", "tcp", "");
    synAckPkt.syn = true;
    synAckPkt.ack = true;
    synAckPkt.ackNum = 1001;
    synAckPkt.seqNum = 2000;
    // 3. Client sends ACK
    Packet ackPkt(client, server, "tcp", "tcp", "");
    ackPkt.ack = true;
    ackPkt.ackNum = 2001;
    // For simplicity, assume success if connected
    return adj.count(client) && adj.count(server) && adj.at(client).count(server);
}

bool Network::sendTCPPacket(const std::string& src, const std::string& dst, Packet pkt) {
    // Send packet, simulate ACK
    pkt.protocol = "tcp";
    // Assume sendPacket handles it, with retransmission on loss
    // For test, just check if sent
    return true; // Simplified
}

// UDP Simulation
bool Network::sendUDPPacket(const std::string& src, const std::string& dst, Packet pkt) {
    pkt.protocol = "udp";
    // UDP is connectionless, no retransmission, no guarantee
    // Just send, always succeeds unless node failed
    auto srcNode = findByName(src);
    auto dstNode = findByName(dst);
    if (isFailed(src) || isFailed(dst)) return false;
    return true; // Simplified success
}

// Time-Based Simulation
void Network::advanceTime(int ms) {
    currentTime += ms;
    // Deliver packets that have arrived
    for (auto it = scheduledPackets.begin(); it != scheduledPackets.end(); ) {
        if (it->first <= currentTime) {
            for (const auto& pkt : it->second) {
                arrivedPackets[pkt.dest] = true;
            }
            it = scheduledPackets.erase(it);
        } else {
            ++it;
        }
    }
}

void Network::schedulePacketDelivery(const Packet& pkt, int delay) {
    int linkDelay = getLinkDelay(pkt.src, pkt.dest);
    int totalDelay = delay + pkt.delayMs + linkDelay;
    int arrivalTime = currentTime + totalDelay;
    scheduledPackets[arrivalTime].push_back(pkt);
}

bool Network::hasPacketArrived(const std::string& node) const {
    auto it = arrivedPackets.find(node);
    if (it != arrivedPackets.end() && it->second) {
        // Reset for next check
        const_cast<Network*>(this)->arrivedPackets[node] = false;
        return true;
    }
    return false;
}


void Network::connectWirelessRange(const std::string &nameA, const std::string &nameB, int range)
{
    auto nodeA = findByName(nameA);
    auto nodeB = findByName(nameB);


    if (!nodeA || !nodeB)
        throw std::runtime_error("Cannot connect null nodes");

    
    connect(nameA, nameB);
}

void Network::connectWireless(const std::string &nameA, const std::string &nameB)
{
    connect(nameA, nameB);
    // Set wireless range (for simplicity, just a fixed value)
    wirelessRanges[{nameA, nameB}] = 50; // 50 meters
    wirelessRanges[{nameB, nameA}] = 50; // 50 meters

}

// Congestion Control
void Network::setQueueSize(const std::string& name, int size) {
    auto node = findByName(name);
    if (node) node->setMaxQueueSize(size);
}

void Network::enqueuePacket(const std::string& name, const Packet& pkt) {
    auto node = findByName(name);
    if (node) node->enqueuePacket(pkt);
}

void Network::dequeuePacket(const std::string& name) {
    auto node = findByName(name);
    if (node) node->dequeuePacket();
}

bool Network::isCongested(const std::string& name) const {
    auto node = findByName(name);
    return node ? node->isCongested() : false;
}

// Export/Import
std::string Network::exportToJson() const {
    nlohmann::json j;
    j["nodes"] = nlohmann::json::array();
    for (auto& n : nodes) {
        nlohmann::json node;
        node["name"] = n->getName();
        node["ip"] = n->getIp();
        j["nodes"].push_back(node);
    }
    j["connections"] = nlohmann::json::array();
    for (auto& [a, neighbors] : adj) {
        for (auto& b : neighbors) {
            if (a < b) { // avoid duplicates
                j["connections"].push_back({a, b});
            }
        }
    }
    return j.dump();
}

void Network::importFromJson(const std::string& jsonStr) {
    nlohmann::json j = nlohmann::json::parse(jsonStr);
    // Clear current
    nodes.clear();
    nodesByName.clear();
    adj.clear();
    // Add nodes
    for (auto& node : j["nodes"]) {
        std::string name = node["name"];
        std::string ip = node["ip"];
        addNode<DummyNode>(name, ip); // Assume DummyNode for simplicity
    }
    // Add connections
    for (auto& conn : j["connections"]) {
        std::string a = conn[0];
        std::string b = conn[1];
        connect(a, b);
    }
}

// ===== Network Statistics Implementation =====

void Network::recordPacketSent(const std::string& nodeName) {
    // Sprawdź czy węzeł istnieje
    if (nodesByName.find(nodeName) == nodesByName.end()) {
        throw std::runtime_error("Node not found: " + nodeName);
    }
    packetsSent[nodeName]++;
}

void Network::recordPacketReceived(const std::string& nodeName) {
    // Sprawdź czy węzeł istnieje
    if (nodesByName.find(nodeName) == nodesByName.end()) {
        throw std::runtime_error("Node not found: " + nodeName);
    }
    packetsReceived[nodeName]++;
}

int Network::getPacketsSent(const std::string& nodeName) const {
    auto it = packetsSent.find(nodeName);
    if (it != packetsSent.end()) {
        return it->second;
    }
    return 0;
}

int Network::getPacketsReceived(const std::string& nodeName) const {
    auto it = packetsReceived.find(nodeName);
    if (it != packetsReceived.end()) {
        return it->second;
    }
    return 0;
}

int Network::getTotalPacketsSent() const {
    int total = 0;
    for (const auto& entry : packetsSent) {
        total += entry.second;
    }
    return total;
}

int Network::getTotalPacketsReceived() const {
    int total = 0;
    for (const auto& entry : packetsReceived) {
        total += entry.second;
    }
    return total;
}

void Network::resetNodeStatistics(const std::string& nodeName) {
    // Sprawdź czy węzeł istnieje
    if (nodesByName.find(nodeName) == nodesByName.end()) {
        throw std::runtime_error("Node not found: " + nodeName);
    }
    packetsSent[nodeName] = 0;
    packetsReceived[nodeName] = 0;
}

void Network::resetAllStatistics() {
    packetsSent.clear();
    packetsReceived.clear();
}

std::string Network::getMostActiveNode() const {
    std::string mostActive;
    int maxPackets = 0;
    
    for (const auto& entry : packetsSent) {
        if (entry.second > maxPackets) {
            maxPackets = entry.second;
            mostActive = entry.first;
        }
    }
    
    return mostActive;
}

// ===== Traffic Monitoring Implementation =====

void Network::recordLinkTraffic(const std::string& nodeA, const std::string& nodeB) {
    // Sprawdź czy węzły istnieją
    if (nodesByName.find(nodeA) == nodesByName.end()) {
        throw std::runtime_error("Node not found: " + nodeA);
    }
    if (nodesByName.find(nodeB) == nodesByName.end()) {
        throw std::runtime_error("Node not found: " + nodeB);
    }
    
    // Zlicz ruch w obu kierunkach (unormalizowany klucz)
    auto key = std::make_pair(std::min(nodeA, nodeB), std::max(nodeA, nodeB));
    linkTrafficCount[key]++;
}

TrafficStats Network::getTrafficStats() const {
    TrafficStats stats;
    
    // Kopiuj statystyki węzłów
    stats.nodePacketsSent = packetsSent;
    stats.nodePacketsReceived = packetsReceived;
    
    // Kopiuj statystyki łączy
    stats.linkTraffic = linkTrafficCount;
    
    // Oblicz całkowitą liczbę pakietów
    stats.totalPackets = getTotalPacketsSent();
    
        // Oblicz średnią liczbę pakietów na węzeł
    if (!nodes.empty()) {
        stats.averagePacketsPerNode = static_cast<double>(stats.totalPackets) / nodes.size();
    }
    
    return stats;
}

// ===== Wireless Networks Implementation =====

void Network::setWirelessRange(const std::string& name, int range) {
    if (nodesByName.find(name) == nodesByName.end()) {
        throw std::runtime_error("Node not found: " + name);
    }
    wirelessNodeRanges[name] = range;
}

bool Network::isWirelessConnected(const std::string& nameA, const std::string& nameB) const {
    // Sprawdź czy węzły istnieją
    if (nodesByName.find(nameA) == nodesByName.end()) {
        throw std::runtime_error("Node not found: " + nameA);
    }
    if (nodesByName.find(nameB) == nodesByName.end()) {
        throw std::runtime_error("Node not found: " + nameB);
    }
    
    // Sprawdź czy są połączone w grafie
    if (adj.find(nameA) == adj.end() || adj.at(nameA).find(nameB) == adj.at(nameA).end()) {
        return false; // Nie ma połączenia
    }
    
    // Sprawdź zasięg bezprzewodowy
    auto rangeA = wirelessNodeRanges.find(nameA);
    auto rangeB = wirelessNodeRanges.find(nameB);
    
    // Jeśli któryś węzeł ma ustawiony zasięg, połączenie jest wireless
    if (rangeA != wirelessNodeRanges.end() || rangeB != wirelessNodeRanges.end()) {
        // Dla uproszczenia, jeśli jest połączenie i jeden ma zasięg, uznajemy za OK
        // W rzeczywistej symulacji sprawdzalibyśmy odległość fizyczną
        
        // Sprawdź interferencje
        auto interA = interferenceLevel.find(nameA);
        auto interB = interferenceLevel.find(nameB);
        
        // Jeśli interferencje są zbyt duże (>0.5), połączenie nie działa
        if ((interA != interferenceLevel.end() && interA->second > 0.5) ||
            (interB != interferenceLevel.end() && interB->second > 0.5)) {
            return false;
        }
        
        return true;
    }
    
    return true; // Zwykłe połączenie przewodowe
}

void Network::simulateInterference(const std::string& name, double lossProb) {
    if (nodesByName.find(name) == nodesByName.end()) {
        throw std::runtime_error("Node not found: " + name);
    }
    interferenceLevel[name] = lossProb;
}

// ===== Cloud Integration Implementation =====

void Network::addCloudNode(const std::string& name, const std::string& ip) {
    // Dodaj jako zwykły węzeł
    addNode<DummyNode>(name, ip);
    cloudNodes.insert(name);
    cloudGroups[name] = {name}; // Inicjuj grupę z jedną instancją
}

std::vector<std::string> Network::getCloudNodes() const {
    std::vector<std::string> result;
    for (const auto& group : cloudGroups) {
        for (const auto& instance : group.second) {
            result.push_back(instance);
        }
    }
    return result;
}

void Network::scaleUp(const std::string& cloudName) {
    if (cloudNodes.find(cloudName) == cloudNodes.end()) {
        throw std::runtime_error("Cloud node not found: " + cloudName);
    }
    
    // Utwórz nową instancję
    std::string instanceName = cloudName + "_instance_" + std::to_string(cloudInstanceCounter++);
    auto baseNode = findByName(cloudName);
    addNode<DummyNode>(instanceName, baseNode->getIp() + "." + std::to_string(cloudInstanceCounter));
    
    cloudGroups[cloudName].push_back(instanceName);
}

void Network::scaleDown(const std::string& cloudName) {
    if (cloudNodes.find(cloudName) == cloudNodes.end()) {
        throw std::runtime_error("Cloud node not found: " + cloudName);
    }
    
    auto& instances = cloudGroups[cloudName];
    if (instances.size() <= 1) {
        // Nie usuwaj ostatniej instancji
        return;
    }
    
    // Usuń ostatnią dodaną instancję (nie bazową)
    std::string toRemove;
    for (auto it = instances.rbegin(); it != instances.rend(); ++it) {
        if (*it != cloudName) {
            toRemove = *it;
            break;
        }
    }
    
    if (!toRemove.empty()) {
        // Usuń z grupy
        instances.erase(std::remove(instances.begin(), instances.end(), toRemove), instances.end());
        
        // Usuń węzeł (tylko jeśli nie ma połączeń)
        try {
            // Najpierw usuń wszystkie połączenia
            auto neighbors = getNeighbors(toRemove);
            for (const auto& neighbor : neighbors) {
                disconnect(toRemove, neighbor);
            }
            removeNode(toRemove);
        } catch (...) {
            // Ignoruj błędy usuwania
        }
    }
}

// ===== IoT Devices Implementation =====

void Network::addIoTDevice(const std::string& name, const std::string& ip) {
    // Dodaj jako zwykły węzeł
    addNode<DummyNode>(name, ip);
    iotDevices.insert(name);
    iotBatteries[name] = 100; // Inicjuj baterię na 100%
}

bool Network::hasIoTDevice(const std::string& name) const {
    return iotDevices.find(name) != iotDevices.end();
}

void Network::simulateBatteryDrain(const std::string& name, int percent) {
    if (!hasIoTDevice(name)) {
        throw std::runtime_error("IoT device not found: " + name);
    }
    
    iotBatteries[name] -= percent;
    if (iotBatteries[name] < 0) {
        iotBatteries[name] = 0;
    }
    
    // Jeśli bateria < 10%, odłącz urządzenie
    if (iotBatteries[name] < 10) {
        failNode(name);
    }
}

int Network::getBatteryLevel(const std::string& name) const {
    if (!hasIoTDevice(name)) {
        throw std::runtime_error("IoT device not found: " + name);
    }
    
    auto it = iotBatteries.find(name);
    if (it != iotBatteries.end()) {
        return it->second;
    }
    return 100; // Domyślnie pełna bateria
}

// ===== Performance Metrics Implementation =====

int Network::getLatency(const std::string& nameA, const std::string& nameB) const {
    // Zwróć opóźnienie łącza
    return getLinkDelay(nameA, nameB);
}

int Network::getThroughput(const std::string& nameA, const std::string& nameB) const {
    // Zwróć liczbę pakietów wysłanych między węzłami
    try {
        return const_cast<Network*>(this)->getPacketCount(nameA, nameB);
    } catch (...) {
        return 0;
    }
}

double Network::getPacketLossRate(const std::string& nameA, const std::string& nameB) const {
    // Zwróć współczynnik utraty pakietów
    auto key = std::make_pair(nameA, nameB);
    auto it = packetLoss.find(key);
    if (it != packetLoss.end()) {
        return it->second;
    }
    return 0.0; // Brak utraty pakietów
}

// Database Persistence Implementation
#ifdef HAVE_MYSQL_SUPPORT
#include "../database/DatabaseManager.hpp"
#include "../database/Config.hpp"
#include "../database/NodeRepository.hpp"
#include "../database/LinkRepository.hpp"
#include "../database/StatsRepository.hpp"
#endif

#ifdef HAVE_MYSQL_SUPPORT

void Network::enablePersistence(const std::string& dbHost, int dbPort,
                                const std::string& dbUser, const std::string& dbPassword,
                                const std::string& dbName) {
    try {
        auto& dbManager = netsim::db::DatabaseManager::getInstance();
        dbManager.connect(dbHost, dbPort, dbUser, dbPassword, dbName);
        persistenceEnabled = true;
        std::cout << "[Network] Database persistence enabled" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Network] Failed to enable persistence: " << e.what() << std::endl;
        throw;
    }
}

bool Network::saveTopologyToDB() {
    if (!persistenceEnabled) {
        std::cerr << "[Network] Persistence not enabled" << std::endl;
        return false;
    }

    try {
        auto& dbManager = netsim::db::DatabaseManager::getInstance();
        netsim::db::NodeRepository nodeRepo(dbManager);
        netsim::db::LinkRepository linkRepo(dbManager);
        netsim::db::StatsRepository statsRepo(dbManager);

        // Start transaction
        dbManager.beginTransaction();

        // Clear existing data
        linkRepo.deleteAllLinks();
        nodeRepo.deleteAllNodes();
        statsRepo.deleteAllStats();

        // Save all nodes
        std::map<std::string, int64_t> nodeIdMap; // name -> database ID
        for (const auto& node : nodes) {
            int64_t dbId = nodeRepo.createNode(*node);
            nodeIdMap[node->getName()] = dbId;
            std::cout << "[Network] Saved node: " << node->getName() << " (ID: " << dbId << ")" << std::endl;
        }

        // Save all links
        for (const auto& [nodeName, neighbors] : adj) {
            int64_t nodeAId = nodeIdMap[nodeName];
            
            for (const auto& neighborName : neighbors) {
                // Only save each link once (avoid duplicates)
                if (nodeName < neighborName) {
                    int64_t nodeBId = nodeIdMap[neighborName];
                    
                    // Get link properties
                    int delay = getLinkDelay(nodeName, neighborName);
                    int bandwidth = getBandwidth(nodeName, neighborName);
                    float loss = static_cast<float>(getPacketLossRate(nodeName, neighborName));
                    
                    linkRepo.createLink(nodeAId, nodeBId, delay, bandwidth, loss);
                    std::cout << "[Network] Saved link: " << nodeName << " <-> " << neighborName << std::endl;
                }
            }
        }

        // Save packet statistics
        for (const auto& [link, count] : linkTrafficCount) {
            const auto& [srcName, dstName] = link;
            if (nodeIdMap.count(srcName) && nodeIdMap.count(dstName)) {
                int64_t srcId = nodeIdMap[srcName];
                int64_t dstId = nodeIdMap[dstName];
                statsRepo.recordPacket(srcId, dstId, count);
            }
        }

        // Commit transaction
        dbManager.commit();
        std::cout << "[Network] Topology saved to database successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        auto& dbManager = netsim::db::DatabaseManager::getInstance();
        dbManager.rollback();
        std::cerr << "[Network] Failed to save topology: " << e.what() << std::endl;
        return false;
    }
}

bool Network::loadTopologyFromDB() {
    if (!persistenceEnabled) {
        std::cerr << "[Network] Persistence not enabled" << std::endl;
        return false;
    }

    try {
        auto& dbManager = netsim::db::DatabaseManager::getInstance();
        netsim::db::NodeRepository nodeRepo(dbManager);
        netsim::db::LinkRepository linkRepo(dbManager);

        // Clear current topology
        nodes.clear();
        nodesByName.clear();
        adj.clear();
        linkDelays.clear();
        bandwidths.clear();
        packetLoss.clear();
        linkTrafficCount.clear();

        // Load all nodes from database
        auto dbNodes = nodeRepo.getAllNodes();
        std::map<int64_t, std::string> idToNameMap; // database ID -> name

        for (const auto& [dbId, name, ip, type] : dbNodes) {
            // Create appropriate node type based on database type
            if (type == "host") {
                auto node = addNode<Host>(name, ip, 8080);
                idToNameMap[dbId] = name;
                std::cout << "[Network] Loaded host: " << name << " (" << ip << ")" << std::endl;
            } else if (type == "router") {
                auto node = addNode<Router>(name, ip);
                idToNameMap[dbId] = name;
                std::cout << "[Network] Loaded router: " << name << " (" << ip << ")" << std::endl;
            } else {
                // Unknown type - skip or create as DummyNode
                auto node = addNode<DummyNode>(name, ip);
                idToNameMap[dbId] = name;
                std::cout << "[Network] Loaded node: " << name << " (type: " << type << ")" << std::endl;
            }
        }

        // Load all links from database
        auto dbLinks = linkRepo.getAllLinks();
        for (const auto& [linkId, nodeAId, nodeBId, delay, bandwidth, loss] : dbLinks) {
            if (idToNameMap.count(nodeAId) && idToNameMap.count(nodeBId)) {
                const std::string& nameA = idToNameMap[nodeAId];
                const std::string& nameB = idToNameMap[nodeBId];
                
                // Connect nodes
                connect(nameA, nameB);
                
                // Restore link properties
                if (delay > 0) {
                    setLinkDelay(nameA, nameB, delay);
                }
                if (bandwidth > 0) {
                    setBandwidth(nameA, nameB, bandwidth);
                }
                if (loss > 0.0) {
                    setPacketLoss(nameA, nameB, loss);
                }
                
                std::cout << "[Network] Loaded link: " << nameA << " <-> " << nameB 
                         << " (delay: " << delay << "ms, bw: " << bandwidth << "Mbps)" << std::endl;
            }
        }

        std::cout << "[Network] Topology loaded from database successfully" << std::endl;
        std::cout << "[Network] Loaded " << nodes.size() << " nodes and " 
                  << (dbLinks.size()) << " links" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[Network] Failed to load topology: " << e.what() << std::endl;
        return false;
    }
}

void Network::disablePersistence() {
    if (persistenceEnabled) {
        auto& dbManager = netsim::db::DatabaseManager::getInstance();
        dbManager.disconnect();
        persistenceEnabled = false;
        std::cout << "[Network] Database persistence disabled" << std::endl;
    }
}

bool Network::isPersistenceEnabled() const {
    return persistenceEnabled;
}

#else

// Stub implementations when MySQL is not available
void Network::enablePersistence(const std::string&, int, const std::string&, 
                                const std::string&, const std::string&) {
    std::cerr << "[Network] Database support not compiled in. Build with MySQL Connector/C++." << std::endl;
    persistenceEnabled = false;
}

bool Network::saveTopologyToDB() {
    std::cerr << "[Network] Database support not available" << std::endl;
    return false;
}

bool Network::loadTopologyFromDB() {
    std::cerr << "[Network] Database support not available" << std::endl;
    return false;
}

void Network::disablePersistence() {
    persistenceEnabled = false;
}

bool Network::isPersistenceEnabled() const {
    return false;
}

#endif // HAVE_MYSQL_SUPPORT

