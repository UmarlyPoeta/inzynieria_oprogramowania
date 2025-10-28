#!/bin/bash

# Test Database Integration for NetSimCPP
# This script tests the database persistence functionality

BASE_URL="http://localhost:8080"

echo "========================================="
echo "NetSimCPP Database Integration Test"
echo "========================================="
echo ""

# Test 1: Check server status
echo "1. Checking server status..."
curl -s "$BASE_URL/status" | jq .
echo ""

# Test 2: Enable database persistence
echo "2. Enabling database persistence..."
curl -s -X POST "$BASE_URL/db/enable" \
  -H "Content-Type: application/json" \
  -d '{
    "host": "127.0.0.1",
    "port": 3306,
    "user": "root",
    "password": "NetSimCPP1234",
    "database": "netsim"
  }' | jq .
echo ""

# Test 3: Check database status
echo "3. Checking database status..."
curl -s "$BASE_URL/db/status" | jq .
echo ""

# Test 4: Create some nodes
echo "4. Creating test nodes..."
curl -s -X POST "$BASE_URL/node/add" \
  -H "Content-Type: application/json" \
  -d '{"name":"H1", "type":"host", "ip":"10.0.0.1"}' | jq .

curl -s -X POST "$BASE_URL/node/add" \
  -H "Content-Type: application/json" \
  -d '{"name":"H2", "type":"host", "ip":"10.0.0.2"}' | jq .

curl -s -X POST "$BASE_URL/node/add" \
  -H "Content-Type: application/json" \
  -d '{"name":"R1", "type":"router", "ip":"10.0.0.254"}' | jq .
echo ""

# Test 5: Connect nodes
echo "5. Connecting nodes..."
curl -s -X POST "$BASE_URL/link/connect" \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"H1", "nodeB":"R1"}' | jq .

curl -s -X POST "$BASE_URL/link/connect" \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"H2", "nodeB":"R1"}' | jq .
echo ""

# Test 6: List all nodes
echo "6. Listing all nodes..."
curl -s "$BASE_URL/nodes" | jq .
echo ""

# Test 7: Save topology to database
echo "7. Saving topology to database..."
curl -s "$BASE_URL/db/save" | jq .
echo ""

# Test 8: Verify database content
echo "8. Verifying database content (via MySQL)..."
mysql -h 127.0.0.1 -u root -pNetSimCPP1234 netsim -e "
SELECT 'Nodes in database:' AS info;
SELECT id, name, ip, type FROM nodes;
SELECT 'Links in database:' AS info;
SELECT l.id, n1.name AS node_a, n2.name AS node_b, delay_ms, bandwidth_mbps 
FROM links l 
JOIN nodes n1 ON l.node_a_id = n1.id 
JOIN nodes n2 ON l.node_b_id = n2.id;
"
echo ""

# Test 9: Remove all nodes from memory
echo "9. Removing nodes from memory (simulating restart)..."
curl -s -X POST "$BASE_URL/node/remove" \
  -H "Content-Type: application/json" \
  -d '{"name":"H1"}' | jq .

curl -s -X POST "$BASE_URL/node/remove" \
  -H "Content-Type: application/json" \
  -d '{"name":"H2"}' | jq .

curl -s -X POST "$BASE_URL/node/remove" \
  -H "Content-Type: application/json" \
  -d '{"name":"R1"}' | jq .

echo ""
echo "10. Verifying nodes removed from memory..."
curl -s "$BASE_URL/nodes" | jq .
echo ""

# Test 10: Load topology from database
echo "11. Loading topology from database..."
curl -s "$BASE_URL/db/load" | jq .
echo ""

# Test 11: Verify nodes restored
echo "12. Verifying nodes restored from database..."
curl -s "$BASE_URL/nodes" | jq .
echo ""

# Test 12: Disable database
echo "13. Disabling database persistence..."
curl -s -X POST "$BASE_URL/db/disable" | jq .
echo ""

echo "========================================="
echo "Database Integration Test Complete!"
echo "========================================="
