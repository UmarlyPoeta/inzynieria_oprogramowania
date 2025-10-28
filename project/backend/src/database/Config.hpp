#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>

namespace netsim {
namespace db {

/**
 * @brief Configuration loader for .env files
 */
class Config {
public:
    /**
     * @brief Load configuration from .env file
     * @param envFilePath Path to .env file (default: project/database/.env)
     */
    static void load(const std::string& envFilePath = "../database/.env") {
        std::ifstream file(envFilePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open .env file: " + envFilePath);
        }

        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // Parse KEY=VALUE
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                config[key] = value;
            }
        }
        file.close();
    }

    /**
     * @brief Get configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value
     */
    static std::string get(const std::string& key, const std::string& defaultValue = "") {
        auto it = config.find(key);
        if (it != config.end()) {
            return it->second;
        }
        return defaultValue;
    }

    /**
     * @brief Get integer configuration value
     */
    static int getInt(const std::string& key, int defaultValue = 0) {
        auto it = config.find(key);
        if (it != config.end()) {
            return std::stoi(it->second);
        }
        return defaultValue;
    }

private:
    static std::map<std::string, std::string> config;
};

// Static member initialization
std::map<std::string, std::string> Config::config;

} // namespace db
} // namespace netsim

#endif // CONFIG_HPP
