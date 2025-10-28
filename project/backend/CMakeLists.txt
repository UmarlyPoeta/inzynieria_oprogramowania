cmake_minimum_required(VERSION 3.16)
project(NetSimCPP VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpprestsdk REQUIRED) # Microsoft C++ REST SDK
find_package(nlohmann_json REQUIRED)
<<<<<<< HEAD
find_package(MySQLConnectorCPP REQUIRED)  # <-- dodane: MySQL Connector/C++
=======
find_package(OpenSSL REQUIRED)
>>>>>>> fe3dcd3711bf4fde6258fb81b968e5d4a5626880

add_executable(netsim
    src/main.cpp
    src/core/Node.cpp
    src/core/Packet.cpp
    src/core/Network.cpp
    src/core/Engine.cpp
    src/core/Host.cpp
    src/core/Router.cpp
)

target_include_directories(netsim PRIVATE src)

<<<<<<< HEAD
target_link_libraries(netsim PRIVATE
    cpprestsdk::cpprest
    nlohmann_json::nlohmann_json
    mysqlcppconn               # <-- dodane: link do klienta MySQL
)
=======
target_link_libraries(netsim PRIVATE cpprestsdk::cpprest nlohmann_json::nlohmann_json OpenSSL::SSL OpenSSL::Crypto)
>>>>>>> fe3dcd3711bf4fde6258fb81b968e5d4a5626880

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
target_include_directories(netsim_tests PRIVATE src)
<<<<<<< HEAD
target_link_libraries(netsim_tests PRIVATE
    GTest::gtest_main
    cpprestsdk::cpprest
    nlohmann_json::nlohmann_json
    mysqlcppconn               # <-- dodane dla testów
)
add_test(NAME netsim_tests COMMAND netsim_tests)
=======
target_link_libraries(netsim_tests PRIVATE GTest::gtest_main cpprestsdk::cpprest nlohmann_json::nlohmann_json)
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
target_include_directories(netsim_perf_tests PRIVATE src)
target_link_libraries(netsim_perf_tests PRIVATE GTest::gtest_main cpprestsdk::cpprest nlohmann_json::nlohmann_json)
add_test(NAME netsim_perf_tests COMMAND netsim_perf_tests)
>>>>>>> fe3dcd3711bf4fde6258fb81b968e5d4a5626880
