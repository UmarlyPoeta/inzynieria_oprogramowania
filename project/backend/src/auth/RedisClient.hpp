#pragma once

#include <hiredis/hiredis.h>
#include <string>
#include <stdexcept>
#include <memory>

namespace netsim {
namespace auth {

/**
 * @brief Redis client wrapper for session management
 * 
 * Stores JWT tokens and session data in Redis
 */
class RedisClient {
public:
    /**
     * @brief Construct Redis client
     * 
     * @param host Redis server host
     * @param port Redis server port
     * @param password Redis password (optional)
     */
    RedisClient(const std::string& host, int port, const std::string& password = "") 
        : m_host(host), m_port(port), m_password(password), m_context(nullptr) {
        connect();
    }
    
    ~RedisClient() {
        disconnect();
    }
    
    /**
     * @brief Store a session token in Redis
     * 
     * @param user_id User ID
     * @param token JWT token
     * @param ttl Time to live in seconds (default: 24 hours)
     */
    void storeSession(int user_id, const std::string& token, int ttl = 86400) {
        std::string key = "session:" + std::to_string(user_id);
        
        redisReply* reply = (redisReply*)redisCommand(
            m_context, 
            "SETEX %s %d %s", 
            key.c_str(), 
            ttl, 
            token.c_str()
        );
        
        if (reply == nullptr) {
            throw std::runtime_error("Redis SETEX failed");
        }
        
        freeReplyObject(reply);
    }
    
    /**
     * @brief Get a session token from Redis
     * 
     * @param user_id User ID
     * @return std::string Session token (empty if not found)
     */
    std::string getSession(int user_id) {
        std::string key = "session:" + std::to_string(user_id);
        
        redisReply* reply = (redisReply*)redisCommand(
            m_context, 
            "GET %s", 
            key.c_str()
        );
        
        if (reply == nullptr) {
            throw std::runtime_error("Redis GET failed");
        }
        
        std::string token;
        if (reply->type == REDIS_REPLY_STRING) {
            token = std::string(reply->str, reply->len);
        }
        
        freeReplyObject(reply);
        return token;
    }
    
    /**
     * @brief Delete a session from Redis (logout)
     * 
     * @param user_id User ID
     */
    void deleteSession(int user_id) {
        std::string key = "session:" + std::to_string(user_id);
        
        redisReply* reply = (redisReply*)redisCommand(
            m_context, 
            "DEL %s", 
            key.c_str()
        );
        
        if (reply == nullptr) {
            throw std::runtime_error("Redis DEL failed");
        }
        
        freeReplyObject(reply);
    }
    
    /**
     * @brief Check if a session exists
     * 
     * @param user_id User ID
     * @return true if session exists
     * @return false if session doesn't exist
     */
    bool sessionExists(int user_id) {
        std::string key = "session:" + std::to_string(user_id);
        
        redisReply* reply = (redisReply*)redisCommand(
            m_context, 
            "EXISTS %s", 
            key.c_str()
        );
        
        if (reply == nullptr) {
            throw std::runtime_error("Redis EXISTS failed");
        }
        
        bool exists = (reply->integer == 1);
        freeReplyObject(reply);
        
        return exists;
    }
    
    /**
     * @brief Store generic key-value pair with TTL
     * 
     * @param key Key
     * @param value Value
     * @param ttl Time to live in seconds
     */
    void set(const std::string& key, const std::string& value, int ttl = 0) {
        redisReply* reply;
        
        if (ttl > 0) {
            reply = (redisReply*)redisCommand(
                m_context, 
                "SETEX %s %d %s", 
                key.c_str(), 
                ttl, 
                value.c_str()
            );
        } else {
            reply = (redisReply*)redisCommand(
                m_context, 
                "SET %s %s", 
                key.c_str(), 
                value.c_str()
            );
        }
        
        if (reply == nullptr) {
            throw std::runtime_error("Redis SET failed");
        }
        
        freeReplyObject(reply);
    }
    
    /**
     * @brief Get value by key
     * 
     * @param key Key
     * @return std::string Value (empty if not found)
     */
    std::string get(const std::string& key) {
        redisReply* reply = (redisReply*)redisCommand(
            m_context, 
            "GET %s", 
            key.c_str()
        );
        
        if (reply == nullptr) {
            throw std::runtime_error("Redis GET failed");
        }
        
        std::string value;
        if (reply->type == REDIS_REPLY_STRING) {
            value = std::string(reply->str, reply->len);
        }
        
        freeReplyObject(reply);
        return value;
    }
    
    /**
     * @brief Delete a key
     * 
     * @param key Key to delete
     */
    void del(const std::string& key) {
        redisReply* reply = (redisReply*)redisCommand(
            m_context, 
            "DEL %s", 
            key.c_str()
        );
        
        if (reply == nullptr) {
            throw std::runtime_error("Redis DEL failed");
        }
        
        freeReplyObject(reply);
    }
    
private:
    void connect() {
        m_context = redisConnect(m_host.c_str(), m_port);
        
        if (m_context == nullptr || m_context->err) {
            if (m_context) {
                std::string err = m_context->errstr;
                redisFree(m_context);
                throw std::runtime_error("Redis connection failed: " + err);
            } else {
                throw std::runtime_error("Redis connection failed: Can't allocate context");
            }
        }
        
        // Authenticate if password is provided
        if (!m_password.empty()) {
            redisReply* reply = (redisReply*)redisCommand(
                m_context, 
                "AUTH %s", 
                m_password.c_str()
            );
            
            if (reply == nullptr || reply->type == REDIS_REPLY_ERROR) {
                std::string err = reply ? reply->str : "Authentication failed";
                if (reply) freeReplyObject(reply);
                throw std::runtime_error("Redis auth failed: " + err);
            }
            
            freeReplyObject(reply);
        }
    }
    
    void disconnect() {
        if (m_context) {
            redisFree(m_context);
            m_context = nullptr;
        }
    }
    
    std::string m_host;
    int m_port;
    std::string m_password;
    redisContext* m_context;
};

} // namespace auth
} // namespace netsim
