#pragma once

#include <string>

struct Packet {
    std::string src;
    std::string dest;
    std::string type;
    std::string protocol;
    std::string payload;
    int delayMs = 10;
    int ttl = 64;
    Packet() = default;
    Packet(std::string src, std::string dest, std::string type, std::string protocol, std::string payload)
        : src(std::move(src)), dest(std::move(dest)), type(std::move(type)), protocol(std::move(protocol)), payload(std::move(payload)) {}
};