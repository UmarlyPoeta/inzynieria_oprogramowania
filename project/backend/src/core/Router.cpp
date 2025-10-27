#include "Router.hpp"

Node *Router::getNextHop(const std::string &dst)
{
    auto it = routingTable.find(dst);
    if (it != routingTable.end()) return it->second;
    return nullptr;
}

void Router::receivePacket(Packet &p)
{
    std::cout << "[ROUTER " << name << "] Received packet destined for " << p.dest << std::endl;

    if (p.ttl > 0) {
        p.ttl--;
    } else {
        std::cout << "[ROUTER " << name << "] Packet TTL expired.\n";
        return;
    }

    auto it = routingTable.find(p.dest);
    if (it != routingTable.end()) {
        Node* nextHop = it->second;
        std::cout << "[ROUTER " << name << "] Forwarding packet to next hop: " << nextHop->getName() << std::endl;
        sendPacket(p, *nextHop);
    } else {
        std::cout << "[ROUTER " << name << "] No route to destination: " << p.dest << std::endl;
    }
}