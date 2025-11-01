#include "ScenarioTypes.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

#ifdef HAVE_YAML_SUPPORT
#include <yaml-cpp/yaml.h>
#endif

namespace netsim {
namespace scenario {

Scenario Scenario::fromYAML(const std::string& yaml_content) {
#ifdef HAVE_YAML_SUPPORT
    try {
        YAML::Node root = YAML::Load(yaml_content);
        
        // Convert YAML to JSON for unified processing
        json j;
        
        // Basic metadata
        if (root["name"]) j["name"] = root["name"].as<std::string>();
        if (root["description"]) j["description"] = root["description"].as<std::string>();
        if (root["version"]) j["version"] = root["version"].as<std::string>();
        if (root["author"]) j["author"] = root["author"].as<std::string>();
        
        // Tags
        if (root["tags"] && root["tags"].IsSequence()) {
            j["tags"] = json::array();
            for (const auto& tag : root["tags"]) {
                j["tags"].push_back(tag.as<std::string>());
            }
        }
        
        // Setup - Nodes
        if (root["setup"] && root["setup"]["nodes"]) {
            j["setup"]["nodes"] = json::array();
            for (const auto& node : root["setup"]["nodes"]) {
                json node_j;
                if (node["name"]) node_j["name"] = node["name"].as<std::string>();
                if (node["type"]) node_j["type"] = node["type"].as<std::string>();
                if (node["ip"]) node_j["ip"] = node["ip"].as<std::string>();
                if (node["vlan"]) node_j["vlan"] = node["vlan"].as<int>();
                
                if (node["config"]) {
                    node_j["config"] = json::object();
                    for (const auto& kv : node["config"]) {
                        std::string key = kv.first.as<std::string>();
                        if (kv.second.IsScalar()) {
                            // Try to parse as different types
                            try {
                                node_j["config"][key] = kv.second.as<int>();
                            } catch (...) {
                                node_j["config"][key] = kv.second.as<std::string>();
                            }
                        }
                    }
                }
                j["setup"]["nodes"].push_back(node_j);
            }
        }
        
        // Setup - Links
        if (root["setup"] && root["setup"]["links"]) {
            j["setup"]["links"] = json::array();
            for (const auto& link : root["setup"]["links"]) {
                json link_j;
                if (link["from"]) link_j["from"] = link["from"].as<std::string>();
                if (link["to"]) link_j["to"] = link["to"].as<std::string>();
                if (link["delay_ms"]) link_j["delay_ms"] = link["delay_ms"].as<int>();
                if (link["bandwidth_mbps"]) link_j["bandwidth_mbps"] = link["bandwidth_mbps"].as<int>();
                if (link["packet_loss"]) link_j["packet_loss"] = link["packet_loss"].as<double>();
                j["setup"]["links"].push_back(link_j);
            }
        }
        
        // Steps
        if (root["steps"] && root["steps"].IsSequence()) {
            j["steps"] = json::array();
            for (const auto& step : root["steps"]) {
                json step_j;
                if (step["name"]) step_j["name"] = step["name"].as<std::string>();
                if (step["action"]) step_j["action"] = step["action"].as<std::string>();
                
                if (step["params"]) {
                    step_j["params"] = json::object();
                    for (const auto& kv : step["params"]) {
                        std::string key = kv.first.as<std::string>();
                        if (kv.second.IsScalar()) {
                            try {
                                step_j["params"][key] = kv.second.as<int>();
                            } catch (...) {
                                try {
                                    step_j["params"][key] = kv.second.as<bool>();
                                } catch (...) {
                                    step_j["params"][key] = kv.second.as<std::string>();
                                }
                            }
                        }
                    }
                }
                
                if (step["expect"]) {
                    step_j["expect"] = json::object();
                    for (const auto& kv : step["expect"]) {
                        std::string key = kv.first.as<std::string>();
                        if (kv.second.IsScalar()) {
                            try {
                                step_j["expect"][key] = kv.second.as<bool>();
                            } catch (...) {
                                step_j["expect"][key] = kv.second.as<std::string>();
                            }
                        }
                    }
                }
                
                j["steps"].push_back(step_j);
            }
        }
        
        // Validation
        if (root["validation"] && root["validation"].IsSequence()) {
            j["validation"] = json::array();
            for (const auto& val : root["validation"]) {
                json val_j;
                if (val["type"]) val_j["type"] = val["type"].as<std::string>();
                
                if (val["params"]) {
                    val_j["params"] = json::object();
                    for (const auto& kv : val["params"]) {
                        std::string key = kv.first.as<std::string>();
                        val_j["params"][key] = kv.second.as<std::string>();
                    }
                }
                
                if (val["threshold"]) {
                    val_j["threshold"] = json::object();
                    for (const auto& kv : val["threshold"]) {
                        std::string key = kv.first.as<std::string>();
                        try {
                            val_j["threshold"][key] = kv.second.as<int>();
                        } catch (...) {
                            try {
                                val_j["threshold"][key] = kv.second.as<double>();
                            } catch (...) {
                                val_j["threshold"][key] = kv.second.as<std::string>();
                            }
                        }
                    }
                }
                
                j["validation"].push_back(val_j);
            }
        }
        
        if (root["expected_outcome"]) {
            j["expected_outcome"] = root["expected_outcome"].as<std::string>();
        }
        
        // Now use the JSON parser
        return fromJSON(j);
        
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("YAML parsing error: " + std::string(e.what()));
    }
#else
    throw std::runtime_error("YAML parsing not available. Rebuild with yaml-cpp support.");
#endif
}

Scenario Scenario::fromJSON(const json& j) {
    Scenario scenario;
    
    // Basic metadata
    scenario.name = j.value("name", "Unnamed Scenario");
    scenario.description = j.value("description", "");
    scenario.version = j.value("version", "1.0");
    scenario.author = j.value("author", "Unknown");
    
    if (j.contains("tags") && j["tags"].is_array()) {
        for (const auto& tag : j["tags"]) {
            scenario.tags.push_back(tag.get<std::string>());
        }
    }
    
    // Setup - Nodes
    if (j.contains("setup") && j["setup"].contains("nodes")) {
        for (const auto& node_json : j["setup"]["nodes"]) {
            NodeSetup node;
            node.name = node_json.value("name", "");
            node.type = node_json.value("type", "host");
            node.ip = node_json.value("ip", "");
            node.vlan = node_json.value("vlan", 0);
            
            if (node_json.contains("config")) {
                node.config = node_json["config"];
            }
            
            scenario.nodes.push_back(node);
        }
    }
    
    // Setup - Links
    if (j.contains("setup") && j["setup"].contains("links")) {
        for (const auto& link_json : j["setup"]["links"]) {
            LinkSetup link;
            link.from = link_json.value("from", "");
            link.to = link_json.value("to", "");
            link.delay_ms = link_json.value("delay_ms", 0);
            link.bandwidth_mbps = link_json.value("bandwidth_mbps", 1000);
            link.packet_loss = link_json.value("packet_loss", 0.0);
            
            scenario.links.push_back(link);
        }
    }
    
    // Steps
    if (j.contains("steps") && j["steps"].is_array()) {
        for (const auto& step_json : j["steps"]) {
            ScenarioStep step;
            step.name = step_json.value("name", "");
            step.action = step_json.value("action", "");
            
            if (step_json.contains("params")) {
                step.params = step_json["params"];
            }
            
            if (step_json.contains("expect")) {
                step.expect = step_json["expect"];
            }
            
            scenario.steps.push_back(step);
        }
    }
    
    // Validation
    if (j.contains("validation") && j["validation"].is_array()) {
        for (const auto& val_json : j["validation"]) {
            ValidationRule rule;
            rule.type = val_json.value("type", "");
            
            if (val_json.contains("params")) {
                rule.params = val_json["params"];
            }
            
            if (val_json.contains("threshold")) {
                rule.threshold = val_json["threshold"];
            }
            
            scenario.validations.push_back(rule);
        }
    }
    
    scenario.expected_outcome = j.value("expected_outcome", "");
    
    return scenario;
}

json Scenario::toJSON() const {
    json j;
    
    j["name"] = name;
    j["description"] = description;
    j["version"] = version;
    j["author"] = author;
    j["tags"] = tags;
    
    // Setup
    json setup;
    json nodes_array = json::array();
    for (const auto& node : nodes) {
        json node_json;
        node_json["name"] = node.name;
        node_json["type"] = node.type;
        node_json["ip"] = node.ip;
        if (node.vlan != 0) {
            node_json["vlan"] = node.vlan;
        }
        if (!node.config.is_null()) {
            node_json["config"] = node.config;
        }
        nodes_array.push_back(node_json);
    }
    setup["nodes"] = nodes_array;
    
    json links_array = json::array();
    for (const auto& link : links) {
        json link_json;
        link_json["from"] = link.from;
        link_json["to"] = link.to;
        link_json["delay_ms"] = link.delay_ms;
        link_json["bandwidth_mbps"] = link.bandwidth_mbps;
        link_json["packet_loss"] = link.packet_loss;
        links_array.push_back(link_json);
    }
    setup["links"] = links_array;
    
    j["setup"] = setup;
    
    // Steps
    json steps_array = json::array();
    for (const auto& step : steps) {
        json step_json;
        step_json["name"] = step.name;
        step_json["action"] = step.action;
        step_json["params"] = step.params;
        step_json["expect"] = step.expect;
        steps_array.push_back(step_json);
    }
    j["steps"] = steps_array;
    
    // Validation
    json val_array = json::array();
    for (const auto& rule : validations) {
        json rule_json;
        rule_json["type"] = rule.type;
        rule_json["params"] = rule.params;
        rule_json["threshold"] = rule.threshold;
        val_array.push_back(rule_json);
    }
    j["validation"] = val_array;
    
    j["expected_outcome"] = expected_outcome;
    
    return j;
}

json ScenarioResult::toJSON() const {
    json j;
    
    j["scenario_name"] = scenario_name;
    j["success"] = success;
    j["total_time_ms"] = total_time_ms;
    j["summary"] = summary;
    
    // Step results
    json steps_array = json::array();
    for (const auto& step : step_results) {
        json step_json;
        step_json["step_name"] = step.step_name;
        step_json["success"] = step.success;
        step_json["message"] = step.message;
        step_json["actual_values"] = step.actual_values;
        step_json["execution_time_ms"] = step.execution_time_ms;
        steps_array.push_back(step_json);
    }
    j["step_results"] = steps_array;
    
    // Validation results
    json val_array = json::array();
    for (const auto& val : validation_results) {
        json val_json;
        val_json["validation_type"] = val.validation_type;
        val_json["passed"] = val.passed;
        val_json["message"] = val.message;
        val_json["details"] = val.details;
        val_array.push_back(val_json);
    }
    j["validation_results"] = val_array;
    
    return j;
}

} // namespace scenario
} // namespace netsim
