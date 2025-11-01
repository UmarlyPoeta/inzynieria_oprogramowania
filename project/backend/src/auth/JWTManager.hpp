#pragma once

#include <jwt-cpp/jwt.h>
#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

namespace netsim {
namespace auth {

/**
 * @brief JWT token generator and validator
 * 
 * Uses jwt-cpp library for creating and verifying JWT tokens
 */
class JWTManager {
public:
    /**
     * @brief Construct JWT manager with secret key
     * 
     * @param secret Secret key for signing tokens (get from environment)
     */
    explicit JWTManager(const std::string& secret) : m_secret(secret) {}
    
    /**
     * @brief Generate a JWT token for a user
     * 
     * @param user_id User ID
     * @param username Username
     * @param role User role (admin, user, viewer)
     * @param expiry_hours Token expiry in hours (default: 24h)
     * @return std::string JWT token
     */
    std::string generateToken(
        int user_id, 
        const std::string& username, 
        const std::string& role,
        int expiry_hours = 24
    ) {
        auto now = std::chrono::system_clock::now();
        auto exp = now + std::chrono::hours(expiry_hours);
        
        auto token = jwt::create()
            .set_issuer("netsim-cpp")
            .set_type("JWT")
            .set_issued_at(now)
            .set_expires_at(exp)
            .set_payload_claim("user_id", jwt::claim(std::to_string(user_id)))
            .set_payload_claim("username", jwt::claim(username))
            .set_payload_claim("role", jwt::claim(role))
            .sign(jwt::algorithm::hs256{m_secret});
        
        return token;
    }
    
    /**
     * @brief Verify and decode a JWT token
     * 
     * @param token JWT token string
     * @return nlohmann::json Decoded payload
     * @throws std::runtime_error if token is invalid
     */
    nlohmann::json verifyToken(const std::string& token) {
        try {
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{m_secret})
                .with_issuer("netsim-cpp");
            
            auto decoded = jwt::decode(token);
            verifier.verify(decoded);
            
            // Extract claims
            nlohmann::json payload;
            payload["user_id"] = std::stoi(decoded.get_payload_claim("user_id").as_string());
            payload["username"] = decoded.get_payload_claim("username").as_string();
            payload["role"] = decoded.get_payload_claim("role").as_string();
            
            return payload;
            
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid JWT token: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Check if token is expired
     * 
     * @param token JWT token string
     * @return true if token is expired
     * @return false if token is still valid
     */
    bool isTokenExpired(const std::string& token) {
        try {
            auto decoded = jwt::decode(token);
            auto exp = decoded.get_expires_at();
            auto now = std::chrono::system_clock::now();
            
            return exp < now;
            
        } catch (const std::exception&) {
            return true; // Treat invalid tokens as expired
        }
    }
    
    /**
     * @brief Extract user ID from token without full verification
     * 
     * @param token JWT token string
     * @return int User ID
     */
    int getUserIdFromToken(const std::string& token) {
        try {
            auto decoded = jwt::decode(token);
            return std::stoi(decoded.get_payload_claim("user_id").as_string());
        } catch (const std::exception&) {
            return -1;
        }
    }
    
    /**
     * @brief Extract role from token without full verification
     * 
     * @param token JWT token string
     * @return std::string User role
     */
    std::string getRoleFromToken(const std::string& token) {
        try {
            auto decoded = jwt::decode(token);
            return decoded.get_payload_claim("role").as_string();
        } catch (const std::exception&) {
            return "";
        }
    }
    
private:
    std::string m_secret;
};

} // namespace auth
} // namespace netsim

