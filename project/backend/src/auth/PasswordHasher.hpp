#pragma once

#include <string>
#include <argon2.h>
#include <cstring>
#include <vector>
#include <random>
#include <sstream>
#include <iomanip>

namespace netsim {
namespace auth {

/**
 * @brief Password hashing utility using Argon2id
 * 
 * Argon2id is the recommended password hashing algorithm (winner of PHC 2015)
 * Provides strong defense against GPU cracking attacks
 */
class PasswordHasher {
public:
    /**
     * @brief Hash a password using Argon2id
     * 
     * @param password Plain text password
     * @return std::string Encoded hash string in PHC format
     */
    static std::string hashPassword(const std::string& password) {
        // Argon2 parameters (moderate security for development)
        const uint32_t t_cost = 3;          // 3 iterations
        const uint32_t m_cost = 65536;      // 64 MiB memory
        const uint32_t parallelism = 4;     // 4 parallel threads
        const size_t hash_len = 32;         // 32 bytes output
        const size_t salt_len = 16;         // 16 bytes salt
        
        // Generate random salt
        std::vector<uint8_t> salt(salt_len);
        generateSalt(salt.data(), salt_len);
        
        // Encode to PHC string format (argon2id_hash_encoded does hashing + encoding)
        std::vector<char> encoded(256);
        int result = argon2id_hash_encoded(
            t_cost,
            m_cost,
            parallelism,
            password.c_str(),
            password.length(),
            salt.data(),
            salt_len,
            hash_len,
            encoded.data(),
            encoded.size()
        );
        
        if (result != ARGON2_OK) {
            throw std::runtime_error("Argon2 hashing failed: " + std::string(argon2_error_message(result)));
        }
        
        return std::string(encoded.data());
    }
    
    /**
     * @brief Verify a password against a hash
     * 
     * @param password Plain text password to verify
     * @param hash Encoded hash string from database
     * @return true if password matches
     * @return false if password doesn't match
     */
    static bool verifyPassword(const std::string& password, const std::string& hash) {
        int result = argon2id_verify(
            hash.c_str(),
            password.c_str(),
            password.length()
        );
        
        return result == ARGON2_OK;
    }
    
private:
    static void generateSalt(uint8_t* salt, size_t len) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        
        for (size_t i = 0; i < len; ++i) {
            salt[i] = static_cast<uint8_t>(dis(gen));
        }
    }
};

} // namespace auth
} // namespace netsim
