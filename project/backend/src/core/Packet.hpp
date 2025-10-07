#pragma once

#include <string>

struct Packet {
    std::string src;
    std::string dest;
    std::string type;
    int ttl = 64;
};