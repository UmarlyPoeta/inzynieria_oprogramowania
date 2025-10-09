#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <set>
#include <stdexcept>
#include "Node.hpp"
#include <algorithm>

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

    void Network::removeNode(const std::string& name);
    void Network::disconnect(const std::string& nameA, const std::string& nameB);

    void setLinkDelay(const std::string& nameA, const std::string& nameB, int delayMs);
    int getLinkDelay(const std::string& nameA, const std::string& nameB) const;
    void removeLinkDelay(const std::string& nameA, const std::string& nameB);

    void checkConnectivity(const std::string& nameA, const std::string& nameB) const;
    void getLinkDelays(std::map<std::pair<std::string, std::string>, int>& outDelays) const;

private:
    std::vector<std::shared_ptr<Node>> nodes;                // wszystkie węzły
    std::map<std::string, std::shared_ptr<Node>> nodesByName; // szybki lookup
    std::map<std::string, std::set<std::string>> adj;         // graf połączeń
    std::map<std::pair<std::string, std::string>, int> linkDelays; // opóźnienia łączy

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
