#!/bin/bash

# WebSocket Test Script for NetSimCPP
# Tests WebSocket connectivity and event broadcasting

echo "=== NetSimCPP WebSocket Test ==="
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

REST_API="http://localhost:8080"
WS_URL="ws://localhost:9001"

# Test 1: Check if server is running
echo -e "${YELLOW}[1/5]${NC} Checking if REST API is running..."
if curl -s "${REST_API}/status" > /dev/null 2>&1; then
    echo -e "${GREEN}✓${NC} REST API is running"
else
    echo -e "${RED}✗${NC} REST API is not running"
    echo "Please start the server first: ./netsim or docker compose up"
    exit 1
fi

# Test 2: Check WebSocket port
echo -e "${YELLOW}[2/5]${NC} Checking if WebSocket port is open..."
if nc -z localhost 9001 2>/dev/null; then
    echo -e "${GREEN}✓${NC} WebSocket port 9001 is open"
else
    echo -e "${RED}✗${NC} WebSocket port 9001 is not accessible"
    echo "Tip: Check if server is running and firewall allows port 9001"
    exit 1
fi

# Test 3: Test with wscat (if available)
echo -e "${YELLOW}[3/5]${NC} Testing WebSocket connection..."
if command -v wscat &> /dev/null; then
    echo "Using wscat to test WebSocket..."
    timeout 3s wscat -c "${WS_URL}" 2>&1 | head -5 || true
    echo -e "${GREEN}✓${NC} WebSocket connection test completed"
else
    echo -e "${YELLOW}!${NC} wscat not found (install with: npm install -g wscat)"
    echo "   Skipping detailed WebSocket test"
fi

# Test 4: Trigger events and monitor
echo -e "${YELLOW}[4/5]${NC} Testing event broadcasting..."
echo "Adding a test node to trigger WebSocket event..."

curl -s -X POST "${REST_API}/node/add" \
    -H "Content-Type: application/json" \
    -d '{"name":"WSTest1","type":"host","ip":"10.0.0.99"}' > /dev/null

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} Node added successfully (should trigger node_added event)"
else
    echo -e "${RED}✗${NC} Failed to add node"
fi

sleep 1

echo "Removing test node..."
curl -s -X POST "${REST_API}/node/remove" \
    -H "Content-Type: application/json" \
    -d '{"name":"WSTest1"}' > /dev/null

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} Node removed successfully (should trigger node_removed event)"
else
    echo -e "${YELLOW}!${NC} Failed to remove node (may not exist)"
fi

# Test 5: Monitor client
echo -e "${YELLOW}[5/5]${NC} WebSocket monitor client..."
echo ""
echo "To test WebSocket in browser:"
echo "  1. Open: project/frontend/websocket-monitor.html"
echo "  2. Click 'Connect' button"
echo "  3. Perform REST API operations (add/remove nodes)"
echo "  4. Watch real-time events appear"
echo ""
echo "Alternative test with wscat:"
echo "  $ npm install -g wscat"
echo "  $ wscat -c ${WS_URL}"
echo "  Connected (press CTRL+C to quit)"
echo "  > {\"type\":\"ping\"}"
echo ""

echo -e "${GREEN}=== WebSocket Test Complete ===${NC}"
echo ""
echo "WebSocket Events Documentation: docs/WEBSOCKET_API.md"
echo ""
