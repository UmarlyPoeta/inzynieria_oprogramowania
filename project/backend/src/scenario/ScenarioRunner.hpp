#pragma once

#include "ScenarioTypes.hpp"
#include "../core/Network.hpp"
#include "../core/Engine.hpp"
#include <memory>
#include <chrono>

namespace netsim {
namespace scenario {

/**
 * @brief Executes network scenarios step by step
 * 
 * ScenarioRunner sets up the network, executes actions,
 * and validates outcomes according to scenario definition.
 */
class ScenarioRunner {
public:
    ScenarioRunner(Network& network, Engine& engine);
    
    /**
     * @brief Run a complete scenario
     * 
     * @param scenario Scenario to execute
     * @return ScenarioResult with execution details
     */
    ScenarioResult runScenario(const Scenario& scenario);
    
    /**
     * @brief Set up network topology from scenario
     * 
     * @param scenario Scenario containing setup info
     * @return true if setup successful
     */
    bool setupNetwork(const Scenario& scenario);
    
    /**
     * @brief Execute a single scenario step
     * 
     * @param step Step to execute
     * @return StepResult with execution details
     */
    StepResult executeStep(const ScenarioStep& step);
    
    /**
     * @brief Run all validations
     * 
     * @param validations Validation rules to check
     * @return Vector of validation results
     */
    std::vector<ValidationResult> runValidations(
        const std::vector<ValidationRule>& validations
    );
    
    /**
     * @brief Clean up network state
     */
    void cleanup();
    
private:
    Network& m_network;
    Engine& m_engine;
    
    // Action handlers
    StepResult handlePing(const json& params, const json& expect);
    StepResult handleSend(const json& params, const json& expect);
    StepResult handleConfigure(const json& params, const json& expect);
    StepResult handleWait(const json& params, const json& expect);
    StepResult handleValidate(const json& params, const json& expect);
    
    // Validation handlers
    ValidationResult validateConnectivity(const json& params, const json& threshold);
    ValidationResult validateLatency(const json& params, const json& threshold);
    ValidationResult validatePacketLoss(const json& params, const json& threshold);
    ValidationResult validateThroughput(const json& params, const json& threshold);
    ValidationResult validateIsolation(const json& params, const json& threshold);
    ValidationResult validateTopology(const json& params, const json& threshold);
    ValidationResult validateVLAN(const json& params, const json& threshold);
    
    // Helper methods
    bool checkExpectations(const json& expect, const json& actual);
    std::string formatDuration(double ms) const;
};

} // namespace scenario
} // namespace netsim
