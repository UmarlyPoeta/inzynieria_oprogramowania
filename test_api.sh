#!/bin/bash

# Quick API Test - NetSimCPP
# Testy podstawowych endpointów API

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}╔═══════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   NetSimCPP API Quick Test               ║${NC}"
echo -e "${BLUE}╚═══════════════════════════════════════════╝${NC}"
echo ""

BASE_URL="http://localhost:8080"

# Test 1: Status endpoint
echo -e "${YELLOW}[1/6]${NC} Testing /status..."
RESPONSE=$(curl -s -w "\n%{http_code}" ${BASE_URL}/status)
HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ]; then
    echo -e "${GREEN}✓${NC} Status OK"
    echo "  Response: $BODY"
else
    echo -e "${RED}✗${NC} Status endpoint failed (HTTP $HTTP_CODE)"
fi
echo ""

# Test 2: Add node
echo -e "${YELLOW}[2/6]${NC} Testing POST /node/add..."
RESPONSE=$(curl -s -w "\n%{http_code}" -X POST ${BASE_URL}/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"TestNode","ip":"10.0.0.1","type":"host","port":8080}')
HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ]; then
    echo -e "${GREEN}✓${NC} Node added"
    echo "  Response: $BODY"
else
    echo -e "${RED}✗${NC} Add node failed (HTTP $HTTP_CODE)"
    echo "  Response: $BODY"
fi
echo ""

# Test 3: List topology
echo -e "${YELLOW}[3/6]${NC} Testing GET /topology..."
RESPONSE=$(curl -s -w "\n%{http_code}" ${BASE_URL}/topology)
HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ]; then
    echo -e "${GREEN}✓${NC} Topology retrieved"
    echo "  Response: ${BODY:0:100}..."
else
    echo -e "${RED}✗${NC} Get topology failed (HTTP $HTTP_CODE)"
fi
echo ""

# Test 4: Add link
echo -e "${YELLOW}[4/6]${NC} Testing POST /link/add..."
# Najpierw dodaj drugi node
curl -s -X POST ${BASE_URL}/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"TestNode2","ip":"10.0.0.2","type":"host"}' > /dev/null

RESPONSE=$(curl -s -w "\n%{http_code}" -X POST ${BASE_URL}/link/add \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"TestNode","nodeB":"TestNode2"}')
HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ]; then
    echo -e "${GREEN}✓${NC} Link added"
    echo "  Response: $BODY"
else
    echo -e "${RED}✗${NC} Add link failed (HTTP $HTTP_CODE)"
    echo "  Response: $BODY"
fi
echo ""

# Test 5: Send packet
echo -e "${YELLOW}[5/6]${NC} Testing POST /packet/send..."
RESPONSE=$(curl -s -w "\n%{http_code}" -X POST ${BASE_URL}/packet/send \
  -H "Content-Type: application/json" \
  -d '{"src":"TestNode","dst":"TestNode2","size":64}')
HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ]; then
    echo -e "${GREEN}✓${NC} Packet sent"
    echo "  Response: $BODY"
else
    echo -e "${RED}✗${NC} Send packet failed (HTTP $HTTP_CODE)"
    echo "  Response: $BODY"
fi
echo ""

# Test 6: Authentication (jeśli dostępne)
echo -e "${YELLOW}[6/6]${NC} Testing POST /auth/register..."
RESPONSE=$(curl -s -w "\n%{http_code}" -X POST ${BASE_URL}/auth/register \
  -H "Content-Type: application/json" \
  -d '{"username":"test","email":"test@example.com","password":"Test123!"}')
HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | head -n-1)

if [ "$HTTP_CODE" = "200" ]; then
    echo -e "${GREEN}✓${NC} Auth available"
    echo "  Response: $BODY"
else
    echo -e "${YELLOW}⚠${NC}  Auth not available (expected if MySQL not running)"
    echo "  Response: $BODY"
fi
echo ""

# Podsumowanie
echo -e "${BLUE}╔═══════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   API Test Complete                       ║${NC}"
echo -e "${BLUE}╚═══════════════════════════════════════════╝${NC}"
echo ""
echo -e "${GREEN}Dostępne endpointy:${NC}"
echo "  GET  /status           - Server status"
echo "  GET  /topology         - Network topology"
echo "  POST /node/add         - Add node"
echo "  POST /node/remove      - Remove node"
echo "  POST /link/add         - Add link"
echo "  POST /link/remove      - Remove link"
echo "  POST /packet/send      - Send packet"
echo "  POST /simulation/start - Start simulation"
echo "  POST /simulation/stop  - Stop simulation"
echo ""
echo "  POST /auth/register    - Register (requires MySQL)"
echo "  POST /auth/login       - Login (requires MySQL)"
echo ""
echo "📖 Pełna dokumentacja: docs/api.md"
echo ""
