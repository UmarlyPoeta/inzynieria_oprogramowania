#!/bin/bash

set -e  # Exit on error

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}=== NetSimCPP Complete Build Script ===${NC}\n"

# Step 1: Go to build directory
BUILD_DIR="/home/patryk/inzynieria_oprogramowania/project/backend/build"
echo -e "${YELLOW}Step 1: Navigating to build directory${NC}"
cd "$BUILD_DIR" || exit 1
echo "Current directory: $(pwd)"
echo ""

# Step 2: Clean build directory
echo -e "${YELLOW}Step 2: Cleaning build directory${NC}"
rm -rf CMake* Makefile cmake_install.cmake *.cmake NetSimCPP 2>/dev/null || true
echo -e "${GREEN}✓ Build directory cleaned${NC}"
echo ""

# Step 3: Run CMake
echo -e "${YELLOW}Step 3: Running CMake configuration${NC}"
if cmake .. ; then
    echo -e "${GREEN}✓ CMake configuration successful${NC}"
else
    echo -e "${RED}✗ CMake configuration failed!${NC}"
    exit 1
fi
echo ""

# Step 4: Check if Makefile was generated
echo -e "${YELLOW}Step 4: Verifying Makefile${NC}"
if [ -f "Makefile" ]; then
    echo -e "${GREEN}✓ Makefile generated${NC}"
else
    echo -e "${RED}✗ Makefile not found!${NC}"
    exit 1
fi
echo ""

# Step 5: Compile
echo -e "${YELLOW}Step 5: Compiling project (this may take 30-60 seconds)${NC}"
if make -j$(nproc) ; then
    echo -e "${GREEN}✓ Compilation successful!${NC}"
else
    echo -e "${RED}✗ Compilation failed!${NC}"
    exit 1
fi
echo ""

# Step 6: Verify executable
echo -e "${YELLOW}Step 6: Verifying executable${NC}"
if [ -f "NetSimCPP" ]; then
    echo -e "${GREEN}✓ NetSimCPP executable created!${NC}"
    ls -lh NetSimCPP
else
    echo -e "${RED}✗ NetSimCPP executable not found!${NC}"
    exit 1
fi
echo ""

# Step 7: Check Redis
echo -e "${YELLOW}Step 7: Checking Redis${NC}"
if redis-cli -a "NetSimCPP1234" ping 2>&1 | grep -q "PONG"; then
    echo -e "${GREEN}✓ Redis is running${NC}"
else
    echo -e "${YELLOW}⚠ Redis not responding with password${NC}"
    echo "Try: sudo systemctl start redis-server"
fi
echo ""

# Step 8: Check MySQL
echo -e "${YELLOW}Step 8: Checking MySQL${NC}"
if docker ps --filter name=netsim-mysql --format "{{.Status}}" | grep -q "healthy"; then
    echo -e "${GREEN}✓ MySQL is healthy${NC}"
else
    echo -e "${YELLOW}⚠ MySQL not healthy${NC}"
    echo "Try: docker-compose up -d mysql"
fi
echo ""

echo -e "${GREEN}=== Build Complete! ===${NC}\n"
echo "To run the server:"
echo "  cd $BUILD_DIR"
echo "  ./run_server.sh"
echo ""
echo "To test (in another terminal):"
echo "  cd /home/patryk/inzynieria_oprogramowania"
echo "  ./test_auth.sh"
