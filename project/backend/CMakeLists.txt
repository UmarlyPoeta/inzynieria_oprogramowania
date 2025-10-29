cmake_minimum_required(VERSION 3.16)
project(NetSimCPP VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpprestsdk REQUIRED) # Microsoft C++ REST SDK
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread) # For WebSocket

# MySQL Connector/C++ - for database persistence
find_library(MYSQLCPPCONN_LIB mysqlcppconn REQUIRED)
find_path(MYSQLCPPCONN_INCLUDE mysql_driver.h PATH_SUFFIXES mysql-cppconn-8 mysql-cppconn)

# WebSocket++ (header-only library)
find_path(WEBSOCKETPP_INCLUDE websocketpp/server.hpp PATHS /usr/include /usr/local/include)

# Redis client (hiredis)
find_library(HIREDIS_LIB hiredis REQUIRED)
find_path(HIREDIS_INCLUDE hiredis/hiredis.h PATHS /usr/include /usr/local/include)

# Argon2 for password hashing
find_library(ARGON2_LIB argon2 REQUIRED)
find_path(ARGON2_INCLUDE argon2.h PATHS /usr/include /usr/local/include)

# JWT-CPP (header-only library)
find_path(JWT_CPP_INCLUDE jwt-cpp/jwt.h PATHS /usr/include /usr/local/include)

# PicoJSON (required by jwt-cpp)
find_path(PICOJSON_INCLUDE picojson.h PATHS /usr/include /usr/local/include)

add_executable(netsim
    src/main.cpp
    src/core/Node.cpp
    src/core/Packet.cpp
    src/core/Network.cpp
    src/core/Engine.cpp
    src/core/Host.cpp
    src/core/Router.cpp
)

target_include_directories(netsim PRIVATE 
    src 
    ${MYSQLCPPCONN_INCLUDE}
    ${WEBSOCKETPP_INCLUDE}
    ${HIREDIS_INCLUDE}
    ${ARGON2_INCLUDE}
    ${JWT_CPP_INCLUDE}
    ${PICOJSON_INCLUDE}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(netsim PRIVATE 
    cpprestsdk::cpprest 
    nlohmann_json::nlohmann_json 
    OpenSSL::SSL 
    OpenSSL::Crypto
    ${MYSQLCPPCONN_LIB}
    ${HIREDIS_LIB}
    ${ARGON2_LIB}
    Boost::system
    Boost::thread
    pthread
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
    ${MYSQLCPPCONN_INCLUDE}
    ${WEBSOCKETPP_INCLUDE}
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(netsim_tests PRIVATE 
    GTest::gtest_main 
    cpprestsdk::cpprest 
    nlohmann_json::nlohmann_json
    OpenSSL::SSL 
    OpenSSL::Crypto
    ${MYSQLCPPCONN_LIB}
    Boost::system
    Boost::thread
    pthread
)
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
    ${MYSQLCPPCONN_INCLUDE}
    ${WEBSOCKETPP_INCLUDE}
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(netsim_perf_tests PRIVATE 
    GTest::gtest_main 
    cpprestsdk::cpprest 
    nlohmann_json::nlohmann_json
    OpenSSL::SSL 
    OpenSSL::Crypto
    ${MYSQLCPPCONN_LIB}
    Boost::system
    Boost::thread
    pthread
)
add_test(NAME netsim_perf_tests COMMAND netsim_perf_tests)
