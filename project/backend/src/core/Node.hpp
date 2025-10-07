#pragma once
#include "Packet.hpp"


class Node {
    Node() = default;
    Node(const std::string& name, const std::string& ip) : name(name), ip(ip) {}
    virtual ~Node() = default;


    std::string getName() const {return name;}
    std::string getIp() const {return ip;}

    virtual void receivePacket(const Packet& p) = 0;

    protected:
    std::string name;
    std::string ip;
    Packet packet;
};
