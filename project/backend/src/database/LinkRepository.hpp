#ifndef LINK_REPOSITORY_HPP
#define LINK_REPOSITORY_HPP

#include "DatabaseManager.hpp"
#include <string>
#include <vector>
#include <tuple>

namespace netsim {
namespace db {

/**
 * @brief Repository for Link persistence
 */
class LinkRepository {
public:
    LinkRepository(DatabaseManager& dbManager) : db(dbManager) {}

    /**
     * @brief Save link to database
     * @param nodeAId Database ID of first node
     * @param nodeBId Database ID of second node
     * @param delayMs Link delay in milliseconds
     * @param bandwidthMbps Link bandwidth in Mbps
     * @param packetLoss Packet loss percentage
     * @return Database ID of inserted link
     */
    int64_t createLink(int64_t nodeAId, int64_t nodeBId, 
                       int delayMs = 10, int bandwidthMbps = 100, 
                       float packetLoss = 0.0) {
        std::string query = 
            "INSERT INTO links (node_a_id, node_b_id, delay_ms, bandwidth_mbps, packet_loss) "
            "VALUES (?, ?, ?, ?, ?)";
        
        auto stmt = db.prepareStatement(query);
        stmt->setInt64(1, nodeAId);
        stmt->setInt64(2, nodeBId);
        stmt->setInt(3, delayMs);
        stmt->setInt(4, bandwidthMbps);
        stmt->setDouble(5, packetLoss);
        
        stmt->executeUpdate();
        return db.getLastInsertId();
    }

    /**
     * @brief Get all links from database
     * @return Vector of (id, nodeAId, nodeBId, delay, bandwidth, loss) tuples
     */
    std::vector<std::tuple<int64_t, int64_t, int64_t, int, int, float>> getAllLinks() {
        std::vector<std::tuple<int64_t, int64_t, int64_t, int, int, float>> links;
        
        auto rs = db.executeQuery(
            "SELECT id, node_a_id, node_b_id, delay_ms, bandwidth_mbps, packet_loss "
            "FROM links"
        );
        
        while (rs->next()) {
            links.emplace_back(
                rs->getInt64("id"),
                rs->getInt64("node_a_id"),
                rs->getInt64("node_b_id"),
                rs->getInt("delay_ms"),
                rs->getInt("bandwidth_mbps"),
                static_cast<float>(rs->getDouble("packet_loss"))
            );
        }
        
        return links;
    }

    /**
     * @brief Check if link exists between two nodes
     */
    bool linkExists(int64_t nodeAId, int64_t nodeBId) {
        std::string query = 
            "SELECT COUNT(*) as cnt FROM links "
            "WHERE (node_a_id = ? AND node_b_id = ?) "
            "   OR (node_a_id = ? AND node_b_id = ?)";
        
        auto stmt = db.prepareStatement(query);
        stmt->setInt64(1, nodeAId);
        stmt->setInt64(2, nodeBId);
        stmt->setInt64(3, nodeBId);
        stmt->setInt64(4, nodeAId);
        
        auto rs = stmt->executeQuery();
        if (rs->next()) {
            return rs->getInt("cnt") > 0;
        }
        return false;
    }

    /**
     * @brief Delete link between two nodes
     */
    bool deleteLink(int64_t nodeAId, int64_t nodeBId) {
        std::string query = 
            "DELETE FROM links "
            "WHERE (node_a_id = ? AND node_b_id = ?) "
            "   OR (node_a_id = ? AND node_b_id = ?)";
        
        auto stmt = db.prepareStatement(query);
        stmt->setInt64(1, nodeAId);
        stmt->setInt64(2, nodeBId);
        stmt->setInt64(3, nodeBId);
        stmt->setInt64(4, nodeAId);
        
        int affected = stmt->executeUpdate();
        return affected > 0;
    }

    /**
     * @brief Delete all links
     */
    void deleteAllLinks() {
        db.executeUpdate("DELETE FROM links");
    }

    /**
     * @brief Get links for a specific node
     */
    std::vector<std::tuple<int64_t, int64_t, int64_t>> getLinksForNode(int64_t nodeId) {
        std::vector<std::tuple<int64_t, int64_t, int64_t>> links;
        
        std::string query = 
            "SELECT id, node_a_id, node_b_id FROM links "
            "WHERE node_a_id = ? OR node_b_id = ?";
        
        auto stmt = db.prepareStatement(query);
        stmt->setInt64(1, nodeId);
        stmt->setInt64(2, nodeId);
        
        auto rs = stmt->executeQuery();
        while (rs->next()) {
            links.emplace_back(
                rs->getInt64("id"),
                rs->getInt64("node_a_id"),
                rs->getInt64("node_b_id")
            );
        }
        
        return links;
    }

private:
    DatabaseManager& db;
};

} // namespace db
} // namespace netsim

#endif // LINK_REPOSITORY_HPP
