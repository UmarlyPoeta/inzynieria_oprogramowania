#include "Packet.hpp"
#include <algorithm>

std::vector<Packet> Packet::fragmentPacket(int mtu) {
    std::vector<Packet> fragments;
    if (payload.size() <= mtu) {
        fragments.push_back(*this);
        return fragments;
    }
    int id = 0; // Simple id, can be improved
    int numFragments = (payload.size() + mtu - 1) / mtu;
    for (int i = 0; i < numFragments; ++i) {
        Packet frag = *this;
        frag.fragmentId = id;
        frag.seqNum = i;
        int start = i * mtu;
        int len = std::min(mtu, (int)payload.size() - start);
        frag.payload = payload.substr(start, len);
        frag.isLast = (i == numFragments - 1);
        fragments.push_back(frag);
    }
    return fragments;
}

Packet Packet::reassemblePacket(const std::vector<Packet>& fragments) {
    if (fragments.empty()) return Packet();
    Packet reassembled = fragments[0];
    reassembled.payload = "";
    // Sort by seqNum
    std::vector<Packet> sorted = fragments;
    std::sort(sorted.begin(), sorted.end(), [](const Packet& a, const Packet& b) {
        return a.seqNum < b.seqNum;
    });
    for (const auto& frag : sorted) {
        reassembled.payload += frag.payload;
    }
    reassembled.isLast = false;
    return reassembled;
}