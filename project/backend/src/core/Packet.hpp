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
    int priority = 0;
    // TCP fields
    int seqNum = 0;
    int ackNum = 0;
    bool syn = false;
    bool ack = false;
    Packet() = default;
    Packet(std::string src, std::string dest, std::string type, std::string protocol, std::string payload)
        : src(std::move(src)), dest(std::move(dest)), type(std::move(type)), protocol(std::move(protocol)), payload(std::move(payload)) {}

    void setPriority(int p) { priority = p; }
    int getPriority() const { return priority; }
};