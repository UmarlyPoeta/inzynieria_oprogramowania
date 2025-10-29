#!/bin/bash
# NetSimCPP - Dependencies Installation Script
# For Ubuntu 22.04 LTS

set -e

echo "=========================================="
echo "NetSimCPP Dependencies Installer"
echo "=========================================="
echo ""

# Check if running on Ubuntu/Debian
if ! command -v apt-get &> /dev/null; then
    echo "❌ Error: This script is for Ubuntu/Debian systems only"
    exit 1
fi

# Check Ubuntu version
if [ -f /etc/os-release ]; then
    . /etc/os-release
    echo "📦 Detected OS: $NAME $VERSION"
    if [[ "$VERSION_ID" != "22.04" ]]; then
        echo "⚠️  Warning: This script is tested on Ubuntu 22.04 LTS"
        echo "   Your version: $VERSION_ID"
        read -p "   Continue anyway? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
else
    echo "⚠️  Warning: Could not detect OS version"
fi

echo ""
echo "📥 Installing core dependencies..."
echo ""

sudo apt-get update

# Core build tools
echo "→ Installing build essentials..."
sudo apt-get install -y \
    build-essential \
    cmake \
    g++ \
    git \
    curl

# C++ REST SDK dependencies
echo "→ Installing C++ REST SDK..."
sudo apt-get install -y \
    libcpprest-dev \
    libssl-dev

# JSON library
echo "→ Installing JSON library..."
sudo apt-get install -y nlohmann-json3-dev

# MySQL Connector
echo "→ Installing MySQL client and connector..."
sudo apt-get install -y \
    libmysqlcppconn-dev \
    mysql-client

# WebSocket++ and Boost
echo "→ Installing WebSocket++ and Boost..."
sudo apt-get install -y \
    libwebsocketpp-dev \
    libboost-system-dev \
    libboost-thread-dev

# Testing frameworks (optional)
echo ""
read -p "📊 Install testing tools (GoogleTest, Valgrind, cppcheck)? (Y/n): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]; then
    echo "→ Installing testing tools..."
    sudo apt-get install -y \
        libgtest-dev \
        valgrind \
        cppcheck
fi

# Docker (optional)
echo ""
read -p "🐳 Install Docker and Docker Compose? (Y/n): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]; then
    echo "→ Installing Docker..."
    
    # Remove old versions
    sudo apt-get remove -y docker docker-engine docker.io containerd runc 2>/dev/null || true
    
    # Install dependencies
    sudo apt-get install -y \
        ca-certificates \
        curl \
        gnupg \
        lsb-release
    
    # Add Docker GPG key
    sudo mkdir -p /etc/apt/keyrings
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
    
    # Add Docker repository
    echo \
      "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
      $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
    
    # Install Docker
    sudo apt-get update
    sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
    
    # Add current user to docker group
    sudo usermod -aG docker $USER
    
    echo "⚠️  You need to log out and back in for Docker group changes to take effect"
fi

# Clean up
echo ""
echo "🧹 Cleaning up..."
sudo apt-get autoremove -y
sudo apt-get clean

# Verify installations
echo ""
echo "=========================================="
echo "✅ Installation Complete!"
echo "=========================================="
echo ""
echo "📋 Installed Versions:"
echo "-------------------"
echo "GCC:          $(g++ --version | head -n1 | awk '{print $4}')"
echo "CMake:        $(cmake --version | head -n1 | awk '{print $3}')"
echo "Git:          $(git --version | awk '{print $3}')"

# Check libraries
if pkg-config --exists cpprestsdk; then
    echo "cpprestsdk:   $(pkg-config --modversion cpprestsdk)"
else
    echo "cpprestsdk:   installed (version unavailable)"
fi

if [ -f /usr/include/nlohmann/json.hpp ]; then
    echo "nlohmann-json: installed ✓"
else
    echo "nlohmann-json: ✗ NOT FOUND"
fi

if [ -f /usr/include/websocketpp/server.hpp ]; then
    echo "WebSocket++:  installed ✓"
else
    echo "WebSocket++:  ✗ NOT FOUND"
fi

if dpkg -l | grep -q libboost-system-dev; then
    BOOST_VER=$(dpkg -l | grep libboost-system-dev | awk '{print $3}')
    echo "Boost:        $BOOST_VER"
else
    echo "Boost:        ✗ NOT FOUND"
fi

if command -v mysql &> /dev/null; then
    echo "MySQL Client: $(mysql --version | awk '{print $5}' | sed 's/,//')"
else
    echo "MySQL Client: ✗ NOT FOUND"
fi

if command -v docker &> /dev/null; then
    echo "Docker:       $(docker --version | awk '{print $3}' | sed 's/,//')"
else
    echo "Docker:       not installed"
fi

echo ""
echo "🎯 Next Steps:"
echo "-------------------"
echo "1. Build the project:"
echo "   cd project/backend"
echo "   cmake ."
echo "   make -j\$(nproc)"
echo ""
echo "2. Run tests:"
echo "   ./netsim_tests"
echo ""
echo "3. Start server:"
echo "   ./netsim"
echo ""
echo "4. Or use Docker:"
echo "   docker compose up -d"
echo ""
echo "📖 Documentation: README.md"
echo "🐛 Issues: https://github.com/UmarlyPoeta/inzynieria_oprogramowania/issues"
echo ""
