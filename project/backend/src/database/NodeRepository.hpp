#ifndef NODE_REPOSITORY_HPP
#define NODE_REPOSITORY_HPP

#include "DatabaseManager.hpp"
#include "../core/Node.hpp"
#include "../core/Host.hpp"
#include "../core/Router.hpp"
#include <vector>
#include <memory>
#include <optional>

namespace netsim {
namespace db {

/**
 * @brief Repository for Node persistence
 */
class NodeRepository {
public:
    NodeRepository(DatabaseManager& dbManager) : db(dbManager) {}

    /**
     * @brief Save node to database
     * @param node Node to save
     * @return Database ID of inserted node
     */
    int64_t createNode(const Node& node) {
        std::string query = 
            "INSERT INTO nodes (name, ip, type, vlan_id, failed, created_at) "
            "VALUES (?, ?, ?, ?, ?, NOW())";
        
        auto stmt = db.prepareStatement(query);
        stmt->setString(1, node.getName());
        stmt->setString(2, node.getAddress());
        stmt->setString(3, getNodeTypeString(node));
        stmt->setNull(4, sql::DataType::INTEGER); // VLAN not implemented yet
        stmt->setBoolean(5, false); // Failed status
        
        stmt->executeUpdate();
        return db.getLastInsertId();
    }

    /**
     * @brief Find node by name
     * @param name Node name
     * @return Optional node ID
     */
    std::optional<int64_t> getNodeIdByName(const std::string& name) {
        std::string query = "SELECT id FROM nodes WHERE name = ?";
        auto stmt = db.prepareStatement(query);
        stmt->setString(1, name);
        
        auto rs = stmt->executeQuery();
        if (rs->next()) {
            return rs->getInt64("id");
        }
        return std::nullopt;
    }

    /**
     * @brief Get all nodes from database
     * @return Vector of (id, name, ip, type) tuples
     */
    std::vector<std::tuple<int64_t, std::string, std::string, std::string>> getAllNodes() {
        std::vector<std::tuple<int64_t, std::string, std::string, std::string>> nodes;
        
        auto rs = db.executeQuery("SELECT id, name, ip, type FROM nodes ORDER BY created_at");
        while (rs->next()) {
            nodes.emplace_back(
                rs->getInt64("id"),
                rs->getString("name"),
                rs->getString("ip"),
                rs->getString("type")
            );
        }
        
        return nodes;
    }

    /**
     * @brief Delete node by name
     * @param name Node name
     * @return True if deleted
     */
    bool deleteNode(const std::string& name) {
        std::string query = "DELETE FROM nodes WHERE name = ?";
        auto stmt = db.prepareStatement(query);
        stmt->setString(1, name);
        
        int affected = stmt->executeUpdate();
        return affected > 0;
    }

    /**
     * @brief Delete all nodes
     */
    void deleteAllNodes() {
        db.executeUpdate("DELETE FROM nodes");
    }

    /**
     * @brief Check if node exists
     */
    bool nodeExists(const std::string& name) {
        return getNodeIdByName(name).has_value();
    }

    /**
     * @brief Update node failure status
     */
    void updateNodeFailureStatus(const std::string& name, bool failed) {
        std::string query = "UPDATE nodes SET failed = ? WHERE name = ?";
        auto stmt = db.prepareStatement(query);
        stmt->setBoolean(1, failed);
        stmt->setString(2, name);
        stmt->executeUpdate();
    }

private:
    DatabaseManager& db;

    /**
     * @brief Get node type as string
     */
    std::string getNodeTypeString(const Node& node) {
        // Try to determine node type
        if (dynamic_cast<const Host*>(&node)) {
            return "host";
        } else if (dynamic_cast<const Router*>(&node)) {
            return "router";
        } else {
            return "unknown";
        }
    }
};

} // namespace db
} // namespace netsim

#endif // NODE_REPOSITORY_HPP
