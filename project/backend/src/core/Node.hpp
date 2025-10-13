#pragma once
#include "Packet.hpp"
#include <map>
#include <vector>
#include <string>
#include <queue>


class Node {
public:
    Node() = default;
    Node(const std::string& name, const std::string& ip) : name(name), ip(ip) {}
    virtual ~Node() = default;

    std::string getName() const {return name;}
    std::string getIp() const {return ip;}

    virtual void receivePacket(Packet& p) = 0;
    void sendPacket(Packet& p, Node& dest);
    int getAllPacketCount() const { return packetCount; }
    int getPacketCountByNeighbor(const std::string& neighborName) const;
    void incrementPacketCount() { packetCount++; }
    void incrementPacketCountToNeighbor(const std::string& neighborName);

    void addNeighbor(Node* neighbor);
    void setQueueSize(int size) { queuesize = size; }
    void enqueuePacket(const Packet& pkt) { if (packetQueue.size() < queuesize) packetQueue.push(pkt); }
    void dequeuePacket() { if (!packetQueue.empty()) packetQueue.pop(); }
    bool isCongested() const { return packetQueue.size() >= queuesize; }


    int getPacketCount() const { return packetCount; }
    int getQueueSize() const { return queuesize; }
    protected:
    std::string name;
    std::string ip;
    Packet packet;
    std::vector<Node*> connections;
    std::map<std::string, int> packetCountByNeighbor;
    mutable int packetCount = 0;
    std::queue<Packet> packetQueue; // For queueing packets
    int queuesize = 10;
    
};
