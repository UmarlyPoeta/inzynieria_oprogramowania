#pragma once
#include <nlohmann/json.hpp>
#include "core/Packet.hpp"


namespace JsonAdapter {
    using json = nlohmann::json;



inline json packetToJson(const Packet &p) {
    json j;
    j["src"] = p.src;
    j["dest"] = p.dest;
    j["type"] = p.type;
    j["ttl"] = p.ttl;
    return j;
}

inline Packet jsonToPacket(const json &j) {
    Packet p;
    if (j.contains("src")) p.src = j["src"].get<std::string>();
    if (j.contains("dest")) p.dest = j["dest"].get<std::string>();
    if (j.contains("type")) p.type = j["type"].get<std::string>();
    if (j.contains("ttl")) p.ttl = j["ttl"].get<int>();
    return p;
}

}