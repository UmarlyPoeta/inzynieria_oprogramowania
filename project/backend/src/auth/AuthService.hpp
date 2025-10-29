#pragma once

#include <string>
#include <memory>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <nlohmann/json.hpp>
#include "PasswordHasher.hpp"
#include "JWTManager.hpp"
#include "RedisClient.hpp"

namespace netsim {
namespace auth {

/**
 * @brief Authentication service - handles user registration, login, logout
 */
class AuthService {
public:
    /**
     * @brief Construct authentication service
     * 
     * @param db_host MySQL host
     * @param db_user MySQL username
     * @param db_pass MySQL password
     * @param db_name Database name
     * @param redis_host Redis host
     * @param redis_port Redis port
     * @param redis_pass Redis password
     * @param jwt_secret JWT secret key
     */
    AuthService(
        const std::string& db_host,
        const std::string& db_user,
        const std::string& db_pass,
        const std::string& db_name,
        const std::string& redis_host,
        int redis_port,
        const std::string& redis_pass,
        const std::string& jwt_secret
    ) : m_jwt(jwt_secret),
        m_redis(std::make_shared<RedisClient>(redis_host, redis_port, redis_pass))
    {
        // Initialize MySQL connection
        m_driver = sql::mysql::get_mysql_driver_instance();
        m_conn.reset(m_driver->connect(db_host, db_user, db_pass));
        m_conn->setSchema(db_name);
    }
    
    /**
     * @brief Register a new user
     * 
     * @param username Username
     * @param email Email address
     * @param password Plain text password
     * @param role User role (admin, user, viewer)
     * @return nlohmann::json User info on success
     * @throws std::runtime_error on failure
     */
    nlohmann::json registerUser(
        const std::string& username,
        const std::string& email,
        const std::string& password,
        const std::string& role = "user"
    ) {
        // Validate role
        if (role != "admin" && role != "user" && role != "viewer") {
            throw std::runtime_error("Invalid role. Must be: admin, user, or viewer");
        }
        
        // Hash password
        std::string password_hash = PasswordHasher::hashPassword(password);
        
        // Insert user into database
        std::unique_ptr<sql::PreparedStatement> pstmt(
            m_conn->prepareStatement(
                "INSERT INTO users (username, email, password_hash, role) VALUES (?, ?, ?, ?)"
            )
        );
        
        pstmt->setString(1, username);
        pstmt->setString(2, email);
        pstmt->setString(3, password_hash);
        pstmt->setString(4, role);
        
        try {
            pstmt->executeUpdate();
        } catch (sql::SQLException& e) {
            if (e.getErrorCode() == 1062) { // Duplicate entry
                throw std::runtime_error("Username or email already exists");
            }
            throw std::runtime_error("Database error: " + std::string(e.what()));
        }
        
        // Get user ID
        std::unique_ptr<sql::PreparedStatement> select_stmt(
            m_conn->prepareStatement("SELECT id FROM users WHERE username = ?")
        );
        select_stmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(select_stmt->executeQuery());
        
        if (!res->next()) {
            throw std::runtime_error("Failed to retrieve user ID");
        }
        
        int user_id = res->getInt("id");
        
        // Log audit event
        logAuthEvent(user_id, username, "register", "", "", true, "User registered");
        
        nlohmann::json result;
        result["success"] = true;
        result["user_id"] = user_id;
        result["username"] = username;
        result["email"] = email;
        result["role"] = role;
        
        return result;
    }
    
    /**
     * @brief Login user and generate JWT token
     * 
     * @param username Username
     * @param password Plain text password
     * @param ip_address Client IP address (for audit log)
     * @param user_agent Client user agent
     * @return nlohmann::json Login response with token
     * @throws std::runtime_error on failure
     */
    nlohmann::json loginUser(
        const std::string& username,
        const std::string& password,
        const std::string& ip_address = "",
        const std::string& user_agent = ""
    ) {
        // Get user from database
        std::unique_ptr<sql::PreparedStatement> pstmt(
            m_conn->prepareStatement(
                "SELECT id, username, email, password_hash, role, is_active "
                "FROM users WHERE username = ?"
            )
        );
        pstmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (!res->next()) {
            logAuthEvent(-1, username, "failed_login", ip_address, user_agent, false, "User not found");
            throw std::runtime_error("Invalid username or password");
        }
        
        int user_id = res->getInt("id");
        std::string stored_hash = res->getString("password_hash");
        std::string role = res->getString("role");
        std::string email = res->getString("email");
        bool is_active = res->getBoolean("is_active");
        
        // Check if user is active
        if (!is_active) {
            logAuthEvent(user_id, username, "failed_login", ip_address, user_agent, false, "User is inactive");
            throw std::runtime_error("User account is inactive");
        }
        
        // Verify password
        if (!PasswordHasher::verifyPassword(password, stored_hash)) {
            logAuthEvent(user_id, username, "failed_login", ip_address, user_agent, false, "Invalid password");
            throw std::runtime_error("Invalid username or password");
        }
        
        // Generate JWT token (24 hour expiry)
        std::string token = m_jwt.generateToken(user_id, username, role, 24);
        
        // Store session in Redis (24 hours)
        m_redis->storeSession(user_id, token, 86400);
        
        // Update last_login timestamp
        std::unique_ptr<sql::PreparedStatement> update_stmt(
            m_conn->prepareStatement("UPDATE users SET last_login = NOW() WHERE id = ?")
        );
        update_stmt->setInt(1, user_id);
        update_stmt->executeUpdate();
        
        // Log successful login
        logAuthEvent(user_id, username, "login", ip_address, user_agent, true, "Login successful");
        
        nlohmann::json result;
        result["success"] = true;
        result["token"] = token;
        result["user"]["id"] = user_id;
        result["user"]["username"] = username;
        result["user"]["email"] = email;
        result["user"]["role"] = role;
        result["expires_in"] = 86400;
        
        return result;
    }
    
    /**
     * @brief Logout user (invalidate session)
     * 
     * @param token JWT token
     * @return nlohmann::json Logout response
     */
    nlohmann::json logoutUser(const std::string& token) {
        try {
            auto payload = m_jwt.verifyToken(token);
            int user_id = payload["user_id"];
            std::string username = payload["username"];
            
            // Delete session from Redis
            m_redis->deleteSession(user_id);
            
            // Log logout event
            logAuthEvent(user_id, username, "logout", "", "", true, "Logout successful");
            
            nlohmann::json result;
            result["success"] = true;
            result["message"] = "Logged out successfully";
            
            return result;
            
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid token");
        }
    }
    
    /**
     * @brief Verify JWT token and check if session exists
     * 
     * @param token JWT token
     * @return nlohmann::json User payload
     * @throws std::runtime_error if token is invalid
     */
    nlohmann::json verifyToken(const std::string& token) {
        auto payload = m_jwt.verifyToken(token);
        int user_id = payload["user_id"];
        
        // Check if session exists in Redis
        if (!m_redis->sessionExists(user_id)) {
            throw std::runtime_error("Session expired or invalid");
        }
        
        return payload;
    }
    
    /**
     * @brief Get user information
     * 
     * @param user_id User ID
     * @return nlohmann::json User info with permissions
     */
    nlohmann::json getUserInfo(int user_id) {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            m_conn->prepareStatement(
                "SELECT id, username, email, role, is_active, created_at, last_login "
                "FROM users WHERE id = ?"
            )
        );
        pstmt->setInt(1, user_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (!res->next()) {
            throw std::runtime_error("User not found");
        }
        
        nlohmann::json user;
        user["id"] = res->getInt("id");
        user["username"] = res->getString("username");
        user["email"] = res->getString("email");
        user["role"] = res->getString("role");
        user["is_active"] = res->getBoolean("is_active");
        
        // Get permissions
        std::unique_ptr<sql::PreparedStatement> perm_stmt(
            m_conn->prepareStatement(
                "SELECT resource, action FROM user_permissions WHERE user_id = ?"
            )
        );
        perm_stmt->setInt(1, user_id);
        std::unique_ptr<sql::ResultSet> perm_res(perm_stmt->executeQuery());
        
        std::vector<std::string> permissions;
        while (perm_res->next()) {
            std::string perm = perm_res->getString("resource") + ":" + perm_res->getString("action");
            permissions.push_back(perm);
        }
        
        user["permissions"] = permissions;
        
        return user;
    }
    
    /**
     * @brief Check if user has permission
     * 
     * @param user_id User ID
     * @param resource Resource name (node, link, topology)
     * @param action Action (read, write, delete)
     * @return true if user has permission
     */
    bool hasPermission(int user_id, const std::string& resource, const std::string& action) {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            m_conn->prepareStatement(
                "SELECT EXISTS(SELECT 1 FROM user_permissions "
                "WHERE user_id = ? AND resource = ? AND action = ?) AS has_perm"
            )
        );
        pstmt->setInt(1, user_id);
        pstmt->setString(2, resource);
        pstmt->setString(3, action);
        
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        res->next();
        
        return res->getBoolean("has_perm");
    }
    
    /**
     * @brief Check rate limit for user
     * 
     * @param user_id User ID
     * @param endpoint Endpoint path
     * @param max_requests Maximum requests allowed
     * @param window_seconds Time window in seconds
     * @return true if request is allowed
     */
    bool checkRateLimit(int user_id, const std::string& endpoint, int max_requests, int window_seconds) {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            m_conn->prepareStatement(
                "SELECT get_user_rate_limit(?, ?, ?, ?) AS allowed"
            )
        );
        pstmt->setInt(1, user_id);
        pstmt->setString(2, endpoint);
        pstmt->setInt(3, max_requests);
        pstmt->setInt(4, window_seconds);
        
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        res->next();
        
        return res->getBoolean("allowed");
    }
    
private:
    void logAuthEvent(
        int user_id,
        const std::string& username,
        const std::string& event_type,
        const std::string& ip_address,
        const std::string& user_agent,
        bool success,
        const std::string& message
    ) {
        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(
                m_conn->prepareStatement(
                    "INSERT INTO auth_audit_log "
                    "(user_id, username, event_type, ip_address, user_agent, success, message) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?)"
                )
            );
            
            if (user_id > 0) {
                pstmt->setInt(1, user_id);
            } else {
                pstmt->setNull(1, sql::DataType::INTEGER);
            }
            pstmt->setString(2, username);
            pstmt->setString(3, event_type);
            pstmt->setString(4, ip_address);
            pstmt->setString(5, user_agent);
            pstmt->setBoolean(6, success);
            pstmt->setString(7, message);
            
            pstmt->executeUpdate();
        } catch (const std::exception& e) {
            std::cerr << "[Auth] Failed to log audit event: " << e.what() << std::endl;
        }
    }
    
    sql::mysql::MySQL_Driver* m_driver;
    std::unique_ptr<sql::Connection> m_conn;
    JWTManager m_jwt;
    std::shared_ptr<RedisClient> m_redis;
};

} // namespace auth
} // namespace netsim
