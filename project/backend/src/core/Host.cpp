#include "Host.hpp"

Host::Host(const std::string& name, const std::string& address, int port)
    : Node(name, address), address(address), port(port) {}

std::string Host::getAddress() const {
    return address;
}

int Host::getPort() const {
    return port;
}

void Host::setAddress(const std::string& address) {
    this->address = address;
}

void Host::setPort(int port) {
    this->port = port;
}

void Host::receivePacket(Packet& p) {
    // TODO: Implement packet handling logic here
    if (p.dest == address) {
        packet = p; // Store the received packet
        // Further processing can be done here
        if (packet.ttl > 0) {
            packet.ttl--;
        }
        if (packet.ttl == 0) {
            // Handle TTL expiration if needed
            std::cout << "[HOST " << name << "] Packet TTL expired.\n";
            return;
        }
        

        if (packet.dest == ip) {
            std::cout << "[HOST " << name << "] Received: " << packet.payload << std::endl;
        } else {
            std::cout << "[HOST " << name << "] Packet not for me.\n";
        }

    }
}