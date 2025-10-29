#!/bin/bash

echo "=== NetSimCPP Build & Test Setup ==="
echo ""

# Kolory
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Krok 1: Sprawdź MySQL
echo -e "${YELLOW}Step 1: Checking MySQL...${NC}"
if docker ps --filter name=netsim-mysql --format "{{.Status}}" | grep -q "healthy"; then
    echo -e "${GREEN}✓ MySQL is running and healthy${NC}"
else
    echo -e "${RED}✗ MySQL is not ready${NC}"
    echo "Please run: docker-compose up -d mysql"
    exit 1
fi
echo ""

# Krok 2: Zaimportuj schemat
echo -e "${YELLOW}Step 2: Importing authentication schema...${NC}"
docker exec -i netsim-mysql mysql -uroot -pNetSimCPP1234 netsim < project/database/AuthSchema.sql 2>&1 | grep -v "Warning" || true
echo -e "${GREEN}✓ Schema import complete${NC}"
echo ""

# Krok 3: Sprawdź tabele
echo -e "${YELLOW}Step 3: Verifying tables...${NC}"
TABLES=$(docker exec netsim-mysql mysql -uroot -pNetSimCPP1234 -e "SHOW TABLES FROM netsim;" 2>&1 | grep -E "users|permissions|rate_limit|audit" | wc -l)
if [ "$TABLES" -ge 4 ]; then
    echo -e "${GREEN}✓ Authentication tables created ($TABLES found)${NC}"
    docker exec netsim-mysql mysql -uroot -pNetSimCPP1234 -e "SHOW TABLES FROM netsim;" 2>&1 | grep -v "Warning"
else
    echo -e "${YELLOW}⚠ Expected 4+ auth tables, found $TABLES${NC}"
fi
echo ""

# Krok 4: Sprawdź Redis
echo -e "${YELLOW}Step 4: Checking Redis...${NC}"
if redis-cli -a "NetSimCPP1234" ping 2>&1 | grep -q "PONG"; then
    echo -e "${GREEN}✓ Redis is running (with password)${NC}"
else
    echo -e "${RED}✗ Redis is not running or password incorrect${NC}"
    echo "Please start Redis: sudo systemctl start redis-server"
    echo "Redis password should be: NetSimCPP1234"
fi
echo ""

# Krok 5: Sprawdź zależności
echo -e "${YELLOW}Step 5: Checking dependencies...${NC}"
MISSING=0

if dpkg -l | grep -q libhiredis-dev; then
    echo -e "${GREEN}✓ libhiredis-dev${NC}"
else
    echo -e "${RED}✗ libhiredis-dev${NC}"
    MISSING=1
fi

if dpkg -l | grep -q libargon2-dev; then
    echo -e "${GREEN}✓ libargon2-dev${NC}"
else
    echo -e "${RED}✗ libargon2-dev${NC}"
    MISSING=1
fi

if [ -d "/usr/local/include/jwt-cpp" ]; then
    echo -e "${GREEN}✓ jwt-cpp${NC}"
else
    echo -e "${RED}✗ jwt-cpp${NC}"
    MISSING=1
fi

if dpkg -l | grep -q picojson-dev; then
    echo -e "${GREEN}✓ picojson-dev${NC}"
else
    echo -e "${RED}✗ picojson-dev${NC}"
    MISSING=1
fi

if [ "$MISSING" -eq 1 ]; then
    echo -e "${YELLOW}Run: sudo ./scripts/install_auth_deps.sh${NC}"
fi
echo ""

# Krok 6: Build
echo -e "${YELLOW}Step 6: Building project...${NC}"

# Przejdź do katalogu backend (NIE build!)
BUILD_DIR="/home/patryk/inzynieria_oprogramowania/project/backend"
cd "$BUILD_DIR" || {
    echo -e "${RED}✗ Cannot access directory: $BUILD_DIR${NC}"
    exit 1
}

echo "Current directory: $(pwd)"

# Wyczyść poprzednie pliki build w głównym katalogu
echo "Cleaning previous build files..."
rm -f CMakeCache.txt Makefile cmake_install.cmake netsim 2>/dev/null || true
rm -rf CMakeFiles 2>/dev/null || true

# Uruchom CMake w głównym katalogu backend (zbuduje w miejscu)
echo "Running CMake in $(pwd)..."
if cmake . 2>&1 | tail -10; then
    echo -e "${GREEN}✓ CMake configuration successful${NC}"
else
    echo -e "${RED}✗ CMake failed${NC}"
    exit 1
fi

# Sprawdź czy Makefile został utworzony
if [ ! -f "Makefile" ]; then
    echo -e "${RED}✗ Makefile not generated${NC}"
    exit 1
fi

echo "Compiling (this may take 30-60 seconds)..."
if make -j$(nproc) 2>&1 | tail -15; then
    echo -e "${GREEN}✓ Compilation successful${NC}"
else
    echo -e "${RED}✗ Compilation failed${NC}"
    exit 1
fi

if [ -f "netsim" ]; then
    echo -e "${GREEN}✓ Build successful!${NC}"
    ls -lh netsim
else
    echo -e "${RED}✗ Build failed. netsim executable not found.${NC}"
    exit 1
fi
echo ""

# Krok 7: Informacja o uruchomieniu
echo -e "${GREEN}=== Setup Complete! ===${NC}"
echo ""
echo "To run the server:"
echo "  cd /home/patryk/inzynieria_oprogramowania/project/backend"
echo "  export JWT_SECRET='netsim-secret-key-change-in-production-12345'"
echo "  export DB_HOST='localhost'"
echo "  export DB_USER='root'"
echo "  export DB_PASS='NetSimCPP1234'"
echo "  export DB_NAME='netsim'"
echo "  export REDIS_HOST='localhost'"
echo "  export REDIS_PORT='6379'"
echo "  export REDIS_PASS='NetSimCPP1234'"
echo "  ./netsim"
echo ""
echo "OR use the helper script:"
echo "  cd project/backend/build"
echo "  ./run_server.sh"
echo ""
echo "To run tests (in another terminal):"
echo "  cd /home/patryk/inzynieria_oprogramowania"
echo "  ./test_auth.sh"
echo ""
