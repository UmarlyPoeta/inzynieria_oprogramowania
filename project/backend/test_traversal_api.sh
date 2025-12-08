#!/bin/bash

# Test script dla Traversal Algorithms API
# Użycie: ./test_traversal_api.sh

API_URL="http://localhost:8081/test/traversal"

echo "=== Testing Traversal Algorithms API ==="
echo ""

# 1. Help endpoint
echo "1. GET /help"
curl -X GET "${API_URL}/help" | jq .
echo ""
echo ""

# 2. ECMP Next Hops
echo "2. POST /ecmp/next-hops (A -> F)"
curl -X POST "${API_URL}/ecmp/next-hops" \
  -H "Content-Type: application/json" \
  -d '{
    "node": "B",
    "dst": "F"
  }' | jq .
echo ""
echo ""

# 3. Constrained Shortest Path
echo "3. POST /constrained-path (A -> F with min_bandwidth=100)"
curl -X POST "${API_URL}/constrained-path" \
  -H "Content-Type: application/json" \
  -d '{
    "src": "A",
    "dst": "F",
    "min_bandwidth": 100
  }' | jq .
echo ""
echo ""

# 4. Link-State Routing
echo "4. POST /link-state (node B)"
curl -X POST "${API_URL}/link-state" \
  -H "Content-Type: application/json" \
  -d '{
    "node": "B"
  }' | jq .
echo ""
echo ""

# 5. RPF Check
echo "5. POST /rpf (node E, incoming A, source A)"
curl -X POST "${API_URL}/rpf" \
  -H "Content-Type: application/json" \
  -d '{
    "node": "E",
    "incoming_neighbor": "B",
    "src": "A"
  }' | jq .
echo ""
echo ""

# 6. Multipath Flow-Aware
echo "6. POST /multipath (A -> F, k=3)"
curl -X POST "${API_URL}/multipath" \
  -H "Content-Type: application/json" \
  -d '{
    "src": "A",
    "dst": "F",
    "k": 3
  }' | jq .
echo ""
echo ""

# 7. Multi-Commodity Flow
echo "7. POST /multi-commodity (2 commodities)"
curl -X POST "${API_URL}/multi-commodity" \
  -H "Content-Type: application/json" \
  -d '{
    "commodities": [
      {"src": "A", "dst": "F", "demand": 50},
      {"src": "D", "dst": "C", "demand": 30}
    ],
    "k": 2
  }' | jq .
echo ""
echo ""

echo "=== Tests completed ==="
