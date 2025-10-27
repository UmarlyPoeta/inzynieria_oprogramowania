# Testing Documentation

## Overview

NetSimCPP includes comprehensive testing infrastructure:
- **60 Unit Tests**: Core functionality testing
- **10 Performance Tests**: Speed and resource usage testing
- **10 Docker Tests**: Container deployment validation
- **4 CI/CD Jobs**: Automated testing pipeline

---

## Unit Tests (60 tests)

### Running Unit Tests

```bash
cd project/backend/build
./netsim_tests

# Run specific test
./netsim_tests --gtest_filter=NetworkTest.Ping

# Run with verbose output
./netsim_tests --gtest_filter=NetworkTest.* --gtest_print_time=1

# Generate XML report
./netsim_tests --gtest_output=xml:test-results.xml
```

### Test Coverage

- **PacketTest** (6 tests): Packet construction, fragmentation, QoS
- **NodeTest** (3 tests): Node creation, packet handling
- **NetworkTest** (24 tests): Topology, connections, advanced features
- **EngineTest** (3 tests): Ping, traceroute, multicast
- **RouterTest** (3 tests): Routing, load balancing
- **Advanced** (21 tests): VLAN, firewall, TCP/UDP, statistics, wireless, cloud, IoT

---

## Performance Tests (10 tests)

### Running Performance Tests

```bash
cd project/backend/build
./netsim_perf_tests
```

### Performance Test Suite

1. **NodeCreationPerformance**: Tests node creation speed
   - Target: <1ms per node
   - Measures: 100 nodes creation time

2. **LinkCreationPerformance**: Tests link establishment speed
   - Target: <0.5ms per link
   - Measures: 50 links creation time

3. **PingPerformance**: Tests routing algorithm speed
   - Target: <5ms for 20-hop ping
   - Measures: BFS pathfinding performance

4. **LargeNetworkPerformance**: Tests scalability
   - Target: <500ms setup for 100 nodes
   - Target: <10ms ping in large network

5. **TopologyExportPerformance**: Tests serialization speed
   - Target: <100ms for 50-node topology
   - Measures: JSON export time

6. **StatisticsPerformance**: Tests monitoring overhead
   - Target: <0.01ms per operation
   - Measures: 1000 statistics operations

7. **MulticastPerformance**: Tests one-to-many transmission
   - Target: <20ms for 10 destinations
   - Measures: Multicast efficiency

8. **MemoryUsage**: Tests memory consumption
   - Estimates: ~1KB per node
   - Validates node count

9. **ConcurrentStatistics**: Tests read performance
   - Target: <0.01ms per read
   - Measures: 1000 concurrent reads

10. **StressTest**: Tests system under load
    - Target: <1000ms for 100 random operations
    - Tests resilience

### Performance Benchmarks

| Operation | Target | Typical |
|-----------|--------|---------|
| Node creation | <1ms | ~0.1ms |
| Link creation | <0.5ms | ~0.1ms |
| Ping (20 hops) | <5ms | ~1ms |
| Large network setup | <500ms | ~200ms |
| Topology export | <100ms | ~50ms |
| Statistics op | <0.01ms | ~0.001ms |

---

## Docker Tests

### Running Docker Tests

```bash
# Run full Docker test suite
./scripts/test_docker.sh
```

### Docker Test Suite

1. **Build Image**: Validates Dockerfile
2. **Unit Tests in Container**: Runs all 60 tests
3. **Performance Tests in Container**: Runs performance suite
4. **Start Server**: Launches REST API server
5. **Health Check**: Validates /status endpoint
6. **API Endpoints**: Tests node operations
7. **Container Logs**: Checks server output
8. **Resource Usage**: Monitors CPU/memory
9. **Docker Compose**: Tests orchestration
10. **Image Size**: Validates build efficiency

### Manual Docker Commands

```bash
# Build image
docker build -t netsim:latest .

# Run tests
docker run --rm netsim:latest ./netsim_tests

# Start server
docker run -d -p 8080:8080 --name netsim-server netsim:latest

# Check health
curl http://localhost:8080/status

# View logs
docker logs netsim-server

# Stop and remove
docker stop netsim-server
docker rm netsim-server

# Using Docker Compose
docker-compose up -d
docker-compose logs
docker-compose down
```

---

## CI/CD Pipeline

### GitHub Actions Workflow

The CI/CD pipeline runs automatically on:
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop`

### Jobs

#### 1. Build and Test
- Install dependencies
- Configure with CMake
- Build project
- Run all 60 unit tests
- Publish test results

#### 2. Performance Tests
- Build project
- Run performance test suite
- Memory leak check with Valgrind

#### 3. Docker Build
- Build Docker image
- Test image
- Start server and test API
- Push to Docker Hub (main branch only)

#### 4. Code Quality
- Run cppcheck static analysis
- Check for code issues

### Running CI/CD Locally

```bash
# Simulate GitHub Actions locally
./scripts/test_ci_cd.sh
```

### CI/CD Configuration

Location: `.github/workflows/ci-cd.yml`

Required secrets (for Docker Hub push):
- `DOCKER_USERNAME`
- `DOCKER_PASSWORD`

---

## Memory Testing

### Using Valgrind

```bash
# Check for memory leaks
valgrind --leak-check=full --show-leak-kinds=all ./netsim_tests

# Specific test
valgrind --leak-check=full ./netsim_tests --gtest_filter=NetworkTest.AddNode
```

### Expected Results
- No memory leaks
- All memory properly freed
- Smart pointers handling cleanup

---

## Load Testing

### API Load Testing with curl

```bash
# Simple load test
for i in {1..100}; do
    curl -X POST http://localhost:8080/node/add \
        -H "Content-Type: application/json" \
        -d "{\"name\":\"Node$i\",\"ip\":\"10.0.0.$i\",\"type\":\"host\"}"
done
```

### Using Apache Bench

```bash
# Install Apache Bench
sudo apt-get install apache2-utils

# Test status endpoint
ab -n 1000 -c 10 http://localhost:8080/status

# Test with POST
ab -n 100 -c 5 -p node.json -T application/json http://localhost:8080/node/add
```

---

## Continuous Integration Best Practices

### Before Committing

1. Run unit tests locally
```bash
cd project/backend/build && ./netsim_tests
```

2. Run performance tests
```bash
./netsim_perf_tests
```

3. Check code quality
```bash
cppcheck --enable=all project/backend/src/
```

4. Test Docker build
```bash
docker build -t netsim:test .
```

### Pull Request Checklist

- [ ] All 60 unit tests pass
- [ ] Performance tests pass
- [ ] Docker builds successfully
- [ ] No cppcheck warnings
- [ ] Documentation updated
- [ ] Changelog updated (if applicable)

---

## Troubleshooting Tests

### Common Issues

**Tests fail to build:**
```bash
# Clean and rebuild
cd project/backend
rm -rf build
mkdir build && cd build
cmake .. && make
```

**Docker build fails:**
```bash
# Check Docker daemon
docker info

# Clean Docker cache
docker system prune -a
```

**Performance tests fail:**
- May be due to system load
- Run on dedicated test machine
- Check baseline performance requirements

**Valgrind reports leaks:**
- Review smart pointer usage
- Check for circular references
- Ensure proper cleanup in destructors

---

## Test Metrics

### Current Test Statistics

- **Total Tests**: 70 (60 unit + 10 performance)
- **Pass Rate**: 100%
- **Code Coverage**: ~85% (estimated)
- **Average Test Time**: ~2 seconds (all tests)
- **Docker Build Time**: ~3 minutes
- **CI/CD Pipeline Time**: ~5 minutes

### Performance Baselines

Tested on: Ubuntu 22.04, Intel i5, 8GB RAM

- Node creation: 0.1ms average
- Link creation: 0.1ms average
- 100-node network setup: 200ms
- Ping in large network: 1-2ms
- Topology export (50 nodes): 50ms
- Docker container startup: 2-3 seconds

---

## Future Testing Enhancements

### Planned Additions

1. **Integration Tests**
   - End-to-end workflows
   - Multi-container scenarios
   - API integration tests

2. **Chaos Testing**
   - Random node failures
   - Network partitions
   - Resource exhaustion

3. **Benchmark Suite**
   - Comparison with other simulators
   - Historical performance tracking
   - Regression detection

4. **Security Testing**
   - API vulnerability scanning
   - Input validation tests
   - DOS resistance tests

5. **UI Tests** (when UI is added)
   - Selenium/Playwright tests
   - Visual regression tests
   - Accessibility tests

---

## Contributing to Tests

### Adding New Tests

1. **Unit Tests**: Add to `test_main.cpp`
```cpp
TEST(CategoryTest, NewFeature) {
    // Arrange
    Network net;
    
    // Act
    net.newFeature();
    
    // Assert
    ASSERT_TRUE(net.someCondition());
}
```

2. **Performance Tests**: Add to `performance_tests.cpp`
```cpp
TEST_F(PerformanceTest, NewFeaturePerformance) {
    auto time = measureTime([&]() {
        // Operation to measure
    });
    
    EXPECT_LT(time, targetTime);
}
```

3. **Docker Tests**: Add to `scripts/test_docker.sh`
```bash
echo -e "${YELLOW}Test N: Description...${NC}"
# Test commands
echo -e "${GREEN}PASS: Test passed${NC}"
```

### Test Naming Conventions

- Use descriptive names: `NetworkTest.AddNodeAndConnect`
- Group by feature: `WirelessTest.*`
- Performance tests: `*Performance`
- Negative tests: `*InvalidInput`, `*ErrorHandling`

---

## Resources

- [GoogleTest Documentation](https://google.github.io/googletest/)
- [Docker Documentation](https://docs.docker.com/)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Valgrind Documentation](https://valgrind.org/docs/manual/manual.html)
- [cppcheck Manual](http://cppcheck.sourceforge.net/manual.pdf)
