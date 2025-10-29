#pragma once

#include "WebSocketServer.hpp"
#include <memory>

namespace netsim {
namespace ws {

/**
 * @brief Singleton event broadcaster for WebSocket notifications
 * 
 * This class provides a global access point to broadcast events
 * from anywhere in the application without direct coupling to WebSocket server.
 */
class EventBroadcaster {
public:
    static EventBroadcaster& getInstance() {
        static EventBroadcaster instance;
        return instance;
    }
    
    void setWebSocketServer(std::shared_ptr<WebSocketServer> server) {
        m_ws_server = server;
    }
    
    // Node events
    void nodeAdded(const std::string& name, const std::string& type, const std::string& ip) {
        if (m_ws_server) {
            m_ws_server->broadcastNodeAdded(name, type, ip);
        }
    }
    
    void nodeRemoved(const std::string& name) {
        if (m_ws_server) {
            m_ws_server->broadcastNodeRemoved(name);
        }
    }
    
    void nodeFailed(const std::string& name) {
        if (m_ws_server) {
            m_ws_server->broadcastNodeFailed(name);
        }
    }
    
    void nodeRecovered(const std::string& name) {
        if (m_ws_server) {
            m_ws_server->broadcastNodeRecovered(name);
        }
    }

    void nodeUpdated(const std::string& name, const std::string& type, const std::string& ip, int mtu, int queueSize) {
        if (m_ws_server) {
            m_ws_server->broadcastNodeUpdated(name, type, ip, mtu, queueSize);
        }
    }
    
    // Link events
    void linkAdded(const std::string& source, const std::string& target, int delay, int bandwidth) {
        if (m_ws_server) {
            m_ws_server->broadcastLinkAdded(source, target, delay, bandwidth);
        }
    }
    
    void linkRemoved(const std::string& from, const std::string& to) {
        if (m_ws_server) {
            m_ws_server->broadcastLinkRemoved(from, to);
        }
    }
    
    // Packet events
    void packetSent(const std::string& from, const std::string& to, 
                   const std::string& protocol, int size) {
        if (m_ws_server) {
            m_ws_server->broadcastPacketSent(from, to, protocol, size);
        }
    }
    
    // Statistics events
    void statisticsUpdate(const nlohmann::json& stats) {
        if (m_ws_server) {
            m_ws_server->broadcastStatistics(stats);
        }
    }
    
    // Topology events
    void topologyChanged() {
        if (m_ws_server) {
            m_ws_server->broadcastTopologyChanged();
        }
    }
    
private:
    EventBroadcaster() = default;
    ~EventBroadcaster() = default;
    EventBroadcaster(const EventBroadcaster&) = delete;
    EventBroadcaster& operator=(const EventBroadcaster&) = delete;
    
    std::shared_ptr<WebSocketServer> m_ws_server;
};

} // namespace ws
} // namespace netsim
