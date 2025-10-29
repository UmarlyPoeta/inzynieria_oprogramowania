#!/bin/bash

# Kolory dla outputu
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

BASE_URL="http://localhost:8080"

echo -e "${YELLOW}=== NetSimCPP Authentication Tests ===${NC}\n"

# Test 1: Sprawdź status serwera
echo -e "${YELLOW}Test 1: Server Status${NC}"
RESPONSE=$(curl -s -w "\n%{http_code}" ${BASE_URL}/status 2>/dev/null)
HTTP_CODE=$(echo "$RESPONSE" | tail -1)
BODY=$(echo "$RESPONSE" | head -n -1)

if [ "$HTTP_CODE" == "200" ]; then
    echo -e "${GREEN}✓ Server is running${NC}"
    echo "$BODY" | jq '.' 2>/dev/null || echo "$BODY"
else
    echo -e "${RED}✗ Server not responding (HTTP $HTTP_CODE)${NC}"
    exit 1
fi

echo ""

# Test 2: Register new user
echo -e "${YELLOW}Test 2: Register New User${NC}"
REGISTER_RESPONSE=$(curl -s -w "\n%{http_code}" -X POST ${BASE_URL}/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "username": "testuser_'$(date +%s)'",
    "email": "test@example.com",
    "password": "TestPass123!",
    "role": "user"
  }' 2>/dev/null)

HTTP_CODE=$(echo "$REGISTER_RESPONSE" | tail -1)
BODY=$(echo "$REGISTER_RESPONSE" | head -n -1)

if [ "$HTTP_CODE" == "201" ]; then
    echo -e "${GREEN}✓ User registered successfully${NC}"
    echo "$BODY" | jq '.' 2>/dev/null || echo "$BODY"
else
    echo -e "${YELLOW}⚠ Registration returned HTTP $HTTP_CODE (might be duplicate)${NC}"
    echo "$BODY" | jq '.' 2>/dev/null || echo "$BODY"
fi

echo ""

# Test 3: Login with admin
echo -e "${YELLOW}Test 3: Login as Admin${NC}"
LOGIN_RESPONSE=$(curl -s -w "\n%{http_code}" -X POST ${BASE_URL}/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "username": "admin",
    "password": "admin123"
  }' 2>/dev/null)

HTTP_CODE=$(echo "$LOGIN_RESPONSE" | tail -1)
BODY=$(echo "$LOGIN_RESPONSE" | head -n -1)

if [ "$HTTP_CODE" == "200" ]; then
    echo -e "${GREEN}✓ Login successful${NC}"
    TOKEN=$(echo "$BODY" | jq -r '.token' 2>/dev/null)
    echo "Token: ${TOKEN:0:50}..."
    echo "$BODY" | jq '.' 2>/dev/null || echo "$BODY"
else
    echo -e "${RED}✗ Login failed (HTTP $HTTP_CODE)${NC}"
    echo "$BODY"
    exit 1
fi

echo ""

# Test 4: Get current user info
echo -e "${YELLOW}Test 4: Get Current User Info (/auth/me)${NC}"
ME_RESPONSE=$(curl -s -w "\n%{http_code}" -X GET ${BASE_URL}/auth/me \
  -H "Authorization: Bearer $TOKEN" 2>/dev/null)

HTTP_CODE=$(echo "$ME_RESPONSE" | tail -1)
BODY=$(echo "$ME_RESPONSE" | head -n -1)

if [ "$HTTP_CODE" == "200" ]; then
    echo -e "${GREEN}✓ User info retrieved${NC}"
    echo "$BODY" | jq '.' 2>/dev/null || echo "$BODY"
else
    echo -e "${RED}✗ Failed to get user info (HTTP $HTTP_CODE)${NC}"
    echo "$BODY"
fi

echo ""

# Test 5: Protected endpoint - Add node (with auth)
echo -e "${YELLOW}Test 5: Protected Endpoint - Add Node${NC}"
NODE_RESPONSE=$(curl -s -w "\n%{http_code}" -X POST ${BASE_URL}/node/add \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "name": "test_router_'$(date +%s)'",
    "ip": "192.168.1.1",
    "type": "router"
  }' 2>/dev/null)

HTTP_CODE=$(echo "$NODE_RESPONSE" | tail -1)
BODY=$(echo "$NODE_RESPONSE" | head -n -1)

if [ "$HTTP_CODE" == "200" ]; then
    echo -e "${GREEN}✓ Node created successfully (auth working!)${NC}"
    echo "$BODY" | jq '.' 2>/dev/null || echo "$BODY"
else
    echo -e "${RED}✗ Failed to create node (HTTP $HTTP_CODE)${NC}"
    echo "$BODY"
fi

echo ""

# Test 6: Protected endpoint without token (should fail)
echo -e "${YELLOW}Test 6: Protected Endpoint Without Token (should fail)${NC}"
NOAUTH_RESPONSE=$(curl -s -w "\n%{http_code}" -X POST ${BASE_URL}/node/add \
  -H "Content-Type: application/json" \
  -d '{
    "name": "unauthorized_node",
    "ip": "192.168.1.99",
    "type": "host"
  }' 2>/dev/null)

HTTP_CODE=$(echo "$NOAUTH_RESPONSE" | tail -1)
BODY=$(echo "$NOAUTH_RESPONSE" | head -n -1)

if [ "$HTTP_CODE" == "401" ]; then
    echo -e "${GREEN}✓ Correctly rejected (HTTP 401 Unauthorized)${NC}"
    echo "$BODY" | jq '.' 2>/dev/null || echo "$BODY"
else
    echo -e "${RED}✗ Should have returned 401, got HTTP $HTTP_CODE${NC}"
    echo "$BODY"
fi

echo ""

# Test 7: Rate limiting test
echo -e "${YELLOW}Test 7: Rate Limiting (sending 5 requests quickly)${NC}"
for i in {1..5}; do
    RATE_RESPONSE=$(curl -s -w "\n%{http_code}" -X GET ${BASE_URL}/status 2>/dev/null)
    HTTP_CODE=$(echo "$RATE_RESPONSE" | tail -1)
    echo "Request $i: HTTP $HTTP_CODE"
done

echo ""

# Test 8: Logout
echo -e "${YELLOW}Test 8: Logout${NC}"
LOGOUT_RESPONSE=$(curl -s -w "\n%{http_code}" -X POST ${BASE_URL}/auth/logout \
  -H "Authorization: Bearer $TOKEN" 2>/dev/null)

HTTP_CODE=$(echo "$LOGOUT_RESPONSE" | tail -1)
BODY=$(echo "$LOGOUT_RESPONSE" | head -n -1)

if [ "$HTTP_CODE" == "200" ]; then
    echo -e "${GREEN}✓ Logout successful${NC}"
    echo "$BODY" | jq '.' 2>/dev/null || echo "$BODY"
else
    echo -e "${RED}✗ Logout failed (HTTP $HTTP_CODE)${NC}"
    echo "$BODY"
fi

echo ""

# Test 9: Try to use token after logout (should fail)
echo -e "${YELLOW}Test 9: Use Token After Logout (should fail)${NC}"
AFTERLOGOUT_RESPONSE=$(curl -s -w "\n%{http_code}" -X GET ${BASE_URL}/auth/me \
  -H "Authorization: Bearer $TOKEN" 2>/dev/null)

HTTP_CODE=$(echo "$AFTERLOGOUT_RESPONSE" | tail -1)
BODY=$(echo "$AFTERLOGOUT_RESPONSE" | head -n -1)

if [ "$HTTP_CODE" == "401" ]; then
    echo -e "${GREEN}✓ Correctly rejected (HTTP 401 - token invalidated)${NC}"
    echo "$BODY" | jq '.' 2>/dev/null || echo "$BODY"
else
    echo -e "${RED}✗ Should have returned 401, got HTTP $HTTP_CODE${NC}"
    echo "$BODY"
fi

echo -e "\n${YELLOW}=== Tests Complete ===${NC}"
