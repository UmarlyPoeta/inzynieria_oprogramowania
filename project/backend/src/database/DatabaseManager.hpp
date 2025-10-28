#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>

namespace netsim {
namespace db {

/**
 * @brief MySQL Database Manager
 * Handles connection, transactions, and query execution
 */
class DatabaseManager {
public:
    /**
     * @brief Get singleton instance
     */
    static DatabaseManager& getInstance() {
        static DatabaseManager instance;
        return instance;
    }

    /**
     * @brief Initialize database connection
     * @param host Database host
     * @param port Database port
     * @param user Database user
     * @param password Database password
     * @param database Database name
     */
    void connect(const std::string& host, int port, 
                 const std::string& user, const std::string& password,
                 const std::string& database) {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            
            // Build connection string
            std::string connStr = "tcp://" + host + ":" + std::to_string(port);
            
            connection.reset(driver->connect(connStr, user, password));
            connection->setSchema(database);
            
            std::cout << "[DB] Connected to MySQL: " << database << std::endl;
            
        } catch (sql::SQLException& e) {
            throw std::runtime_error("MySQL connection error: " + std::string(e.what()));
        }
    }

    /**
     * @brief Disconnect from database
     */
    void disconnect() {
        if (connection && !connection->isClosed()) {
            connection->close();
            std::cout << "[DB] Disconnected from MySQL" << std::endl;
        }
    }

    /**
     * @brief Check if connected
     */
    bool isConnected() const {
        return connection && !connection->isClosed();
    }

    /**
     * @brief Execute SELECT query
     * @param query SQL query
     * @return ResultSet pointer
     */
    std::unique_ptr<sql::ResultSet> executeQuery(const std::string& query) {
        if (!isConnected()) {
            throw std::runtime_error("Not connected to database");
        }

        try {
            std::unique_ptr<sql::Statement> stmt(connection->createStatement());
            return std::unique_ptr<sql::ResultSet>(stmt->executeQuery(query));
        } catch (sql::SQLException& e) {
            throw std::runtime_error("Query error: " + std::string(e.what()));
        }
    }

    /**
     * @brief Execute INSERT/UPDATE/DELETE query
     * @param query SQL query
     * @return Number of affected rows
     */
    int executeUpdate(const std::string& query) {
        if (!isConnected()) {
            throw std::runtime_error("Not connected to database");
        }

        try {
            std::unique_ptr<sql::Statement> stmt(connection->createStatement());
            return stmt->executeUpdate(query);
        } catch (sql::SQLException& e) {
            throw std::runtime_error("Update error: " + std::string(e.what()));
        }
    }

    /**
     * @brief Prepare SQL statement
     * @param query SQL query with placeholders (?)
     * @return PreparedStatement pointer
     */
    std::unique_ptr<sql::PreparedStatement> prepareStatement(const std::string& query) {
        if (!isConnected()) {
            throw std::runtime_error("Not connected to database");
        }

        try {
            return std::unique_ptr<sql::PreparedStatement>(connection->prepareStatement(query));
        } catch (sql::SQLException& e) {
            throw std::runtime_error("Prepare statement error: " + std::string(e.what()));
        }
    }

    /**
     * @brief Start transaction
     */
    void beginTransaction() {
        if (!isConnected()) {
            throw std::runtime_error("Not connected to database");
        }
        connection->setAutoCommit(false);
    }

    /**
     * @brief Commit transaction
     */
    void commit() {
        if (!isConnected()) {
            throw std::runtime_error("Not connected to database");
        }
        connection->commit();
        connection->setAutoCommit(true);
    }

    /**
     * @brief Rollback transaction
     */
    void rollback() {
        if (!isConnected()) {
            throw std::runtime_error("Not connected to database");
        }
        connection->rollback();
        connection->setAutoCommit(true);
    }

    /**
     * @brief Get last insert ID
     */
    int64_t getLastInsertId() {
        auto rs = executeQuery("SELECT LAST_INSERT_ID() AS id");
        if (rs->next()) {
            return rs->getInt64("id");
        }
        return 0;
    }

    // Prevent copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

private:
    DatabaseManager() : driver(nullptr) {}
    ~DatabaseManager() {
        disconnect();
    }

    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> connection;
};

} // namespace db
} // namespace netsim

#endif // DATABASE_MANAGER_HPP
