#include "Node.hpp"

void Node::sendPacket(Packet& p, Node& dest) {
    incrementPacketCount();
    dest.receivePacket(p);
}

void Node::addNeighbor(Node* neighbor) {
    connections.push_back(neighbor);
}

int Node::getPacketCountByNeighbor(const std::string &neighborName) const
{
    if (packetCountByNeighbor.find(neighborName) != packetCountByNeighbor.end()) {
        return packetCountByNeighbor.at(neighborName);
    }
    return 0;
}

void Node::incrementPacketCountToNeighbor(const std::string& neighborName)
{
    packetCountByNeighbor[neighborName]++;
}

