#pragma once
#include "Network.hpp"
#include "Packet.hpp"
#include <string>
#include <vector>

class Engine {
public:
    explicit Engine(Network& net);
    bool ping(const std::string &srcName,
               const std::string &dstName,
               std::vector<std::string> &path);
    bool traceroute(const std::string &srcName,
                    const std::string &dstName,
                    std::vector<std::string> &path);

private:
    Network &net;
};
