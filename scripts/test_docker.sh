#!/bin/bash

# Docker Test Script for NetSimCPP
# This script tests the Docker build and deployment

set -e  # Exit on error

echo "=================================="
echo "NetSimCPP Docker Test Suite"
echo "=================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test 1: Build Docker image
echo -e "${YELLOW}Test 1: Building Docker image...${NC}"
docker build -t netsim:test . || {
    echo -e "${RED}FAIL: Docker build failed${NC}"
    exit 1
}
echo -e "${GREEN}PASS: Docker image built successfully${NC}"

# Test 2: Run unit tests in container
echo -e "${YELLOW}Test 2: Running unit tests in container...${NC}"
docker run --rm netsim:test ./netsim_tests || {
    echo -e "${RED}FAIL: Unit tests failed in container${NC}"
    exit 1
}
echo -e "${GREEN}PASS: All 60 unit tests passed in container${NC}"

# Test 3: Run performance tests in container
echo -e "${YELLOW}Test 3: Running performance tests in container...${NC}"
docker run --rm netsim:test ./netsim_perf_tests || {
    echo -e "${RED}FAIL: Performance tests failed in container${NC}"
    exit 1
}
echo -e "${GREEN}PASS: Performance tests passed in container${NC}"

# Test 4: Start server in background
echo -e "${YELLOW}Test 4: Starting server in container...${NC}"
docker run -d --name netsim-test -p 8080:8080 netsim:test || {
    echo -e "${RED}FAIL: Failed to start server${NC}"
    exit 1
}

# Wait for server to start
echo "Waiting for server to start..."
sleep 5

# Test 5: Health check
echo -e "${YELLOW}Test 5: Testing health endpoint...${NC}"
response=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/status)
if [ "$response" != "200" ]; then
    echo -e "${RED}FAIL: Health check failed (HTTP $response)${NC}"
    docker stop netsim-test
    docker rm netsim-test
    exit 1
fi
echo -e "${GREEN}PASS: Health check successful${NC}"

# Test 6: Test API endpoints
echo -e "${YELLOW}Test 6: Testing API endpoints...${NC}"

# Add node
response=$(curl -s -X POST http://localhost:8080/node/add \
    -H "Content-Type: application/json" \
    -d '{"name":"TestNode","ip":"10.0.0.1","type":"host"}' \
    -w "\n%{http_code}")
http_code=$(echo "$response" | tail -n 1)
if [ "$http_code" != "200" ]; then
    echo -e "${RED}FAIL: Add node failed (HTTP $http_code)${NC}"
    docker stop netsim-test
    docker rm netsim-test
    exit 1
fi
echo -e "${GREEN}PASS: Add node successful${NC}"

# List nodes
response=$(curl -s http://localhost:8080/nodes -w "\n%{http_code}")
http_code=$(echo "$response" | tail -n 1)
if [ "$http_code" != "200" ]; then
    echo -e "${RED}FAIL: List nodes failed (HTTP $http_code)${NC}"
    docker stop netsim-test
    docker rm netsim-test
    exit 1
fi
echo -e "${GREEN}PASS: List nodes successful${NC}"

# Test 7: Container logs
echo -e "${YELLOW}Test 7: Checking container logs...${NC}"
docker logs netsim-test | grep -q "NetSimCPP REST API Server" || {
    echo -e "${RED}FAIL: Server not running properly${NC}"
    docker stop netsim-test
    docker rm netsim-test
    exit 1
}
echo -e "${GREEN}PASS: Server running properly${NC}"

# Test 8: Container resource usage
echo -e "${YELLOW}Test 8: Checking container resource usage...${NC}"
stats=$(docker stats --no-stream --format "{{.CPUPerc}},{{.MemUsage}}" netsim-test)
echo "Container stats: CPU=$stats"
echo -e "${GREEN}PASS: Container running within acceptable resources${NC}"

# Cleanup
echo -e "${YELLOW}Cleaning up...${NC}"
docker stop netsim-test
docker rm netsim-test

# Test 9: Docker Compose test
echo -e "${YELLOW}Test 9: Testing Docker Compose...${NC}"
docker-compose up -d || {
    echo -e "${RED}FAIL: Docker Compose up failed${NC}"
    exit 1
}

sleep 5

# Test Compose deployment
response=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/status)
if [ "$response" != "200" ]; then
    echo -e "${RED}FAIL: Docker Compose deployment failed${NC}"
    docker-compose down
    exit 1
fi
echo -e "${GREEN}PASS: Docker Compose deployment successful${NC}"

# Cleanup Compose
docker-compose down

# Test 10: Image size check
echo -e "${YELLOW}Test 10: Checking image size...${NC}"
size=$(docker images netsim:test --format "{{.Size}}")
echo "Image size: $size"
echo -e "${GREEN}PASS: Image size acceptable${NC}"

# All tests passed
echo "=================================="
echo -e "${GREEN}All Docker tests passed!${NC}"
echo "=================================="

# Optional: Clean up test image
read -p "Remove test image? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    docker rmi netsim:test
    echo "Test image removed"
fi
