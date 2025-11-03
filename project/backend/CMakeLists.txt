cmake_minimum_required(VERSION 3.16)
project(NetSimCPP VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Silence Boost FindBoost deprecation warning
cmake_policy(SET CMP0167 OLD)

find_package(cpprestsdk REQUIRED) # Microsoft C++ REST SDK
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread) # For WebSocket

# MySQL Connector/C++ - REQUIRED for database persistence
find_library(MYSQLCPPCONN_LIB mysqlcppconn REQUIRED)
find_path(MYSQLCPPCONN_INCLUDE mysql_driver.h PATH_SUFFIXES mysql-cppconn-8 mysql-cppconn REQUIRED)

if(NOT MYSQLCPPCONN_LIB)
    message(FATAL_ERROR "MySQL Connector/C++ is required but not found")
endif()
message(STATUS "MySQL Connector/C++ found: ${MYSQLCPPCONN_LIB}")

# WebSocket++ (header-only library)
find_path(WEBSOCKETPP_INCLUDE websocketpp/server.hpp PATHS /usr/include /usr/local/include)

# Redis client (hiredis) - REQUIRED
find_library(HIREDIS_LIB hiredis REQUIRED)
find_path(HIREDIS_INCLUDE hiredis/hiredis.h PATHS /usr/include /usr/local/include REQUIRED)

if(NOT HIREDIS_LIB)
    message(FATAL_ERROR "Redis (hiredis) is required but not found")
endif()
message(STATUS "Redis (hiredis) found: ${HIREDIS_LIB}")

# Argon2 for password hashing - REQUIRED
find_library(ARGON2_LIB argon2 REQUIRED)
find_path(ARGON2_INCLUDE argon2.h PATHS /usr/include /usr/local/include REQUIRED)

if(NOT ARGON2_LIB)
    message(FATAL_ERROR "Argon2 is required but not found")
endif()
message(STATUS "Argon2 found: ${ARGON2_LIB}")

# JWT-CPP (header-only library) - REQUIRED
find_path(JWT_CPP_INCLUDE jwt-cpp/jwt.h PATHS /usr/include /usr/local/include REQUIRED)

if(NOT JWT_CPP_INCLUDE)
    message(FATAL_ERROR "JWT-CPP is required but not found")
endif()
message(STATUS "JWT-CPP found: ${JWT_CPP_INCLUDE}")

# PicoJSON (required by jwt-cpp)
find_path(PICOJSON_INCLUDE picojson.h PATHS /usr/include /usr/local/include)

# YAML-CPP for scenario files - REQUIRED
find_package(yaml-cpp CONFIG REQUIRED)

if(NOT TARGET yaml-cpp)
    message(FATAL_ERROR "yaml-cpp was not found even though libyaml-cpp-dev is installed.")
endif()

message(STATUS "Found yaml-cpp via CMake config")

set(YAML_CPP_LIBRARIES yaml-cpp)

# All dependencies found - enable full features
add_compile_definitions(HAVE_MYSQL_SUPPORT)
add_compile_definitions(HAVE_REDIS_SUPPORT)
add_compile_definitions(HAVE_ARGON2_SUPPORT)
add_compile_definitions(HAVE_JWT_SUPPORT)
add_compile_definitions(HAVE_YAML_SUPPORT)

if(WEBSOCKETPP_INCLUDE)
    add_compile_definitions(HAVE_WEBSOCKET_SUPPORT)
endif()

add_executable(netsim
    src/main.cpp
    src/core/Node.cpp
    src/core/Packet.cpp
    src/core/Network.cpp
    src/core/Engine.cpp
    src/core/Host.cpp
    src/core/Router.cpp
    src/scenario/ScenarioTypes.cpp
    src/scenario/ScenarioRunner.cpp
)

target_include_directories(netsim PRIVATE 
    src 
    ${Boost_INCLUDE_DIRS}
)

if(MYSQLCPPCONN_INCLUDE)
    target_include_directories(netsim PRIVATE ${MYSQLCPPCONN_INCLUDE})
endif()
if(WEBSOCKETPP_INCLUDE)
    target_include_directories(netsim PRIVATE ${WEBSOCKETPP_INCLUDE})
endif()
if(HIREDIS_INCLUDE)
    target_include_directories(netsim PRIVATE ${HIREDIS_INCLUDE})
endif()
if(ARGON2_INCLUDE)
    target_include_directories(netsim PRIVATE ${ARGON2_INCLUDE})
endif()
if(JWT_CPP_INCLUDE)
    target_include_directories(netsim PRIVATE ${JWT_CPP_INCLUDE})
endif()
if(PICOJSON_INCLUDE)
    target_include_directories(netsim PRIVATE ${PICOJSON_INCLUDE})
endif()
if(YAML_CPP_INCLUDE_DIRS)
    target_include_directories(netsim PRIVATE ${YAML_CPP_INCLUDE_DIRS})
endif()

target_link_libraries(netsim PRIVATE 
    cpprestsdk::cpprest 
    nlohmann_json::nlohmann_json 
    OpenSSL::SSL 
    OpenSSL::Crypto
    Boost::system
    Boost::thread
    pthread
    ${MYSQLCPPCONN_LIB}
    ${HIREDIS_LIB}
    ${ARGON2_LIB}
    ${YAML_CPP_LIBRARIES}
)

# Optional: install rules
install(TARGETS netsim DESTINATION bin)

# Tests
enable_testing()
find_package(GTest REQUIRED)
add_executable(netsim_tests
    src/test_main.cpp
    src/core/Node.cpp
    src/core/Packet.cpp
    src/core/Network.cpp
    src/core/Engine.cpp
    src/core/Host.cpp
    src/core/Router.cpp
)
target_include_directories(netsim_tests PRIVATE 
    src 
    ${Boost_INCLUDE_DIRS}
)
if(MYSQLCPPCONN_INCLUDE)
    target_include_directories(netsim_tests PRIVATE ${MYSQLCPPCONN_INCLUDE})
endif()
if(WEBSOCKETPP_INCLUDE)
    target_include_directories(netsim_tests PRIVATE ${WEBSOCKETPP_INCLUDE})
endif()
if(YAML_CPP_INCLUDE_DIRS)
    target_include_directories(netsim_tests PRIVATE ${YAML_CPP_INCLUDE_DIRS})
endif()

target_link_libraries(netsim_tests PRIVATE 
    GTest::gtest_main 
    cpprestsdk::cpprest 
    nlohmann_json::nlohmann_json
    OpenSSL::SSL 
    OpenSSL::Crypto
    Boost::system
    Boost::thread
    pthread
    ${YAML_CPP_LIBRARIES}
)
if(MYSQLCPPCONN_LIB)
    target_link_libraries(netsim_tests PRIVATE ${MYSQLCPPCONN_LIB})
endif()
add_test(NAME netsim_tests COMMAND netsim_tests)

# Performance tests
add_executable(netsim_perf_tests
    src/performance_tests.cpp
    src/core/Node.cpp
    src/core/Packet.cpp
    src/core/Network.cpp
    src/core/Engine.cpp
    src/core/Host.cpp
    src/core/Router.cpp
)
target_include_directories(netsim_perf_tests PRIVATE 
    src 
    ${Boost_INCLUDE_DIRS}
)
if(MYSQLCPPCONN_INCLUDE)
    target_include_directories(netsim_perf_tests PRIVATE ${MYSQLCPPCONN_INCLUDE})
endif()
if(WEBSOCKETPP_INCLUDE)
    target_include_directories(netsim_perf_tests PRIVATE ${WEBSOCKETPP_INCLUDE})
endif()

target_link_libraries(netsim_perf_tests PRIVATE 
    GTest::gtest_main 
    cpprestsdk::cpprest 
    nlohmann_json::nlohmann_json
    OpenSSL::SSL 
    OpenSSL::Crypto
    Boost::system
    Boost::thread
    pthread
)
if(MYSQLCPPCONN_LIB)
    target_link_libraries(netsim_perf_tests PRIVATE ${MYSQLCPPCONN_LIB})
endif()
add_test(NAME netsim_perf_tests COMMAND netsim_perf_tests)
