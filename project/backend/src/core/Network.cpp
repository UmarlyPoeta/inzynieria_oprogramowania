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

