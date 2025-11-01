#pragma once

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace netsim {
namespace scenario {

using json = nlohmann::json;

/**
 * @brief Node setup configuration
 */
struct NodeSetup {
    std::string name;
    std::string type;  // "host" or "router"
    std::string ip;
    int vlan = 0;  // 0 means no VLAN
    json config;  // Additional configuration
};

/**
 * @brief Link setup configuration
 */
struct LinkSetup {
    std::string from;
    std::string to;
    int delay_ms = 0;
    int bandwidth_mbps = 1000;
    double packet_loss = 0.0;
};

/**
 * @brief Scenario step action
 */
struct ScenarioStep {
    std::string name;
    std::string action;  // "ping", "send", "configure", "wait", "validate"
    json params;
    json expect;
};

/**
 * @brief Validation rule
 */
struct ValidationRule {
    std::string type;  // "connectivity", "latency", "packet_loss", "throughput", "isolation"
    json params;
    json threshold;
};

/**
 * @brief Complete scenario definition
 */
struct Scenario {
    std::string name;
    std::string description;
    std::string version;
    std::string author;
    std::vector<std::string> tags;
    
    // Setup
    std::vector<NodeSetup> nodes;
    std::vector<LinkSetup> links;
    
    // Execution
    std::vector<ScenarioStep> steps;
    
    // Validation
    std::vector<ValidationRule> validations;
    std::string expected_outcome;
    
    /**
     * @brief Load scenario from YAML file
     */
    static Scenario fromYAML(const std::string& yaml_content);
    
    /**
     * @brief Load scenario from JSON
     */
    static Scenario fromJSON(const json& j);
    
    /**
     * @brief Convert scenario to JSON
     */
    json toJSON() const;
};

/**
 * @brief Result of a scenario step execution
 */
struct StepResult {
    std::string step_name;
    bool success;
    std::string message;
    json actual_values;
    double execution_time_ms;
};

/**
 * @brief Result of validation
 */
struct ValidationResult {
    std::string validation_type;
    bool passed;
    std::string message;
    json details;
};

/**
 * @brief Complete scenario execution result
 */
struct ScenarioResult {
    std::string scenario_name;
    bool success;
    std::vector<StepResult> step_results;
    std::vector<ValidationResult> validation_results;
    double total_time_ms;
    std::string summary;
    
    /**
     * @brief Convert result to JSON for API response
     */
    json toJSON() const;
};

} // namespace scenario
} // namespace netsim
