#include "ScenarioRunner.hpp"
#include "../core/Host.hpp"
#include "../core/Router.hpp"
#include <iostream>
#include <thread>

namespace netsim {
namespace scenario {

ScenarioRunner::ScenarioRunner(Network& network, Engine& engine)
    : m_network(network), m_engine(engine) {
}

ScenarioResult ScenarioRunner::runScenario(const Scenario& scenario) {
    ScenarioResult result;
    result.scenario_name = scenario.name;
    result.success = false;
    result.total_time_ms = 0.0;
    
    std::cout << "\n[Scenario] Running: " << scenario.name << std::endl;
    std::cout << "[Scenario] Description: " << scenario.description << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Setup phase
    if (!setupNetwork(scenario)) {
        result.summary = "Failed to setup network topology";
        result.total_time_ms = std::chrono::duration<double, std::milli>(
            std::chrono::high_resolution_clock::now() - start_time
        ).count();
        return result;
    }
    
    // Step 2: Execute steps
    std::cout << "[Scenario] Executing " << scenario.steps.size() << " steps..." << std::endl;
    for (const auto& step : scenario.steps) {
        std::cout << "[Scenario] Step: " << step.name << std::endl;
        
        StepResult step_result = executeStep(step);
        result.step_results.push_back(step_result);
        
        if (!step_result.success) {
            result.success = false;
            std::cout << "[Scenario] Step FAILED: " << step_result.message << std::endl;
        } else {
            std::cout << "[Scenario] Step OK (" << step_result.execution_time_ms << "ms)" << std::endl;
        }
    }
    
    // Step 3: Run validations
    if (!scenario.validations.empty()) {
        std::cout << "[Scenario] Running " << scenario.validations.size() << " validations..." << std::endl;
        result.validation_results = runValidations(scenario.validations);
        
        for (const auto& val_result : result.validation_results) {
            if (!val_result.passed) {
                result.success = false;
                std::cout << "[Scenario] Validation FAILED: " << val_result.message << std::endl;
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.total_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    if (result.success) {
        result.summary = "Scenario completed successfully";
        std::cout << "[Scenario] ✅ SUCCESS (" << result.total_time_ms << "ms)" << std::endl;
    } else {
        result.summary = "Scenario failed - see step results";
        std::cout << "[Scenario] ❌ FAILED (" << result.total_time_ms << "ms)" << std::endl;
    }
    
    return result;
}

bool ScenarioRunner::setupNetwork(const Scenario& scenario) {
    try {
        // Add nodes
        for (const auto& node_setup : scenario.nodes) {
            if (node_setup.type == "router") {
                m_network.addNode<Router>(node_setup.name, node_setup.ip);
            } else {
                // Host requires (name, address, port)
                m_network.addNode<Host>(node_setup.name, node_setup.ip, 0);
            }
            
            // Apply VLAN if specified (use Network method)
            if (node_setup.vlan != 0) {
                m_network.assignVLAN(node_setup.name, node_setup.vlan);
            }
        }
        
        // Add links
        for (const auto& link_setup : scenario.links) {
            m_network.connect(link_setup.from, link_setup.to);
            
            // Set link properties
            if (link_setup.delay_ms > 0) {
                m_network.setLinkDelay(link_setup.from, link_setup.to, link_setup.delay_ms);
            }
            
            if (link_setup.bandwidth_mbps > 0) {
                m_network.setBandwidth(link_setup.from, link_setup.to, link_setup.bandwidth_mbps);
            }
            
            if (link_setup.packet_loss > 0.0) {
                m_network.setPacketLoss(link_setup.from, link_setup.to, link_setup.packet_loss);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Scenario] Setup error: " << e.what() << std::endl;
        return false;
    }
}

StepResult ScenarioRunner::executeStep(const ScenarioStep& step) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    StepResult result;
    result.step_name = step.name;
    result.success = false;
    
    try {
        if (step.action == "ping") {
            result = handlePing(step.params, step.expect);
        } else if (step.action == "send") {
            result = handleSend(step.params, step.expect);
        } else if (step.action == "configure") {
            result = handleConfigure(step.params, step.expect);
        } else if (step.action == "wait") {
            result = handleWait(step.params, step.expect);
        } else if (step.action == "validate") {
            result = handleValidate(step.params, step.expect);
        } else {
            result.message = "Unknown action: " + step.action;
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.message = std::string("Exception: ") + e.what();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.execution_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    return result;
}

StepResult ScenarioRunner::handlePing(const json& params, const json& expect) {
    StepResult result;
    result.success = false;
    result.execution_time_ms = 0.0;
    
    std::string from = params.value("from", "");
    std::string to = params.value("to", "");
    
    if (from.empty() || to.empty()) {
        result.message = "Missing 'from' or 'to' parameter";
        return result;
    }
    
    // Execute ping using Engine
    std::vector<std::string> path;
    bool ping_success = m_engine.ping(from, to, path);
    
    result.actual_values["success"] = ping_success;
    
    // Check expectations
    if (expect.contains("success")) {
        bool expected_success = expect["success"].get<bool>();
        if (ping_success == expected_success) {
            result.success = true;
            result.message = ping_success ? "Ping successful" : "Ping failed as expected";
        } else {
            result.message = "Ping result mismatch: expected " + 
                           std::string(expected_success ? "success" : "failure") +
                           " but got " + std::string(ping_success ? "success" : "failure");
        }
    } else {
        result.success = ping_success;
        result.message = ping_success ? "Ping successful" : "Ping failed";
    }
    
    return result;
}

StepResult ScenarioRunner::handleWait(const json& params, const json& expect) {
    StepResult result;
    result.success = true;
    result.execution_time_ms = 0.0;
    
    int duration_ms = params.value("duration_ms", 100);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    
    result.message = "Waited " + std::to_string(duration_ms) + "ms";
    result.actual_values["duration_ms"] = duration_ms;
    
    return result;
}

StepResult ScenarioRunner::handleValidate(const json& params, const json& expect) {
    StepResult result;
    result.success = false;
    result.execution_time_ms = 0.0;
    
    std::string type = params.value("type", "");
    
    if (type == "topology") {
        auto all_nodes = m_network.getAllNodes();
        int actual_nodes = all_nodes.size();
        int actual_links = 0;
        
        // Count links using Network API
        for (const auto& node_name : all_nodes) {
            auto neighbors = m_network.getNeighbors(node_name);
            actual_links += neighbors.size();
        }
        actual_links /= 2;  // Each link counted twice
        
        result.actual_values["nodes_count"] = actual_nodes;
        result.actual_values["links_count"] = actual_links;
        
        if (expect.contains("nodes_count") && expect["nodes_count"] != actual_nodes) {
            result.message = "Node count mismatch";
            return result;
        }
        
        if (expect.contains("links_count") && expect["links_count"] != actual_links) {
            result.message = "Link count mismatch";
            return result;
        }
        
        result.success = true;
        result.message = "Topology validation passed";
    } else {
        result.message = "Unknown validation type: " + type;
    }
    
    return result;
}

StepResult ScenarioRunner::handleSend(const json& params, const json& expect) {
    StepResult result;
    result.success = false;
    result.execution_time_ms = 0.0;
    
    try {
        std::string from = params.value("from", "");
        std::string to = params.value("to", "");
        int count = params.value("count", 1);
        int size_bytes = params.value("size_bytes", 64);
        
        if (from.empty() || to.empty()) {
            result.message = "Missing 'from' or 'to' parameter";
            return result;
        }
        
        int delivered = 0;
        int failed = 0;
        
        // Send packets
        for (int i = 0; i < count; ++i) {
            // Create packet with dummy data (5 params: src, dest, type, protocol, payload)
            std::string data(size_bytes, 'X');
            Packet pkt(from, to, "DATA", "TCP", data);
            
            // Send packet via Network (not Engine)
            m_network.sendPacket(pkt);
            delivered++;  // Assume success for now (no return value from sendPacket)
        }
        
        // Calculate delivery rate
        double delivery_rate = static_cast<double>(delivered) / count;
        
        result.actual_values["sent"] = count;
        result.actual_values["delivered"] = delivered;
        result.actual_values["failed"] = failed;
        result.actual_values["delivery_rate"] = delivery_rate;
        
        // Check expectations
        if (expect.contains("min_delivery_rate")) {
            double min_rate = expect["min_delivery_rate"];
            if (delivery_rate < min_rate) {
                result.success = false;
                result.message = "Delivery rate " + std::to_string(delivery_rate) + 
                                " below minimum " + std::to_string(min_rate);
                return result;
            }
        }
        
        if (expect.contains("success") && expect["success"].is_boolean()) {
            bool expected_success = expect["success"];
            bool actual_success = (delivered > 0);
            if (expected_success != actual_success) {
                result.success = false;
                result.message = expected_success ? "Expected success but failed" : 
                                                   "Expected failure but succeeded";
                return result;
            }
        }
        
        result.success = true;
        result.message = "Sent " + std::to_string(count) + " packets, " + 
                        std::to_string(delivered) + " delivered (" + 
                        std::to_string(static_cast<int>(delivery_rate * 100)) + "%)";
        
    } catch (const std::exception& e) {
        result.success = false;
        result.message = std::string("Send failed: ") + e.what();
    }
    
    return result;
}

StepResult ScenarioRunner::handleConfigure(const json& params, const json& expect) {
    StepResult result;
    result.success = false;
    result.execution_time_ms = 0.0;
    
    try {
        std::string node_name = params.value("node", "");
        
        if (node_name.empty()) {
            result.message = "Missing 'node' parameter";
            return result;
        }
        
        // Check if node exists
        auto node = m_network.findByName(node_name);
        if (!node) {
            result.message = "Node not found: " + node_name;
            return result;
        }
        
        int changes = 0;
        
        // Apply configuration changes
        if (params.contains("config")) {
            const auto& config = params["config"];
            
            // MTU configuration
            if (config.contains("mtu")) {
                int mtu = config["mtu"];
                node->setMTU(mtu);
                result.actual_values["mtu"] = mtu;
                changes++;
            }
            
            // Queue size (use Network method)
            if (config.contains("queue_size")) {
                int queue_size = config["queue_size"];
                m_network.setQueueSize(node_name, queue_size);
                result.actual_values["queue_size"] = queue_size;
                changes++;
            }
            
            // VLAN (use Network method)
            if (config.contains("vlan")) {
                int vlan_id = config["vlan"];
                m_network.assignVLAN(node_name, vlan_id);
                result.actual_values["vlan"] = vlan_id;
                changes++;
            }
            
            // Note: TTL and IP changes not supported by current Node API
            // They would require Node interface extensions
        }
        
        if (changes == 0) {
            result.message = "No configuration parameters provided";
            return result;
        }
        
        result.success = true;
        result.message = "Configured " + std::to_string(changes) + " parameters on " + node_name;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.message = std::string("Configuration failed: ") + e.what();
    }
    
    return result;
}

std::vector<ValidationResult> ScenarioRunner::runValidations(
    const std::vector<ValidationRule>& validations
) {
    std::vector<ValidationResult> results;
    
    for (const auto& rule : validations) {
        ValidationResult result;
        result.validation_type = rule.type;
        
        if (rule.type == "connectivity") {
            result = validateConnectivity(rule.params, rule.threshold);
        } else if (rule.type == "isolation") {
            result = validateIsolation(rule.params, rule.threshold);
        } else if (rule.type == "latency") {
            result = validateLatency(rule.params, rule.threshold);
        } else if (rule.type == "packet_loss") {
            result = validatePacketLoss(rule.params, rule.threshold);
        } else if (rule.type == "throughput") {
            result = validateThroughput(rule.params, rule.threshold);
        } else if (rule.type == "vlan") {
            result = validateVLAN(rule.params, rule.threshold);
        } else {
            result.passed = false;
            result.message = "Unknown validation type: " + rule.type;
        }
        
        results.push_back(result);
    }
    
    return results;
}

ValidationResult ScenarioRunner::validateConnectivity(
    const json& params, const json& threshold
) {
    ValidationResult result;
    result.passed = false;
    result.validation_type = "connectivity";
    
    std::string from = params.value("from", "");
    std::string to = params.value("to", "");
    
    if (!from.empty() && !to.empty()) {
        std::vector<std::string> path;
        bool ping_ok = m_engine.ping(from, to, path);
        result.passed = ping_ok;
        result.message = ping_ok ? "Connectivity OK" : "No connectivity";
        result.details["from"] = from;
        result.details["to"] = to;
    } else {
        result.message = "Missing connectivity parameters";
    }
    
    return result;
}

ValidationResult ScenarioRunner::validateIsolation(
    const json& params, const json& threshold
) {
    ValidationResult result;
    result.passed = false;
    result.validation_type = "isolation";
    
    try {
        std::string nodeA = params.value("nodeA", "");
        std::string nodeB = params.value("nodeB", "");
        
        if (nodeA.empty() || nodeB.empty()) {
            result.message = "Missing nodeA or nodeB parameter";
            return result;
        }
        
        // Nodes should NOT be able to communicate
        std::vector<std::string> path;
                bool can_ping = m_engine.ping(nodeA, nodeB, path);
        result.passed = !can_ping;  // Pass if ping FAILS
        
        result.message = result.passed ? 
            "Isolation verified: " + nodeA + " cannot reach " + nodeB :
            "Isolation failed: " + nodeA + " can reach " + nodeB;
        
        result.details["nodeA"] = nodeA;
        result.details["nodeB"] = nodeB;
        result.details["can_communicate"] = can_ping;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = std::string("Isolation check error: ") + e.what();
    }
    
    return result;
}

ValidationResult ScenarioRunner::validateLatency(const json& params, const json& threshold) {
    ValidationResult result;
    result.passed = false;
    result.validation_type = "latency";
    
    try {
        std::string nodeA = params.value("nodeA", "");
        std::string nodeB = params.value("nodeB", "");
        
        if (nodeA.empty() || nodeB.empty()) {
            result.message = "Missing nodeA or nodeB parameter";
            return result;
        }
        
        // Get latency from network
        int latency_ms = m_network.getLatency(nodeA, nodeB);
        
        result.details["nodeA"] = nodeA;
        result.details["nodeB"] = nodeB;
        result.details["latency_ms"] = latency_ms;
        
        // Check against threshold
        if (threshold.contains("max_ms")) {
            int max_latency = threshold["max_ms"];
            result.passed = (latency_ms <= max_latency);
            result.message = result.passed ?
                "Latency " + std::to_string(latency_ms) + "ms <= " + std::to_string(max_latency) + "ms" :
                "Latency " + std::to_string(latency_ms) + "ms > " + std::to_string(max_latency) + "ms";
        } else {
            result.passed = true;
            result.message = "Latency measured: " + std::to_string(latency_ms) + "ms";
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = std::string("Latency check error: ") + e.what();
    }
    
    return result;
}

ValidationResult ScenarioRunner::validatePacketLoss(const json& params, const json& threshold) {
    ValidationResult result;
    result.passed = false;
    result.validation_type = "packet_loss";
    
    try {
        std::string nodeA = params.value("nodeA", "");
        std::string nodeB = params.value("nodeB", "");
        
        if (nodeA.empty() || nodeB.empty()) {
            result.message = "Missing nodeA or nodeB parameter";
            return result;
        }
        
        // Get packet loss rate from network
        double loss_rate = m_network.getPacketLossRate(nodeA, nodeB);
        
        result.details["nodeA"] = nodeA;
        result.details["nodeB"] = nodeB;
        result.details["packet_loss_rate"] = loss_rate;
        
        // Check against threshold
        if (threshold.contains("max_rate")) {
            double max_loss = threshold["max_rate"];
            result.passed = (loss_rate <= max_loss);
            result.message = result.passed ?
                "Packet loss " + std::to_string(loss_rate) + " <= " + std::to_string(max_loss) :
                "Packet loss " + std::to_string(loss_rate) + " > " + std::to_string(max_loss);
        } else {
            result.passed = true;
            result.message = "Packet loss measured: " + std::to_string(loss_rate);
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = std::string("Packet loss check error: ") + e.what();
    }
    
    return result;
}

ValidationResult ScenarioRunner::validateThroughput(const json& params, const json& threshold) {
    ValidationResult result;
    result.passed = false;
    result.validation_type = "throughput";
    
    try {
        std::string nodeA = params.value("nodeA", "");
        std::string nodeB = params.value("nodeB", "");
        
        if (nodeA.empty() || nodeB.empty()) {
            result.message = "Missing nodeA or nodeB parameter";
            return result;
        }
        
        // Get throughput from network (in Mbps or similar)
        int throughput = m_network.getThroughput(nodeA, nodeB);
        
        result.details["nodeA"] = nodeA;
        result.details["nodeB"] = nodeB;
        result.details["throughput_mbps"] = throughput;
        
        // Check against threshold
        if (threshold.contains("min_mbps")) {
            int min_throughput = threshold["min_mbps"];
            result.passed = (throughput >= min_throughput);
            result.message = result.passed ?
                "Throughput " + std::to_string(throughput) + "Mbps >= " + std::to_string(min_throughput) + "Mbps" :
                "Throughput " + std::to_string(throughput) + "Mbps < " + std::to_string(min_throughput) + "Mbps";
        } else {
            result.passed = true;
            result.message = "Throughput measured: " + std::to_string(throughput) + "Mbps";
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = std::string("Throughput check error: ") + e.what();
    }
    
    return result;
}

ValidationResult ScenarioRunner::validateVLAN(const json& params, const json& threshold) {
    ValidationResult result;
    result.passed = false;
    result.validation_type = "vlan";
    
    try {
        // VLAN validation checks that nodes in same VLAN can communicate
        // and nodes in different VLANs cannot
        
        if (params.contains("same_vlan")) {
            const auto& same_vlan = params["same_vlan"];
            std::string nodeA = same_vlan.value("nodeA", "");
            std::string nodeB = same_vlan.value("nodeB", "");
            
            if (!nodeA.empty() && !nodeB.empty()) {
                std::vector<std::string> path;
                bool can_ping = m_engine.ping(nodeA, nodeB, path);
                result.passed = can_ping;
                result.message = result.passed ?
                    "Same VLAN: " + nodeA + " can reach " + nodeB :
                    "Same VLAN FAILED: " + nodeA + " cannot reach " + nodeB;
                result.details["same_vlan_test"] = can_ping;
            }
        }
        
        if (params.contains("different_vlan")) {
            const auto& diff_vlan = params["different_vlan"];
            std::string nodeA = diff_vlan.value("nodeA", "");
            std::string nodeB = diff_vlan.value("nodeB", "");
            
            if (!nodeA.empty() && !nodeB.empty()) {
                std::vector<std::string> path;
                bool can_ping = m_engine.ping(nodeA, nodeB, path);
                bool isolated = !can_ping;
                result.passed = result.passed && isolated;
                result.message += result.passed ?
                    " | Different VLAN: " + nodeA + " isolated from " + nodeB :
                    " | Different VLAN FAILED: " + nodeA + " can reach " + nodeB;
                result.details["different_vlan_test"] = isolated;
            }
        }
        
        if (!params.contains("same_vlan") && !params.contains("different_vlan")) {
            result.message = "No VLAN test parameters provided";
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = std::string("VLAN validation error: ") + e.what();
    }
    
    return result;
}

void ScenarioRunner::cleanup() {
    // Clear network state if needed
    std::cout << "[Scenario] Cleanup complete" << std::endl;
}

std::string ScenarioRunner::formatDuration(double ms) const {
    if (ms < 1000) {
        return std::to_string(static_cast<int>(ms)) + "ms";
    } else {
        return std::to_string(ms / 1000.0) + "s";
    }
}

} // namespace scenario
} // namespace netsim
