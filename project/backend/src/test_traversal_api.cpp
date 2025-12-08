#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "core/Engine.hpp"
#include "core/Network.hpp"
#include "core/Host.hpp"
#include "core/Router.hpp"
#include <iostream>
#include <memory>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

// Helper: konwersja vector<string> do JSON array
web::json::value string_vector_to_json(const std::vector<std::string>& vec) {
    auto arr = web::json::value::array();
    for (size_t i = 0; i < vec.size(); ++i) {
        arr[i] = web::json::value::string(utility::conversions::to_string_t(vec[i]));
    }
    return arr;
}

// Helper: konwersja JSON array do vector<string>
std::vector<std::string> json_to_string_vector(const web::json::value& arr) {
    std::vector<std::string> result;
    if (arr.is_array()) {
        for (const auto& item : arr.as_array()) {
            result.push_back(utility::conversions::to_utf8string(item.as_string()));
        }
    }
    return result;
}

// Helper: konwersja mapy string->string do JSON
web::json::value string_map_to_json(const std::unordered_map<std::string, std::string>& map) {
    auto obj = web::json::value::object();
    for (const auto& [key, value] : map) {
        obj[utility::conversions::to_string_t(key)] = web::json::value::string(utility::conversions::to_string_t(value));
    }
    return obj;
}

class TraversalTestAPI {
private:
    std::unique_ptr<http_listener> listener;
    Network& network;
    Engine& engine;

public:
    TraversalTestAPI(const std::string& url, Network& net, Engine& eng)
        : network(net), engine(eng) {
        listener = std::make_unique<http_listener>(utility::conversions::to_string_t(url));
        listener->support(methods::POST, std::bind(&TraversalTestAPI::handle_post, this, std::placeholders::_1));
        listener->support(methods::GET, std::bind(&TraversalTestAPI::handle_get, this, std::placeholders::_1));
    }

    void start() {
        listener->open().wait();
        std::cout << "[TEST API] Traversal algorithms test API started\n";
    }

    void stop() {
        listener->close().wait();
        std::cout << "[TEST API] Traversal algorithms test API stopped\n";
    }

private:
    void handle_get(http_request request) {
        auto path = uri::decode(request.relative_uri().path());
        
        if (path == U("/test/traversal/help")) {
            web::json::value help = web::json::value::object();
            help[U("endpoints")] = web::json::value::array();
            int idx = 0;
            
            help[U("endpoints")][idx++] = web::json::value::string(U("POST /test/traversal/ecmp/next-hops - ECMP next hops"));
            help[U("endpoints")][idx++] = web::json::value::string(U("POST /test/traversal/ecmp/k-paths - ECMP k paths"));
            help[U("endpoints")][idx++] = web::json::value::string(U("POST /test/traversal/constrained-path - Constrained shortest path"));
            help[U("endpoints")][idx++] = web::json::value::string(U("POST /test/traversal/link-state - Link-state routing"));
            help[U("endpoints")][idx++] = web::json::value::string(U("POST /test/traversal/rpf - Reverse Path Forwarding check"));
            help[U("endpoints")][idx++] = web::json::value::string(U("POST /test/traversal/multipath - Multipath flow-aware"));
            help[U("endpoints")][idx++] = web::json::value::string(U("POST /test/traversal/multi-commodity - Multi-commodity flow"));
            
            request.reply(status_codes::OK, help);
        } else {
            web::json::value error;
            error[U("error")] = web::json::value::string(U("Unknown endpoint. Try GET /test/traversal/help"));
            request.reply(status_codes::NotFound, error);
        }
    }

    void handle_post(http_request request) {
        auto path = uri::decode(request.relative_uri().path());

        // ECMP: Equal-Cost Next Hops
        if (path == U("/test/traversal/ecmp/next-hops")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string node = utility::conversions::to_utf8string(jv[U("node")].as_string());
                    std::string dst = utility::conversions::to_utf8string(jv[U("dst")].as_string());
                    
                    auto nextHops = engine.getEqualCostNextHops(node, dst);
                    
                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(true);
                    resp[U("node")] = jv[U("node")];
                    resp[U("destination")] = jv[U("dst")];
                    resp[U("next_hops")] = string_vector_to_json(nextHops);
                    resp[U("count")] = web::json::value::number((int)nextHops.size());
                    
                    request.reply(status_codes::OK, resp);
                } catch (const std::exception& e) {
                    web::json::value error;
                    error[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, error);
                }
            }).wait();

        // ECMP: K Equal-Cost Paths
        } else if (path == U("/test/traversal/ecmp/k-paths")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string src = utility::conversions::to_utf8string(jv[U("src")].as_string());
                    std::string dst = utility::conversions::to_utf8string(jv[U("dst")].as_string());
                    int k = jv.has_field(U("k")) ? jv[U("k")].as_integer() : 4;
                    
                    // Wywołaj przez wrapper Engine (jeśli istnieje) lub bezpośrednio traversal
                    // Dla uproszczenia zakładam że trzeba dodać wrapper w Engine
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(U("Not implemented: add Engine::getECMPKPaths wrapper"));
                    request.reply(status_codes::NotImplemented, resp);
                    
                } catch (const std::exception& e) {
                    web::json::value error;
                    error[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, error);
                }
            }).wait();

        // Constrained Shortest Path
        } else if (path == U("/test/traversal/constrained-path")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string src = utility::conversions::to_utf8string(jv[U("src")].as_string());
                    std::string dst = utility::conversions::to_utf8string(jv[U("dst")].as_string());
                    int minBandwidth = jv[U("min_bandwidth")].as_integer();
                    
                    std::vector<std::string> path;
                    bool found = engine.constrainedShortestPath(src, dst, path, minBandwidth);
                    
                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(found);
                    resp[U("path")] = string_vector_to_json(path);
                    resp[U("hops")] = web::json::value::number((int)path.size());
                    resp[U("min_bandwidth")] = web::json::value::number(minBandwidth);
                    
                    if (found) {
                        int totalDelay = engine.getTotalDelay(path);
                        resp[U("total_delay")] = web::json::value::number(totalDelay);
                    }
                    
                    request.reply(status_codes::OK, resp);
                } catch (const std::exception& e) {
                    web::json::value error;
                    error[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, error);
                }
            }).wait();

        // Link-State Routing
        } else if (path == U("/test/traversal/link-state")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string node = utility::conversions::to_utf8string(jv[U("node")].as_string());
                    
                    auto routingTable = engine.computeLinkStateRouting(node);
                    
                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(true);
                    resp[U("node")] = jv[U("node")];
                    resp[U("routing_table")] = string_map_to_json(routingTable);
                    resp[U("destinations_count")] = web::json::value::number((int)routingTable.size());
                    
                    request.reply(status_codes::OK, resp);
                } catch (const std::exception& e) {
                    web::json::value error;
                    error[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, error);
                }
            }).wait();

        // Reverse Path Forwarding Check
        } else if (path == U("/test/traversal/rpf")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string node = utility::conversions::to_utf8string(jv[U("node")].as_string());
                    std::string incomingNeighbor = utility::conversions::to_utf8string(jv[U("incoming_neighbor")].as_string());
                    std::string src = utility::conversions::to_utf8string(jv[U("src")].as_string());
                    
                    bool isValid = engine.isRPF(node, incomingNeighbor, src);
                    
                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(true);
                    resp[U("node")] = jv[U("node")];
                    resp[U("incoming_neighbor")] = jv[U("incoming_neighbor")];
                    resp[U("packet_source")] = jv[U("src")];
                    resp[U("rpf_check")] = web::json::value::boolean(isValid);
                    resp[U("action")] = web::json::value::string(isValid ? U("ACCEPT") : U("REJECT"));
                    
                    request.reply(status_codes::OK, resp);
                } catch (const std::exception& e) {
                    web::json::value error;
                    error[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, error);
                }
            }).wait();

        // Multipath Flow-Aware
        } else if (path == U("/test/traversal/multipath")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string src = utility::conversions::to_utf8string(jv[U("src")].as_string());
                    std::string dst = utility::conversions::to_utf8string(jv[U("dst")].as_string());
                    int k = jv.has_field(U("k")) ? jv[U("k")].as_integer() : 4;
                    
                    auto paths = engine.multipathFlowAware(src, dst, k);
                    
                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(true);
                    resp[U("paths_found")] = web::json::value::number((int)paths.size());
                    
                    auto pathsArray = web::json::value::array();
                    for (size_t i = 0; i < paths.size(); ++i) {
                        auto pathObj = web::json::value::object();
                        pathObj[U("path")] = string_vector_to_json(paths[i].first);
                        pathObj[U("weight")] = web::json::value::number(paths[i].second);
                        pathObj[U("hops")] = web::json::value::number((int)paths[i].first.size());
                        pathsArray[i] = pathObj;
                    }
                    resp[U("paths")] = pathsArray;
                    
                    request.reply(status_codes::OK, resp);
                } catch (const std::exception& e) {
                    web::json::value error;
                    error[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, error);
                }
            }).wait();

        // Multi-Commodity Flow
        } else if (path == U("/test/traversal/multi-commodity")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto commoditiesJson = jv[U("commodities")].as_array();
                    int k = jv.has_field(U("k")) ? jv[U("k")].as_integer() : 4;
                    
                    std::vector<std::tuple<std::string, std::string, int>> commodities;
                    for (const auto& comm : commoditiesJson) {
                        std::string src = utility::conversions::to_utf8string(comm.at(U("src")).as_string());
                        std::string dst = utility::conversions::to_utf8string(comm.at(U("dst")).as_string());
                        int demand = comm.at(U("demand")).as_integer();
                        commodities.push_back({src, dst, demand});
                    }
                    
                    auto allocation = engine.multiCommodityFlow(commodities, k);
                    
                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(true);
                    resp[U("commodities_processed")] = web::json::value::number((int)allocation.size());
                    
                    auto allocArray = web::json::value::array();
                    int idx = 0;
                    for (const auto& [key, pathFlows] : allocation) {
                        auto allocObj = web::json::value::object();
                        allocObj[U("commodity")] = web::json::value::string(utility::conversions::to_string_t(key));
                        
                        auto flowsArray = web::json::value::array();
                        for (size_t i = 0; i < pathFlows.size(); ++i) {
                            auto flowObj = web::json::value::object();
                            flowObj[U("path")] = string_vector_to_json(pathFlows[i].first);
                            flowObj[U("flow")] = web::json::value::number(pathFlows[i].second);
                            flowsArray[i] = flowObj;
                        }
                        allocObj[U("allocations")] = flowsArray;
                        allocObj[U("paths_count")] = web::json::value::number((int)pathFlows.size());
                        
                        allocArray[idx++] = allocObj;
                    }
                    resp[U("allocations")] = allocArray;
                    
                    request.reply(status_codes::OK, resp);
                } catch (const std::exception& e) {
                    web::json::value error;
                    error[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, error);
                }
            }).wait();

        } else {
            web::json::value error;
            error[U("error")] = web::json::value::string(U("Unknown endpoint. Try GET /test/traversal/help"));
            request.reply(status_codes::NotFound, error);
        }
    }
};

// Przykładowy main do testowania (można wyłączyć w testach jednostkowych)
#ifndef NO_MAIN
int main() {
    try {
        // Stwórz prostą sieć testową
        Network network;
        
        // Topologia: A -- B -- C
        //            |    |    |
        //            D -- E -- F
        network.addNode<Host>("A", "10.0.0.1", 8001);
        network.addNode<Host>("B", "10.0.0.2", 8002);
        network.addNode<Host>("C", "10.0.0.3", 8003);
        network.addNode<Host>("D", "10.0.0.4", 8004);
        network.addNode<Host>("E", "10.0.0.5", 8005);
        network.addNode<Host>("F", "10.0.0.6", 8006);
        
        network.connect("A", "B");
        network.connect("B", "C");
        network.connect("A", "D");
        network.connect("B", "E");
        network.connect("C", "F");
        network.connect("D", "E");
        network.connect("E", "F");
        
        // Ustaw opóźnienia
        network.setLinkDelay("A", "B", 10);
        network.setLinkDelay("B", "C", 10);
        network.setLinkDelay("A", "D", 15);
        network.setLinkDelay("B", "E", 5);
        network.setLinkDelay("C", "F", 10);
        network.setLinkDelay("D", "E", 10);
        network.setLinkDelay("E", "F", 5);
        
        // Ustaw bandwidth
        network.setBandwidth("A", "B", 100);
        network.setBandwidth("B", "C", 100);
        network.setBandwidth("A", "D", 50);
        network.setBandwidth("B", "E", 150);
        network.setBandwidth("C", "F", 100);
        network.setBandwidth("D", "E", 75);
        network.setBandwidth("E", "F", 200);
        
        Engine engine(network);
        
        // Uruchom API na porcie 8081
        std::string url = "http://0.0.0.0:8081/test/traversal";
        TraversalTestAPI api(url, network, engine);
        
        std::cout << "=== Traversal Algorithms Test API ===" << std::endl;
        std::cout << "Starting server on " << url << std::endl;
        std::cout << "Test network topology created (6 nodes: A-F)" << std::endl;
        std::cout << "\nAvailable endpoints:" << std::endl;
        std::cout << "  GET  " << url << "/help" << std::endl;
        std::cout << "  POST " << url << "/ecmp/next-hops" << std::endl;
        std::cout << "  POST " << url << "/ecmp/k-paths" << std::endl;
        std::cout << "  POST " << url << "/constrained-path" << std::endl;
        std::cout << "  POST " << url << "/link-state" << std::endl;
        std::cout << "  POST " << url << "/rpf" << std::endl;
        std::cout << "  POST " << url << "/multipath" << std::endl;
        std::cout << "  POST " << url << "/multi-commodity" << std::endl;
        std::cout << "\nPress Ctrl+C to stop..." << std::endl;
        
        api.start();
        
        // Czekaj na Ctrl+C
        std::string input;
        std::getline(std::cin, input);
        
        api.stop();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
#endif
