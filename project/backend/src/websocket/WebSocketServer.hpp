#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include <mutex>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>

namespace netsim {
namespace ws {

typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;
typedef websocketpp::connection_hdl connection_hdl;

/**
 * @brief WebSocket server for real-time updates
 * 
 * Provides live updates for:
 * - Topology changes (nodes, links)
 * - Packet transmission events
 * - Node failures/recoveries
 * - Network statistics
 */
class WebSocketServer {
public:
    WebSocketServer() {
        // Set logging settings
        m_server.set_access_channels(websocketpp::log::alevel::all);
        m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
        
        // Initialize Asio
        m_server.init_asio();
        
        // Register handlers
        m_server.set_open_handler([this](connection_hdl hdl) {
            this->onOpen(hdl);
        });
        
        m_server.set_close_handler([this](connection_hdl hdl) {
            this->onClose(hdl);
        });
        
        m_server.set_message_handler([this](connection_hdl hdl, message_ptr msg) {
            this->onMessage(hdl, msg);
        });
    }
    
    /**
     * @brief Start WebSocket server on specified port
     */
    void start(uint16_t port) {
        m_server.listen(port);
        m_server.start_accept();
        
        std::cout << "[WebSocket] Server started on port " << port << std::endl;
        
        // Run in separate thread
        m_thread = std::thread([this]() {
            m_server.run();
        });
    }
    
    /**
     * @brief Stop WebSocket server
     */
    void stop() {
        m_server.stop_listening();
        
        // Close all connections
        std::lock_guard<std::mutex> lock(m_connection_lock);
        for (auto hdl : m_connections) {
            m_server.close(hdl, websocketpp::close::status::going_away, "Server shutdown");
        }
        
        if (m_thread.joinable()) {
            m_thread.join();
        }
        
        std::cout << "[WebSocket] Server stopped" << std::endl;
    }
    
    /**
     * @brief Broadcast message to all connected clients
     */
    void broadcast(const std::string& event_type, const nlohmann::json& data) {
        nlohmann::json message = {
            {"type", event_type},
            {"timestamp", std::time(nullptr)},
            {"data", data}
        };
        
        std::string payload = message.dump();
        
        std::lock_guard<std::mutex> lock(m_connection_lock);
        for (auto hdl : m_connections) {
            try {
                m_server.send(hdl, payload, websocketpp::frame::opcode::text);
            } catch (const std::exception& e) {
                std::cerr << "[WebSocket] Send failed: " << e.what() << std::endl;
            }
        }
    }
    
    /**
     * @brief Send message to specific client
     */
    void sendToClient(connection_hdl hdl, const std::string& event_type, const nlohmann::json& data) {
        nlohmann::json message = {
            {"type", event_type},
            {"timestamp", std::time(nullptr)},
            {"data", data}
        };
        
        try {
            m_server.send(hdl, message.dump(), websocketpp::frame::opcode::text);
        } catch (const std::exception& e) {
            std::cerr << "[WebSocket] Send to client failed: " << e.what() << std::endl;
        }
    }
    
    /**
     * @brief Get number of connected clients
     */
    size_t getClientCount() const {
        std::lock_guard<std::mutex> lock(m_connection_lock);
        return m_connections.size();
    }
    
    // Event broadcasting methods
    void broadcastNodeAdded(const std::string& name, const std::string& type, const std::string& ip) {
        broadcast("node_added", nlohmann::json{
            {"name", name},
            {"type", type},
            {"ip", ip}
        });
    }
    
    void broadcastNodeRemoved(const std::string& name) {
        broadcast("node_removed", nlohmann::json{
            {"name", name}
        });
    }
    
    void broadcastNodeFailed(const std::string& name) {
        nlohmann::json data = { {"name", name} };
        broadcast("node_failed", data);
    }
    
    void broadcastNodeRecovered(const std::string& name) {
        nlohmann::json data = { {"name", name} };
        broadcast("node_recovered", data);
    }

    void broadcastNodeUpdated(const std::string& name, const std::string& type, const std::string& ip, int mtu, int queueSize) {
        nlohmann::json data = {
            {"name", name},
            {"type", type},
            {"ip", ip},
            {"mtu", mtu},
            {"queueSize", queueSize}
        };
        broadcast("node_updated", data);
    }
    
    // Link events
    void broadcastLinkAdded(const std::string& source, const std::string& target, int delay, int bandwidth) {
        nlohmann::json data = {
            {"source", source},
            {"target", target},
            {"delay", delay},
            {"bandwidth", bandwidth}
        };
        broadcast("link_added", data);
    }
    
    void broadcastLinkRemoved(const std::string& source, const std::string& target) {
        nlohmann::json data = {
            {"source", source},
            {"target", target}
        };
        broadcast("link_removed", data);
    }
    
    // Packet events
    void broadcastPacketSent(const std::string& from, const std::string& to, 
                            const std::string& protocol, int size) {
        broadcast("packet_sent", nlohmann::json{
            {"from", from},
            {"to", to},
            {"protocol", protocol},
            {"size", size}
        });
    }
    
    void broadcastStatistics(const nlohmann::json& stats) {
        broadcast("statistics_update", stats);
    }
    
    void broadcastTopologyChanged() {
        broadcast("topology_changed", nlohmann::json{
            {"message", "Network topology has been modified"}
        });
    }

private:
    void onOpen(connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(m_connection_lock);
        m_connections.insert(hdl);
        
        std::cout << "[WebSocket] Client connected (total: " 
                  << m_connections.size() << ")" << std::endl;
        
        // Send welcome message
        sendToClient(hdl, "connected", nlohmann::json{
            {"message", "Connected to NetSimCPP WebSocket server"},
            {"version", "1.0.0"}
        });
    }
    
    void onClose(connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(m_connection_lock);
        m_connections.erase(hdl);
        
        std::cout << "[WebSocket] Client disconnected (total: " 
                  << m_connections.size() << ")" << std::endl;
    }
    
    void onMessage(connection_hdl hdl, message_ptr msg) {
        std::string payload = msg->get_payload();
        
        try {
            nlohmann::json request = nlohmann::json::parse(payload);
            std::string msg_type = request.value("type", "");
            
            // Handle client requests
            if (msg_type == "ping") {
                sendToClient(hdl, "pong", nlohmann::json{{"timestamp", std::time(nullptr)}});
            }
            else if (msg_type == "subscribe") {
                std::string channel = request.value("channel", "");
                // Future: implement channel-based subscriptions
                sendToClient(hdl, "subscribed", nlohmann::json{{"channel", channel}});
            }
            
        } catch (const std::exception& e) {
            std::cerr << "[WebSocket] Message parse error: " << e.what() << std::endl;
        }
    }
    
    server m_server;
    std::set<connection_hdl, std::owner_less<connection_hdl>> m_connections;
    mutable std::mutex m_connection_lock;
    std::thread m_thread;
};

} // namespace ws
} // namespace netsim
