#include "Network.hpp"
#include <nlohmann/json.hpp>
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
