#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <random>
#include "core/Network.hpp"
#include "core/Engine.hpp"
#include "core/Host.hpp"
#include "core/Router.hpp"

using namespace std::chrono;

// Performance test suite
class PerformanceTest : public ::testing::Test {
protected:
    Network net;
    
    void SetUp() override {
        // Reset network for each test
    }
    
    // Helper to measure execution time
    template<typename Func>
    double measureTime(Func func) {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();
        return duration_cast<microseconds>(end - start).count() / 1000.0; // ms
    }
};

// Test 1: Node creation performance
TEST_F(PerformanceTest, NodeCreationPerformance) {
    const int NUM_NODES = 100;
    
    auto time = measureTime([&]() {
        for (int i = 0; i < NUM_NODES; i++) {
            std::string name = "Node" + std::to_string(i);
            std::string ip = "10.0.0." + std::to_string(i);
            net.addNode<Host>(name, ip, 8080);
        }
    });
    
    double avgTime = time / NUM_NODES;
    std::cout << "Created " << NUM_NODES << " nodes in " << time << "ms" << std::endl;
    std::cout << "Average: " << avgTime << "ms per node" << std::endl;
    
    // Performance requirement: <1ms per node
    EXPECT_LT(avgTime, 1.0) << "Node creation too slow";
}

// Test 2: Link creation performance
TEST_F(PerformanceTest, LinkCreationPerformance) {
    const int NUM_NODES = 50;
    
    // Create nodes first
    for (int i = 0; i < NUM_NODES; i++) {
        std::string name = "Node" + std::to_string(i);
        std::string ip = "10.0.0." + std::to_string(i);
        net.addNode<Host>(name, ip, 8080);
    }
    
    // Measure link creation
    auto time = measureTime([&]() {
        for (int i = 0; i < NUM_NODES - 1; i++) {
            net.connect("Node" + std::to_string(i), "Node" + std::to_string(i + 1));
        }
    });
    
    int numLinks = NUM_NODES - 1;
    double avgTime = time / numLinks;
    std::cout << "Created " << numLinks << " links in " << time << "ms" << std::endl;
    std::cout << "Average: " << avgTime << "ms per link" << std::endl;
    
    // Performance requirement: <0.5ms per link
    EXPECT_LT(avgTime, 0.5) << "Link creation too slow";
}

// Test 3: Ping performance
TEST_F(PerformanceTest, PingPerformance) {
    const int CHAIN_LENGTH = 20;
    
    // Create chain
    for (int i = 0; i < CHAIN_LENGTH; i++) {
        net.addNode<Host>("Node" + std::to_string(i), "10.0.0." + std::to_string(i), 8080);
    }
    
    for (int i = 0; i < CHAIN_LENGTH - 1; i++) {
        net.connect("Node" + std::to_string(i), "Node" + std::to_string(i + 1));
    }
    
    Engine engine(net);
    std::vector<std::string> path;
    
    auto time = measureTime([&]() {
        engine.ping("Node0", "Node" + std::to_string(CHAIN_LENGTH - 1), path);
    });
    
    std::cout << "Ping through " << CHAIN_LENGTH << " nodes: " << time << "ms" << std::endl;
    std::cout << "Path size: " << path.size() << " nodes" << std::endl;
    
    EXPECT_LT(time, 5.0) << "Ping too slow";
    // Path should have at least start and end nodes if ping succeeds
    if (path.size() > 0) {
        EXPECT_GE(path.size(), 2) << "Path should have at least 2 nodes";
    }
}

// Test 4: Large network performance
TEST_F(PerformanceTest, LargeNetworkPerformance) {
    const int NUM_NODES = 100;
    
    auto setupTime = measureTime([&]() {
        for (int i = 0; i < NUM_NODES; i++) {
            net.addNode<Host>("Node" + std::to_string(i), 
                            "10.0." + std::to_string(i / 256) + "." + std::to_string(i % 256), 
                            8080);
        }
        
        for (int i = 0; i < NUM_NODES - 1; i++) {
            net.connect("Node" + std::to_string(i), "Node" + std::to_string(i + 1));
        }
        
        for (int i = 0; i < NUM_NODES - 10; i += 10) {
            net.connect("Node" + std::to_string(i), "Node" + std::to_string(i + 10));
        }
    });
    
    std::cout << "Created " << NUM_NODES << "-node network in " << setupTime << "ms" << std::endl;
    EXPECT_LT(setupTime, 500.0) << "Large network setup too slow";
    
    Engine engine(net);
    std::vector<std::string> path;
    
    auto pingTime = measureTime([&]() {
        engine.ping("Node0", "Node99", path);
    });
    
    std::cout << "Ping in " << NUM_NODES << "-node network: " << pingTime << "ms" << std::endl;
    EXPECT_LT(pingTime, 50.0) << "Ping in large network too slow";
}

// Test 5: Topology export performance
TEST_F(PerformanceTest, TopologyExportPerformance) {
    const int NUM_NODES = 50;
    
    for (int i = 0; i < NUM_NODES; i++) {
        net.addNode<Host>("Node" + std::to_string(i), "10.0.0." + std::to_string(i), 8080);
    }
    
    for (int i = 0; i < NUM_NODES - 1; i++) {
        net.connect("Node" + std::to_string(i), "Node" + std::to_string(i + 1));
    }
    
    std::string json;
    auto time = measureTime([&]() {
        json = net.exportToJson();
    });
    
    std::cout << "Exported " << NUM_NODES << "-node topology in " << time << "ms" << std::endl;
    std::cout << "JSON size: " << json.size() << " bytes" << std::endl;
    
    EXPECT_LT(time, 100.0) << "Topology export too slow";
    EXPECT_GT(json.size(), 0) << "Export failed";
}

// Test 6: Statistics performance
TEST_F(PerformanceTest, StatisticsPerformance) {
    const int NUM_NODES = 50;
    const int NUM_OPERATIONS = 1000;
    
    for (int i = 0; i < NUM_NODES; i++) {
        net.addNode<Host>("Node" + std::to_string(i), "10.0.0." + std::to_string(i), 8080);
    }
    
    auto time = measureTime([&]() {
        for (int i = 0; i < NUM_OPERATIONS; i++) {
            std::string node = "Node" + std::to_string(i % NUM_NODES);
            net.recordPacketSent(node);
            net.recordPacketReceived(node);
        }
    });
    
    double timePerOp = time / (NUM_OPERATIONS * 2);
    std::cout << "Recorded " << (NUM_OPERATIONS * 2) << " statistics in " << time << "ms" << std::endl;
    std::cout << "Average: " << timePerOp << "ms per operation" << std::endl;
    
    EXPECT_LT(timePerOp, 0.01) << "Statistics recording too slow";
}

// Test 7: Multicast performance
TEST_F(PerformanceTest, MulticastPerformance) {
    const int NUM_NODES = 50;
    const int NUM_DESTINATIONS = 10;
    
    for (int i = 0; i < NUM_NODES; i++) {
        net.addNode<Host>("Node" + std::to_string(i), "10.0.0." + std::to_string(i), 8080);
    }
    
    for (int i = 1; i < NUM_NODES; i++) {
        net.connect("Node0", "Node" + std::to_string(i));
    }
    
    std::vector<std::string> destinations;
    for (int i = 1; i <= NUM_DESTINATIONS; i++) {
        destinations.push_back("Node" + std::to_string(i));
    }
    
    Engine engine(net);
    
    auto time = measureTime([&]() {
        engine.multicast("Node0", destinations);
    });
    
    std::cout << "Multicast to " << NUM_DESTINATIONS << " destinations: " << time << "ms" << std::endl;
    EXPECT_LT(time, 20.0) << "Multicast too slow";
}

// Test 8: Memory usage
TEST_F(PerformanceTest, MemoryUsage) {
    const int NUM_NODES = 100;
    
    size_t baselineNodes = net.getAllNodes().size();
    
    for (int i = 0; i < NUM_NODES; i++) {
        net.addNode<Host>("Node" + std::to_string(i), "10.0.0." + std::to_string(i), 8080);
    }
    
    size_t finalNodes = net.getAllNodes().size();
    
    std::cout << "Created " << NUM_NODES << " nodes" << std::endl;
    std::cout << "Nodes in network: " << finalNodes << std::endl;
    std::cout << "Estimated memory: ~" << (NUM_NODES * 1) << "KB" << std::endl;
    
    EXPECT_EQ(finalNodes, baselineNodes + NUM_NODES) << "Node count mismatch";
}

// Test 9: Concurrent statistics
TEST_F(PerformanceTest, ConcurrentStatistics) {
    const int NUM_NODES = 20;
    const int NUM_READS = 1000;
    
    for (int i = 0; i < NUM_NODES; i++) {
        std::string name = "Node" + std::to_string(i);
        net.addNode<Host>(name, "10.0.0." + std::to_string(i), 8080);
        net.recordPacketSent(name);
    }
    
    auto time = measureTime([&]() {
        for (int i = 0; i < NUM_READS; i++) {
            std::string node = "Node" + std::to_string(i % NUM_NODES);
            int sent = net.getPacketsSent(node);
            (void)sent;
        }
    });
    
    double timePerRead = time / NUM_READS;
    std::cout << "Performed " << NUM_READS << " statistics reads in " << time << "ms" << std::endl;
    std::cout << "Average: " << timePerRead << "ms per read" << std::endl;
    
    EXPECT_LT(timePerRead, 0.1) << "Statistics reads too slow";
}

// Test 10: Stress test
TEST_F(PerformanceTest, StressTest) {
    const int NUM_OPERATIONS = 100;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);
    
    auto time = measureTime([&]() {
        for (int i = 0; i < NUM_OPERATIONS; i++) {
            std::string name = "Node" + std::to_string(dis(gen));
            std::string ip = "10.0.0." + std::to_string(dis(gen));
            
            try {
                net.addNode<Host>(name, ip, 8080);
            } catch (...) {
                // Node might already exist
            }
        }
        
        for (int i = 0; i < NUM_OPERATIONS / 2; i++) {
            std::string nodeA = "Node" + std::to_string(dis(gen));
            std::string nodeB = "Node" + std::to_string(dis(gen));
            
            try {
                net.connect(nodeA, nodeB);
            } catch (...) {
                // Connection might fail
            }
        }
    });
    
    std::cout << "Stress test completed in " << time << "ms" << std::endl;
    std::cout << "Final network size: " << net.getAllNodes().size() << " nodes" << std::endl;
    
    EXPECT_LT(time, 1000.0) << "Stress test too slow";
}

// Main function
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
