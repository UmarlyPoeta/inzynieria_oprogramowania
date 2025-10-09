#include "Node.hpp"

void Node::sendPacket(Packet& p, Node& dest) {
    dest.receivePacket(p);
}

void Node::addNeighbor(Node* neighbor) {
    connections.push_back(neighbor);
}

