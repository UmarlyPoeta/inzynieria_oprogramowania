#include <iostream>
#include <memory>
#include <future>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cstdlib>

#include "core/Network.hpp"
#include "core/Engine.hpp"
#include "core/Host.hpp"
#include "core/Router.hpp"
#include "utils/JsonAdapter.hpp"

#ifdef HAVE_WEBSOCKET_SUPPORT
#include "websocket/WebSocketServer.hpp"
#include "websocket/EventBroadcaster.hpp"
#endif

#include "auth/PasswordHasher.hpp"
#include "auth/JWTManager.hpp"
#include "auth/RedisClient.hpp"
#include "auth/AuthService.hpp"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

#ifdef HAVE_MYSQL_SUPPORT
using namespace netsim::auth;
#endif

// Helper function to convert string vector to JSON array
web::json::value string_vector_to_json(const std::vector<std::string>& vec) {
    web::json::value arr = web::json::value::array();
    for (size_t i = 0; i < vec.size(); ++i) {
        arr[i] = web::json::value::string(utility::conversions::to_string_t(vec[i]));
    }
    return arr;
}

// Helper function to extract JWT token from Authorization header
std::string extractToken(const http_request& request) {
    auto headers = request.headers();
    if (headers.has(U("Authorization"))) {
        std::string auth = utility::conversions::to_utf8string(headers[U("Authorization")]);
        if (auth.substr(0, 7) == "Bearer ") {
            return auth.substr(7);
        }
    }
    return "";
}

// Helper function to get client IP address
std::string getClientIP(const http_request& request) {
    auto headers = request.headers();
    if (headers.has(U("X-Forwarded-For"))) {
        return utility::conversions::to_utf8string(headers[U("X-Forwarded-For")]);
    }
    if (headers.has(U("X-Real-IP"))) {
        return utility::conversions::to_utf8string(headers[U("X-Real-IP")]);
    }
    return "unknown";
}

// Helper function to get user agent
std::string getUserAgent(const http_request& request) {
    auto headers = request.headers();
    if (headers.has(U("User-Agent"))) {
        return utility::conversions::to_utf8string(headers[U("User-Agent")]);
    }
    return "";
}

// Auth middleware structures and functions
struct AuthResult {
    int user_id;
    std::string username;
    std::string role;
};

AuthResult authenticateRequest(
    const http_request& request,
    std::shared_ptr<AuthService> auth_service,
    const std::string& required_resource,
    const std::string& required_action
) {
    if (!auth_service) {
        throw std::runtime_error("Authentication service not available");
    }
    
    // Extract token
    std::string token = extractToken(request);
    if (token.empty()) {
        throw std::runtime_error("Missing authorization token");
    }
    
    // Verify token
    auto token_payload = auth_service->verifyToken(token);
    int user_id = token_payload["user_id"];
    std::string username = token_payload["username"];
    std::string role = token_payload["role"];
    
    // Check permission
    if (!auth_service->hasPermission(user_id, required_resource, required_action)) {
        throw std::runtime_error("Insufficient permissions");
    }
    
    return AuthResult{user_id, username, role};
}

// Middleware: Check rate limit for user
void checkRateLimit(
    std::shared_ptr<AuthService> auth_service,
    int user_id,
    const std::string& endpoint,
    int max_requests = 100,
    int window_seconds = 60
) {
    if (!auth_service) {
        return; // Skip rate limiting if auth not available
    }
    
    if (!auth_service->checkRateLimit(user_id, endpoint, max_requests, window_seconds)) {
        throw std::runtime_error("Rate limit exceeded. Please try again later.");
    }
}

int main() {
    Network net;
    Engine engine(net);
    
    // Get environment variables for auth configuration
    const char* jwt_secret_env = std::getenv("JWT_SECRET");
    const char* db_host_env = std::getenv("DB_HOST");
    const char* db_user_env = std::getenv("DB_USER");
    const char* db_pass_env = std::getenv("DB_PASS");
    const char* db_name_env = std::getenv("DB_NAME");
    const char* redis_host_env = std::getenv("REDIS_HOST");
    const char* redis_port_env = std::getenv("REDIS_PORT");
    const char* redis_pass_env = std::getenv("REDIS_PASS");
    
    std::string jwt_secret = jwt_secret_env ? jwt_secret_env : "your-secret-key-change-in-production";
    std::string db_host = db_host_env ? db_host_env : "localhost";
    std::string db_user = db_user_env ? db_user_env : "root";
    std::string db_pass = db_pass_env ? db_pass_env : "NetSimCPP1234";
    std::string db_name = db_name_env ? db_name_env : "netsim";
    std::string redis_host = redis_host_env ? redis_host_env : "localhost";
    int redis_port = redis_port_env ? std::stoi(redis_port_env) : 6379;
    std::string redis_pass = redis_pass_env ? redis_pass_env : "NetSimCPP1234";
    
    // Initialize authentication service
    std::shared_ptr<AuthService> auth_service;
    try {
        auth_service = std::make_shared<AuthService>(
            db_host, db_user, db_pass, db_name,
            redis_host, redis_port, redis_pass,
            jwt_secret
        );
        std::cout << "[Main] Authentication service initialized" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Main] Warning: Failed to initialize auth service: " << e.what() << std::endl;
        std::cerr << "[Main] Server will run without authentication" << std::endl;
    }
    
    // Initialize WebSocket server
    auto ws_server = std::make_shared<netsim::ws::WebSocketServer>();
    netsim::ws::EventBroadcaster::getInstance().setWebSocketServer(ws_server);
    ws_server->start(9001);  // WebSocket on port 9001
    std::cout << "[Main] WebSocket server started on port 9001" << std::endl;

    http_listener listener(U("http://0.0.0.0:8080"));

    // GET /status - Check server status
    listener.support(methods::GET, [&](http_request request) {
        auto path = request.relative_uri().path();
        
        if (path == U("/status")) {
            web::json::value resp;
            resp[U("status")] = web::json::value::string(U("ok"));
            resp[U("message")] = web::json::value::string(U("NetSimCPP REST API v1.0"));
            request.reply(status_codes::OK, resp);
            
        } else if (path == U("/nodes")) {
            // GET /nodes - List all nodes
            try {
                auto nodes = net.getAllNodes();
                web::json::value resp;
                resp[U("nodes")] = string_vector_to_json(nodes);
                resp[U("count")] = web::json::value::number((int)nodes.size());
                request.reply(status_codes::OK, resp);
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::InternalError, resp);
            }
            
        } else if (path == U("/topology")) {
            // GET /topology - Get full network topology
            try {
                std::string jsonStr = net.exportToJson();
                auto resp = web::json::value::parse(utility::conversions::to_string_t(jsonStr));
                request.reply(status_codes::OK, resp);
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::InternalError, resp);
            }
            
        } else if (path == U("/statistics")) {
            // GET /statistics - Get network statistics
            try {
                auto stats = net.getTrafficStats();
                web::json::value resp;
                resp[U("totalPackets")] = web::json::value::number(stats.totalPackets);
                resp[U("totalPacketsSent")] = web::json::value::number(net.getTotalPacketsSent());
                resp[U("totalPacketsReceived")] = web::json::value::number(net.getTotalPacketsReceived());
                resp[U("mostActiveNode")] = web::json::value::string(utility::conversions::to_string_t(net.getMostActiveNode()));
                request.reply(status_codes::OK, resp);
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::InternalError, resp);
            }
            
        } else if (path == U("/cloudnodes")) {
            // GET /cloudnodes - List cloud nodes
            try {
                auto cloudNodes = net.getCloudNodes();
                web::json::value resp;
                resp[U("cloudNodes")] = string_vector_to_json(cloudNodes);
                resp[U("count")] = web::json::value::number((int)cloudNodes.size());
                request.reply(status_codes::OK, resp);
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::InternalError, resp);
            }
            
        } else if (path == U("/db/save")) {
            // GET /db/save - Save topology to database
            try {
                if (!net.isPersistenceEnabled()) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(U("Database persistence not enabled"));
                    resp[U("hint")] = web::json::value::string(U("Use POST /db/enable first"));
                    request.reply(status_codes::BadRequest, resp);
                } else if (net.saveTopologyToDB()) {
                    web::json::value resp;
                    resp[U("status")] = web::json::value::string(U("success"));
                    resp[U("message")] = web::json::value::string(U("Topology saved to database"));
                    resp[U("nodes")] = web::json::value::number((int)net.getAllNodes().size());
                    request.reply(status_codes::OK, resp);
                } else {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(U("Failed to save topology"));
                    request.reply(status_codes::InternalError, resp);
                }
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::InternalError, resp);
            }
            
        } else if (path == U("/db/load")) {
            // GET /db/load - Load topology from database
            try {
                if (!net.isPersistenceEnabled()) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(U("Database persistence not enabled"));
                    resp[U("hint")] = web::json::value::string(U("Use POST /db/enable first"));
                    request.reply(status_codes::BadRequest, resp);
                } else if (net.loadTopologyFromDB()) {
                    web::json::value resp;
                    resp[U("status")] = web::json::value::string(U("success"));
                    resp[U("message")] = web::json::value::string(U("Topology loaded from database"));
                    resp[U("nodes")] = web::json::value::number((int)net.getAllNodes().size());
                    request.reply(status_codes::OK, resp);
                } else {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(U("Failed to load topology"));
                    request.reply(status_codes::InternalError, resp);
                }
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::InternalError, resp);
            }
            
        } else if (path == U("/db/status")) {
            // GET /db/status - Check database status
            try {
                web::json::value resp;
                resp[U("enabled")] = web::json::value::boolean(net.isPersistenceEnabled());
                if (net.isPersistenceEnabled()) {
                    resp[U("status")] = web::json::value::string(U("connected"));
                } else {
                    resp[U("status")] = web::json::value::string(U("disabled"));
                }
                request.reply(status_codes::OK, resp);
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::InternalError, resp);
            }
            
        } else if (path == U("/auth/me")) {
            // GET /auth/me - Get current user info
            if (!auth_service) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(U("Authentication not available"));
                request.reply(status_codes::ServiceUnavailable, resp);
                return;
            }
            
            try {
                std::string token = extractToken(request);
                if (token.empty()) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(U("Missing authorization token"));
                    request.reply(status_codes::Unauthorized, resp);
                    return;
                }
                
                auto payload = auth_service->verifyToken(token);
                int user_id = payload["user_id"];
                auto user_info = auth_service->getUserInfo(user_id);
                
                web::json::value resp;
                resp[U("id")] = web::json::value::number(static_cast<int32_t>(user_info["id"].get<int>()));
                resp[U("username")] = web::json::value::string(utility::conversions::to_string_t(user_info["username"].get<std::string>()));
                resp[U("email")] = web::json::value::string(utility::conversions::to_string_t(user_info["email"].get<std::string>()));
                resp[U("role")] = web::json::value::string(utility::conversions::to_string_t(user_info["role"].get<std::string>()));
                
                // Add permissions
                auto perms = web::json::value::array();
                int idx = 0;
                for (const auto& perm : user_info["permissions"]) {
                    perms[idx++] = web::json::value::string(utility::conversions::to_string_t(perm.get<std::string>()));
                }
                resp[U("permissions")] = perms;
                
                request.reply(status_codes::OK, resp);
                
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::Unauthorized, resp);
            }
            
        } else {
            web::json::value resp;
            resp[U("error")] = web::json::value::string(U("unknown GET endpoint"));
            request.reply(status_codes::NotFound, resp);
        }
    });

    // POST handler - Main API endpoints
    listener.support(methods::POST, [&](http_request request) {
        auto path = request.relative_uri().path();
        auto query = uri::split_query(request.request_uri().query());

        // POST /auth/register - Register new user
        if (path == U("/auth/register")) {
            if (!auth_service) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(U("Authentication not available"));
                request.reply(status_codes::ServiceUnavailable, resp);
                return;
            }
            
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto username = utility::conversions::to_utf8string(jv.at(U("username")).as_string());
                    auto email = utility::conversions::to_utf8string(jv.at(U("email")).as_string());
                    auto password = utility::conversions::to_utf8string(jv.at(U("password")).as_string());
                    auto role = jv.has_field(U("role")) 
                        ? utility::conversions::to_utf8string(jv.at(U("role")).as_string())
                        : std::string("user");
                    
                    auto result = auth_service->registerUser(username, email, password, role);
                    
                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(true);
                    resp[U("user_id")] = web::json::value::number(static_cast<int32_t>(result["user_id"].get<int>()));
                    resp[U("username")] = web::json::value::string(utility::conversions::to_string_t(result["username"].get<std::string>()));
                    resp[U("role")] = web::json::value::string(utility::conversions::to_string_t(result["role"].get<std::string>()));
                    
                    request.reply(status_codes::Created, resp);
                    
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();
            
        } else if (path == U("/auth/login")) {
            // POST /auth/login - User login
            if (!auth_service) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(U("Authentication not available"));
                request.reply(status_codes::ServiceUnavailable, resp);
                return;
            }
            
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto username = utility::conversions::to_utf8string(jv.at(U("username")).as_string());
                    auto password = utility::conversions::to_utf8string(jv.at(U("password")).as_string());
                    
                    std::string ip_address = getClientIP(request);
                    std::string user_agent = getUserAgent(request);
                    
                    auto result = auth_service->loginUser(username, password, ip_address, user_agent);
                    
                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(true);
                    resp[U("token")] = web::json::value::string(utility::conversions::to_string_t(result["token"].get<std::string>()));
                    
                    web::json::value user_obj;
                    user_obj[U("id")] = web::json::value::number(static_cast<int32_t>(result["user"]["id"].get<int>()));
                    user_obj[U("username")] = web::json::value::string(utility::conversions::to_string_t(result["user"]["username"].get<std::string>()));
                    user_obj[U("email")] = web::json::value::string(utility::conversions::to_string_t(result["user"]["email"].get<std::string>()));
                    user_obj[U("role")] = web::json::value::string(utility::conversions::to_string_t(result["user"]["role"].get<std::string>()));
                    resp[U("user")] = user_obj;
                    
                    resp[U("expires_in")] = web::json::value::number(static_cast<int32_t>(result["expires_in"].get<int>()));
                    
                    request.reply(status_codes::OK, resp);
                    
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::Unauthorized, resp);
                }
            }).wait();
            
        } else if (path == U("/auth/logout")) {
            // POST /auth/logout - User logout
            if (!auth_service) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(U("Authentication not available"));
                request.reply(status_codes::ServiceUnavailable, resp);
                return;
            }
            
            try {
                std::string token = extractToken(request);
                if (token.empty()) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(U("Missing authorization token"));
                    request.reply(status_codes::Unauthorized, resp);
                    return;
                }
                
                auto result = auth_service->logoutUser(token);
                
                web::json::value resp;
                resp[U("success")] = web::json::value::boolean(true);
                resp[U("message")] = web::json::value::string(U("Logged out successfully"));
                
                request.reply(status_codes::OK, resp);
                
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::Unauthorized, resp);
            }
            
        // POST /node/add - Add a new node
        } else if (path == U("/node/add")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize
                    auto auth_result = authenticateRequest(request, auth_service, "nodes", "create");
                    
                    // Check rate limit (100 requests per minute)
                    checkRateLimit(auth_service, auth_result.user_id, "/node/add", 100, 60);
                    
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    auto ip = jv.has_field(U("ip")) 
                        ? utility::conversions::to_utf8string(jv[U("ip")].as_string()) 
                        : std::string("127.0.0.1");
                    auto type = jv.has_field(U("type"))
                        ? utility::conversions::to_utf8string(jv[U("type")].as_string())
                        : std::string("host");

                    if (type == "router") {
                        net.addNode<Router>(name, ip);
                    } else {
                        int port = jv.has_field(U("port")) ? jv[U("port")].as_integer() : 8080;
                        net.addNode<Host>(name, ip, port);
                    }
                    
                    // Broadcast WebSocket event
                    netsim::ws::EventBroadcaster::getInstance().nodeAdded(name, type, ip);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("node added"));
                    resp[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
                    resp[U("type")] = web::json::value::string(utility::conversions::to_string_t(type));
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    // Determine appropriate status code
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /node/remove - Remove a node
        } else if (path == U("/node/remove")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize
                    auto auth_result = authenticateRequest(request, auth_service, "nodes", "delete");
                    
                    // Check rate limit
                    checkRateLimit(auth_service, auth_result.user_id, "/node/remove", 50, 60);
                    
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    net.removeNode(name);
                    
                    // Broadcast WebSocket event
                    netsim::ws::EventBroadcaster::getInstance().nodeRemoved(name);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("node removed"));
                    resp[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /node/fail - Simulate node failure
        } else if (path == U("/node/fail")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    net.failNode(name);
                    
                    // Broadcast WebSocket event
                    netsim::ws::EventBroadcaster::getInstance().nodeFailed(name);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("node failed"));
                    resp[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();
        } else if (path == U("/node/config")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize
                    auto auth_result = authenticateRequest(request, auth_service, "nodes", "update");
                    
                    // Check rate limit
                    checkRateLimit(auth_service, auth_result.user_id, "/node/config", 200, 60);
                    
                    auto name = utility::conversions::to_utf8string(jv.at(U("name")).as_string());
                    auto node = net.findByName(name);

                    if (!node) {
                        web::json::value resp;
                        resp[U("error")] = web::json::value::string(U("Node not found"));
                        request.reply(status_codes::NotFound, resp);
                        return;
                    }

                    // Update IP if provided
                    if (jv.has_field(U("ip"))) {
                        auto ip = utility::conversions::to_utf8string(jv.at(U("ip")).as_string());
                        node->setIp(ip);
                    }

                    // Update MTU if provided
                    if (jv.has_field(U("mtu"))) {
                        int mtu = jv.at(U("mtu")).as_integer();
                        node->setMTU(mtu);
                    }

                    // Update queue size if provided
                    if (jv.has_field(U("queueSize"))) {
                        int queueSize = jv.at(U("queueSize")).as_integer();
                        node->setMaxQueueSize(queueSize);
                    }

                    // Broadcast update via WebSocket
                    netsim::ws::EventBroadcaster::getInstance().nodeUpdated(
                        name,
                        node->getType(),
                        node->getIp(),
                        node->getMTU(),
                        node->getMaxQueueSize()
                    );

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("node configuration updated"));
                    resp[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();
        // POST /link/connect - Connect two nodes
        } else if (path == U("/link/connect")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize
                    auto auth_result = authenticateRequest(request, auth_service, "links", "create");
                    
                    // Check rate limit
                    checkRateLimit(auth_service, auth_result.user_id, "/link/connect", 100, 60);
                    
                    auto nodeA = utility::conversions::to_utf8string(jv[U("nodeA")].as_string());
                    auto nodeB = utility::conversions::to_utf8string(jv[U("nodeB")].as_string());
                    int delay = jv.has_field(U("delay")) ? jv[U("delay")].as_integer() : 10;
                    int bandwidth = jv.has_field(U("bandwidth")) ? jv[U("bandwidth")].as_integer() : 1000;
                    net.connect(nodeA, nodeB);
                    
                    // Broadcast WebSocket event
                    netsim::ws::EventBroadcaster::getInstance().linkAdded(nodeA, nodeB, delay, bandwidth);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("nodes connected"));
                    resp[U("nodeA")] = web::json::value::string(utility::conversions::to_string_t(nodeA));
                    resp[U("nodeB")] = web::json::value::string(utility::conversions::to_string_t(nodeB));
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /link/disconnect - Disconnect two nodes
        } else if (path == U("/link/disconnect")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize
                    auto auth_result = authenticateRequest(request, auth_service, "links", "delete");
                    
                    // Check rate limit
                    checkRateLimit(auth_service, auth_result.user_id, "/link/disconnect", 100, 60);
                    
                    auto nodeA = utility::conversions::to_utf8string(jv[U("nodeA")].as_string());
                    auto nodeB = utility::conversions::to_utf8string(jv[U("nodeB")].as_string());
                    net.disconnect(nodeA, nodeB);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("nodes disconnected"));
                    resp[U("nodeA")] = web::json::value::string(utility::conversions::to_string_t(nodeA));
                    resp[U("nodeB")] = web::json::value::string(utility::conversions::to_string_t(nodeB));
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /link/delay - Set link delay
        } else if (path == U("/link/delay")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize
                    auto auth_result = authenticateRequest(request, auth_service, "links", "update");
                    
                    // Check rate limit (higher limit for config changes)
                    checkRateLimit(auth_service, auth_result.user_id, "/link/delay", 200, 60);
                    
                    auto nodeA = utility::conversions::to_utf8string(jv[U("nodeA")].as_string());
                    auto nodeB = utility::conversions::to_utf8string(jv[U("nodeB")].as_string());
                    int delay = jv[U("delay")].as_integer();
                    net.setLinkDelay(nodeA, nodeB, delay);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("link delay set"));
                    resp[U("delay")] = web::json::value::number(delay);
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /link/bandwidth - Set link bandwidth
        } else if (path == U("/link/bandwidth")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize
                    auto auth_result = authenticateRequest(request, auth_service, "links", "update");
                    
                    // Check rate limit
                    checkRateLimit(auth_service, auth_result.user_id, "/link/bandwidth", 200, 60);
                    
                    auto nodeA = utility::conversions::to_utf8string(jv[U("nodeA")].as_string());
                    auto nodeB = utility::conversions::to_utf8string(jv[U("nodeB")].as_string());
                    int bandwidth = jv[U("bandwidth")].as_integer();
                    net.setBandwidth(nodeA, nodeB, bandwidth);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("bandwidth set"));
                    resp[U("bandwidth")] = web::json::value::number(bandwidth);
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /link/packetloss - Set packet loss probability
        } else if (path == U("/link/packetloss")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize
                    auto auth_result = authenticateRequest(request, auth_service, "links", "update");
                    
                    // Check rate limit
                    checkRateLimit(auth_service, auth_result.user_id, "/link/packetloss", 200, 60);
                    
                    auto nodeA = utility::conversions::to_utf8string(jv[U("nodeA")].as_string());
                    auto nodeB = utility::conversions::to_utf8string(jv[U("nodeB")].as_string());
                    double prob = jv[U("probability")].as_double();
                    net.setPacketLoss(nodeA, nodeB, prob);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("packet loss probability set"));
                    resp[U("probability")] = web::json::value::number(prob);
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /vlan/assign - Assign VLAN to node
        } else if (path == U("/vlan/assign")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize (admin/user only)
                    auto auth_result = authenticateRequest(request, auth_service, "network", "update");
                    
                    // Check rate limit
                    checkRateLimit(auth_service, auth_result.user_id, "/vlan/assign", 100, 60);
                    
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    int vlanId = jv[U("vlanId")].as_integer();
                    net.assignVLAN(name, vlanId);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("VLAN assigned"));
                    resp[U("node")] = web::json::value::string(utility::conversions::to_string_t(name));
                    resp[U("vlanId")] = web::json::value::number(vlanId);
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /firewall/rule - Add firewall rule
        } else if (path == U("/firewall/rule")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize (security-critical, admin only)
                    auto auth_result = authenticateRequest(request, auth_service, "firewall", "create");
                    
                    // Check rate limit (stricter for security operations)
                    checkRateLimit(auth_service, auth_result.user_id, "/firewall/rule", 50, 60);
                    
                    auto src = utility::conversions::to_utf8string(jv[U("src")].as_string());
                    auto dst = utility::conversions::to_utf8string(jv[U("dst")].as_string());
                    auto protocol = utility::conversions::to_utf8string(jv[U("protocol")].as_string());
                    bool allow = jv[U("allow")].as_bool();
                    net.addFirewallRule(src, dst, protocol, allow);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("firewall rule added"));
                    resp[U("action")] = web::json::value::string(allow ? U("allow") : U("deny"));
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /ping - Simulate ping
        } else if (path == U("/ping")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string src = utility::conversions::to_utf8string(jv[U("src")].as_string());
                    std::string dst = utility::conversions::to_utf8string(jv[U("dst")].as_string());
                    std::vector<std::string> pathOut;
                    bool ok = engine.ping(src, dst, pathOut);

                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(ok);
                    resp[U("path")] = string_vector_to_json(pathOut);
                    resp[U("hops")] = web::json::value::number((int)pathOut.size());
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /traceroute - Simulate traceroute
        } else if (path == U("/traceroute")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string src = utility::conversions::to_utf8string(jv[U("src")].as_string());
                    std::string dst = utility::conversions::to_utf8string(jv[U("dst")].as_string());
                    std::vector<std::string> pathOut;
                    bool ok = engine.traceroute(src, dst, pathOut);

                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(ok);
                    resp[U("path")] = string_vector_to_json(pathOut);
                    resp[U("hops")] = web::json::value::number((int)pathOut.size());
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /multicast - Simulate multicast
        } else if (path == U("/multicast")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string src = utility::conversions::to_utf8string(jv[U("src")].as_string());
                    auto destArray = jv[U("destinations")].as_array();
                    std::vector<std::string> destinations;
                    for (const auto& dest : destArray) {
                        destinations.push_back(utility::conversions::to_utf8string(dest.as_string()));
                    }
                    
                    bool ok = engine.multicast(src, destinations);

                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(ok);
                    resp[U("destinations")] = web::json::value::number((int)destinations.size());
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /tcp/connect - Initiate TCP connection
        } else if (path == U("/tcp/connect")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string client = utility::conversions::to_utf8string(jv[U("client")].as_string());
                    std::string server = utility::conversions::to_utf8string(jv[U("server")].as_string());
                    bool ok = net.initiateTCPConnection(client, server);

                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(ok);
                    resp[U("message")] = web::json::value::string(ok ? U("TCP connection established") : U("TCP connection failed"));
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /topology/import - Import topology from JSON
        } else if (path == U("/topology/import")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    // Authenticate and authorize (critical operation, admin only)
                    auto auth_result = authenticateRequest(request, auth_service, "topology", "create");
                    
                    // Check rate limit (very strict for topology import)
                    checkRateLimit(auth_service, auth_result.user_id, "/topology/import", 10, 60);
                    
                    std::string jsonStr = utility::conversions::to_utf8string(jv.serialize());
                    net.importFromJson(jsonStr);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("topology imported"));
                    request.reply(status_codes::OK, resp);

                } catch (const std::runtime_error& e) {
                    std::string error_msg = e.what();
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(error_msg));
                    
                    if (error_msg.find("Missing authorization") != std::string::npos ||
                        error_msg.find("Invalid token") != std::string::npos) {
                        request.reply(status_codes::Unauthorized, resp);
                    } else if (error_msg.find("Insufficient permissions") != std::string::npos) {
                        request.reply(status_codes::Forbidden, resp);
                    } else if (error_msg.find("Rate limit") != std::string::npos) {
                        request.reply(status_codes::TooManyRequests, resp);
                    } else if (error_msg.find("Authentication service") != std::string::npos) {
                        request.reply(status_codes::ServiceUnavailable, resp);
                    } else {
                        request.reply(status_codes::BadRequest, resp);
                    }
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /wireless/range - Set wireless range
        } else if (path == U("/wireless/range")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    int range = jv[U("range")].as_integer();
                    net.setWirelessRange(name, range);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("wireless range set"));
                    resp[U("node")] = web::json::value::string(utility::conversions::to_string_t(name));
                    resp[U("range")] = web::json::value::number(range);
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /wireless/interference - Simulate wireless interference
        } else if (path == U("/wireless/interference")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    double lossProb = jv[U("lossProb")].as_double();
                    net.simulateInterference(name, lossProb);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("interference simulated"));
                    resp[U("node")] = web::json::value::string(utility::conversions::to_string_t(name));
                    resp[U("lossProb")] = web::json::value::number(lossProb);
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /cloud/add - Add cloud node
        } else if (path == U("/cloud/add")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    auto ip = utility::conversions::to_utf8string(jv[U("ip")].as_string());
                    net.addCloudNode(name, ip);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("cloud node added"));
                    resp[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /cloud/scaleup - Scale up cloud node
        } else if (path == U("/cloud/scaleup")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    net.scaleUp(name);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("cloud node scaled up"));
                    resp[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /cloud/scaledown - Scale down cloud node
        } else if (path == U("/cloud/scaledown")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    net.scaleDown(name);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("cloud node scaled down"));
                    resp[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /iot/add - Add IoT device
        } else if (path == U("/iot/add")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    auto ip = utility::conversions::to_utf8string(jv[U("ip")].as_string());
                    net.addIoTDevice(name, ip);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("IoT device added"));
                    resp[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /iot/battery - Simulate battery drain
        } else if (path == U("/iot/battery")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    int percent = jv[U("percent")].as_integer();
                    net.simulateBatteryDrain(name, percent);

                    int batteryLevel = net.getBatteryLevel(name);
                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("battery drained"));
                    resp[U("name")] = web::json::value::string(utility::conversions::to_string_t(name));
                    resp[U("batteryLevel")] = web::json::value::number(batteryLevel);
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /statistics/reset - Reset network statistics
        } else if (path == U("/statistics/reset")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    if (jv.has_field(U("node"))) {
                        auto name = utility::conversions::to_utf8string(jv[U("node")].as_string());
                        net.resetNodeStatistics(name);
                        web::json::value resp;
                        resp[U("result")] = web::json::value::string(U("node statistics reset"));
                        resp[U("node")] = web::json::value::string(utility::conversions::to_string_t(name));
                        request.reply(status_codes::OK, resp);
                    } else {
                        net.resetAllStatistics();
                        web::json::value resp;
                        resp[U("result")] = web::json::value::string(U("all statistics reset"));
                        request.reply(status_codes::OK, resp);
                    }

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /metrics/performance - Get performance metrics
        } else if (path == U("/metrics/performance")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto nodeA = utility::conversions::to_utf8string(jv[U("nodeA")].as_string());
                    auto nodeB = utility::conversions::to_utf8string(jv[U("nodeB")].as_string());
                    
                    int latency = net.getLatency(nodeA, nodeB);
                    int throughput = net.getThroughput(nodeA, nodeB);
                    double packetLoss = net.getPacketLossRate(nodeA, nodeB);

                    web::json::value resp;
                    resp[U("latency")] = web::json::value::number(latency);
                    resp[U("throughput")] = web::json::value::number(throughput);
                    resp[U("packetLossRate")] = web::json::value::number(packetLoss);
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /db/enable - Enable database persistence
        } else if (path == U("/db/enable")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto host = jv.has_field(U("host")) 
                        ? utility::conversions::to_utf8string(jv[U("host")].as_string()) 
                        : std::string("127.0.0.1");
                    int port = jv.has_field(U("port")) 
                        ? jv[U("port")].as_integer() 
                        : 3306;
                    auto user = jv.has_field(U("user")) 
                        ? utility::conversions::to_utf8string(jv[U("user")].as_string()) 
                        : std::string("root");
                    auto password = jv.has_field(U("password")) 
                        ? utility::conversions::to_utf8string(jv[U("password")].as_string()) 
                        : std::string("NetSimCPP1234");
                    auto database = jv.has_field(U("database")) 
                        ? utility::conversions::to_utf8string(jv[U("database")].as_string()) 
                        : std::string("netsim");

                    net.enablePersistence(host, port, user, password, database);

                    web::json::value resp;
                    resp[U("status")] = web::json::value::string(U("success"));
                    resp[U("message")] = web::json::value::string(U("Database persistence enabled"));
                    resp[U("host")] = web::json::value::string(utility::conversions::to_string_t(host));
                    resp[U("database")] = web::json::value::string(utility::conversions::to_string_t(database));
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        // POST /db/disable - Disable database persistence
        } else if (path == U("/db/disable")) {
            try {
                net.disablePersistence();
                web::json::value resp;
                resp[U("status")] = web::json::value::string(U("success"));
                resp[U("message")] = web::json::value::string(U("Database persistence disabled"));
                request.reply(status_codes::OK, resp);
            } catch (const std::exception& e) {
                web::json::value resp;
                resp[U("error")] = web::json::value::string(utility::conversions::to_string_t(e.what()));
                request.reply(status_codes::InternalError, resp);
            }

        } else if (path == U("/assignVLAN")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    int vlanId = jv[U("vlanId")].as_integer();
                    net.assignVLAN(name, vlanId);

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("VLAN assigned"));
                    request.reply(status_codes::OK, resp);
                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(
                        utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        } else {
            web::json::value resp;
            resp[U("error")] = web::json::value::string(U("unknown POST endpoint"));
            request.reply(status_codes::NotFound, resp);
        }
    });

    try {
        listener.open().wait();
        std::cout << "==================================================" << std::endl;
        std::cout << "NetSimCPP REST API Server v1.0" << std::endl;
        std::cout << "Server running at http://0.0.0.0:8080" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << "\nAvailable endpoints:" << std::endl;
        std::cout << "GET  /status              - Check server status" << std::endl;
        std::cout << "GET  /nodes               - List all nodes" << std::endl;
        std::cout << "GET  /topology            - Export topology" << std::endl;
        std::cout << "GET  /statistics          - Network statistics" << std::endl;
        std::cout << "GET  /cloudnodes          - List cloud nodes" << std::endl;
        std::cout << "POST /node/add            - Add node" << std::endl;
        std::cout << "POST /node/remove         - Remove node" << std::endl;
        std::cout << "POST /node/fail           - Fail node" << std::endl;
        std::cout << "POST /link/connect        - Connect nodes" << std::endl;
        std::cout << "POST /link/disconnect     - Disconnect nodes" << std::endl;
        std::cout << "POST /link/delay          - Set link delay" << std::endl;
        std::cout << "POST /link/bandwidth      - Set bandwidth" << std::endl;
        std::cout << "POST /link/packetloss     - Set packet loss" << std::endl;
        std::cout << "POST /vlan/assign         - Assign VLAN" << std::endl;
        std::cout << "POST /firewall/rule       - Add firewall rule" << std::endl;
        std::cout << "POST /ping                - Ping nodes" << std::endl;
        std::cout << "POST /traceroute          - Traceroute" << std::endl;
        std::cout << "POST /multicast           - Multicast" << std::endl;
        std::cout << "POST /tcp/connect         - TCP connection" << std::endl;
        std::cout << "POST /topology/import     - Import topology" << std::endl;
        std::cout << "POST /wireless/range      - Set wireless range" << std::endl;
        std::cout << "POST /wireless/interference - Simulate interference" << std::endl;
        std::cout << "POST /cloud/add           - Add cloud node" << std::endl;
        std::cout << "POST /cloud/scaleup       - Scale up cloud" << std::endl;
        std::cout << "POST /cloud/scaledown     - Scale down cloud" << std::endl;
        std::cout << "POST /iot/add             - Add IoT device" << std::endl;
        std::cout << "POST /iot/battery         - Battery drain" << std::endl;
        std::cout << "POST /statistics/reset    - Reset statistics" << std::endl;
        std::cout << "POST /metrics/performance - Performance metrics" << std::endl;
        std::cout << "\nServer running. Press Ctrl+C to stop..." << std::endl;
        std::cout << "==================================================" << std::endl;
        
        // Keep server running (daemon mode for Docker)
        std::promise<void> exitSignal;
        std::future<void> futureObj = exitSignal.get_future();
        futureObj.wait();
        
        listener.close().wait();
    } catch (const std::exception& e) {
        std::cerr << "Listener error: " << e.what() << std::endl;
    }

    return 0;
}
