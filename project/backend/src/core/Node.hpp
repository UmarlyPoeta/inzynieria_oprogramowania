#pragma once
#include "Packet.hpp"
#include <vector>


class Node {
public:
    Node() = default;
        Node(const std::string& name, const std::string& ip) : name(name), ip(ip) {}
        virtual ~Node() = default;


    std::string getName() const {return name;}
    std::string getIp() const {return ip;}

    virtual void receivePacket(Packet& p) = 0;
    void sendPacket(Packet& p, Node& dest);

    void addNeighbor(Node* neighbor);


    protected:
    std::string name;
    std::string ip;
    Packet packet;
    std::vector<Node*> connections;
};
