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
    void setIp(const std::string& newIp) { ip = newIp; }
    virtual std::string getType() const { return "node"; }

    void setMTU(int newMtu) { mtu = newMtu; }
    int getMTU() const { return mtu; }
    void setMaxQueueSize(int size) { maxQueueSize = size; }
    int getMaxQueueSize() const { return maxQueueSize; }

    virtual void receivePacket(Packet& p) = 0;
    void sendPacket(Packet& p, Node& dest);
    int getAllPacketCount() const { return packetCount; }
    int getPacketCountByNeighbor(const std::string& neighborName) const;
    void incrementPacketCount() { packetCount++; }
    void incrementPacketCountToNeighbor(const std::string& neighborName);

    void addNeighbor(Node* neighbor);
    void enqueuePacket(const Packet& pkt) { if (packetQueue.size() < maxQueueSize) packetQueue.push(pkt); }
    void dequeuePacket() { if (!packetQueue.empty()) packetQueue.pop(); }
    bool isCongested() const { return packetQueue.size() >= maxQueueSize; }


    int getPacketCount() const { return packetCount; }
    protected:
    std::string name;
    std::string ip;
    int mtu = 1500;
    Packet packet;
    std::vector<Node*> connections;
    std::map<std::string, int> packetCountByNeighbor;
    mutable int packetCount = 0;
    std::queue<Packet> packetQueue; // For queueing packets
    int maxQueueSize = 10;
    
};
