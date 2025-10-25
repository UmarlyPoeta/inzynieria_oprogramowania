#pragma once
#include <string>
#include "Node.hpp"
#include "Packet.hpp"
#include <iostream>


class Host : public Node {
    public:
        Packet packet;
        int port;
        std::string address;

        Host(const std::string& name, const std::string& address, int port);
        std::string getAddress() const;
        int getPort() const;
        void setAddress(const std::string& address);
        void setPort(int port);

        void receivePacket(Packet& p) override;
        void sendPacket(Packet& p, Node& dest);

    };