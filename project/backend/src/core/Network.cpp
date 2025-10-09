#include "Network.hpp"


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

void Network::removeNode(const std::string &name)
{
    auto nodeIt = nodesByName.find(name);
    if (nodeIt == nodesByName.end())
        throw std::runtime_error("Node not found: " + name);

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

    auto adj = getAdjacency();
    if (adj.find(a->getName()) == adj.end() || adj.find(b->getName()) == adj.end())
        throw std::runtime_error("One or both nodes have no connections");
    if (adj.at(a->getName()).find(b->getName()) == adj.at(a->getName()).end())
        throw std::runtime_error("Nodes are not connected");

    adj[a->getName()].erase(b->getName());
    adj[b->getName()].erase(a->getName());
}
