#ifndef STATS_REPOSITORY_HPP
#define STATS_REPOSITORY_HPP

#include "DatabaseManager.hpp"
#include <string>
#include <vector>
#include <tuple>

namespace netsim {
namespace db {

/**
 * @brief Repository for Packet Statistics persistence
 */
class StatsRepository {
public:
    StatsRepository(DatabaseManager& dbManager) : db(dbManager) {}

    /**
     * @brief Record packet transmission
     * @param srcNodeId Source node database ID
     * @param dstNodeId Destination node database ID
     * @param packetCount Number of packets (default 1)
     */
    void recordPacket(int64_t srcNodeId, int64_t dstNodeId, int packetCount = 1) {
        // Use INSERT ... ON DUPLICATE KEY UPDATE to increment counter
        std::string query = 
            "INSERT INTO packet_stats (src_node_id, dst_node_id, packet_count, last_update) "
            "VALUES (?, ?, ?, NOW()) "
            "ON DUPLICATE KEY UPDATE "
            "packet_count = packet_count + VALUES(packet_count), "
            "last_update = NOW()";
        
        auto stmt = db.prepareStatement(query);
        stmt->setInt64(1, srcNodeId);
        stmt->setInt64(2, dstNodeId);
        stmt->setInt(3, packetCount);
        
        stmt->executeUpdate();
    }

    /**
     * @brief Get statistics between two nodes
     * @return Tuple of (packet_count, last_update)
     */
    std::optional<std::tuple<int, std::string>> getStats(int64_t srcNodeId, int64_t dstNodeId) {
        std::string query = 
            "SELECT packet_count, last_update FROM packet_stats "
            "WHERE src_node_id = ? AND dst_node_id = ?";
        
        auto stmt = db.prepareStatement(query);
        stmt->setInt64(1, srcNodeId);
        stmt->setInt64(2, dstNodeId);
        
        auto rs = stmt->executeQuery();
        if (rs->next()) {
            return std::make_tuple(
                rs->getInt("packet_count"),
                rs->getString("last_update")
            );
        }
        
        return std::nullopt;
    }

    /**
     * @brief Get all statistics
     * @return Vector of (src_id, dst_id, packet_count, last_update) tuples
     */
    std::vector<std::tuple<int64_t, int64_t, int, std::string>> getAllStats() {
        std::vector<std::tuple<int64_t, int64_t, int, std::string>> stats;
        
        auto rs = db.executeQuery(
            "SELECT src_node_id, dst_node_id, packet_count, last_update "
            "FROM packet_stats "
            "ORDER BY packet_count DESC"
        );
        
        while (rs->next()) {
            stats.emplace_back(
                rs->getInt64("src_node_id"),
                rs->getInt64("dst_node_id"),
                rs->getInt("packet_count"),
                rs->getString("last_update")
            );
        }
        
        return stats;
    }

    /**
     * @brief Get top N most active node pairs
     */
    std::vector<std::tuple<std::string, std::string, int>> getTopStats(int limit = 10) {
        std::vector<std::tuple<std::string, std::string, int>> stats;
        
        std::string query = 
            "SELECT n1.name AS src_name, n2.name AS dst_name, ps.packet_count "
            "FROM packet_stats ps "
            "JOIN nodes n1 ON ps.src_node_id = n1.id "
            "JOIN nodes n2 ON ps.dst_node_id = n2.id "
            "ORDER BY ps.packet_count DESC "
            "LIMIT ?";
        
        auto stmt = db.prepareStatement(query);
        stmt->setInt(1, limit);
        
        auto rs = stmt->executeQuery();
        while (rs->next()) {
            stats.emplace_back(
                rs->getString("src_name"),
                rs->getString("dst_name"),
                rs->getInt("packet_count")
            );
        }
        
        return stats;
    }

    /**
     * @brief Delete all statistics
     */
    void deleteAllStats() {
        db.executeUpdate("DELETE FROM packet_stats");
    }

    /**
     * @brief Get statistics for a specific node (as source)
     */
    std::vector<std::tuple<std::string, int>> getStatsFromNode(int64_t nodeId) {
        std::vector<std::tuple<std::string, int>> stats;
        
        std::string query = 
            "SELECT n.name, ps.packet_count "
            "FROM packet_stats ps "
            "JOIN nodes n ON ps.dst_node_id = n.id "
            "WHERE ps.src_node_id = ? "
            "ORDER BY ps.packet_count DESC";
        
        auto stmt = db.prepareStatement(query);
        stmt->setInt64(1, nodeId);
        
        auto rs = stmt->executeQuery();
        while (rs->next()) {
            stats.emplace_back(
                rs->getString("name"),
                rs->getInt("packet_count")
            );
        }
        
        return stats;
    }

private:
    DatabaseManager& db;
};

} // namespace db
} // namespace netsim

#endif // STATS_REPOSITORY_HPP
