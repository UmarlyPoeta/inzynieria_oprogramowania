#!/bin/bash
# NetSimCPP - Authentication Dependencies Installer
# Adds Redis, Argon2, and JWT libraries

set -e

echo "=========================================="
echo "NetSimCPP Authentication Dependencies"
echo "=========================================="
echo ""

# Check if running on Ubuntu/Debian
if ! command -v apt-get &> /dev/null; then
    echo "❌ Error: This script is for Ubuntu/Debian systems only"
    exit 1
fi

echo "📥 Installing authentication dependencies..."
echo ""

# Update package list
sudo apt-get update

# Redis client library (hiredis)
echo "→ Installing Redis client (hiredis)..."
sudo apt-get install -y libhiredis-dev

# Argon2 password hashing library
echo "→ Installing Argon2..."
sudo apt-get install -y libargon2-dev

# YAML parser for scenarios
echo "→ Installing YAML-CPP..."
sudo apt-get install -y libyaml-cpp-dev

# PicoJSON (required by jwt-cpp)
echo "→ Installing PicoJSON..."
sudo apt-get install -y picojson-dev
# Create symlink for jwt-cpp compatibility
sudo mkdir -p /usr/include/picojson
sudo ln -sf /usr/include/picojson.h /usr/include/picojson/picojson.h 2>/dev/null || true

# JWT-CPP (header-only library from GitHub)
echo "→ Installing JWT-CPP..."
if [ ! -d "/usr/local/include/jwt-cpp" ]; then
    echo "  Cloning jwt-cpp from GitHub..."
    git clone https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp
    sudo cp -r /tmp/jwt-cpp/include/jwt-cpp /usr/local/include/
    rm -rf /tmp/jwt-cpp
    echo "  ✓ JWT-CPP installed to /usr/local/include/jwt-cpp"
else
    echo "  ✓ JWT-CPP already installed"
fi

# Redis server (optional, for local development)
echo ""
read -p "📦 Install Redis server locally? (Y/n): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]; then
    echo "→ Installing Redis server..."
    sudo apt-get install -y redis-server
    sudo systemctl enable redis-server
    sudo systemctl start redis-server
    echo "  ✓ Redis server installed and started"
    
    # Configure Redis password (development only)
    echo ""
    echo "🔒 Configuring Redis password (development)..."
    sudo sed -i 's/# requirepass foobared/requirepass DevPassword123!/' /etc/redis/redis.conf
    sudo systemctl restart redis-server
    echo "  ✓ Redis password set to: DevPassword123! (for development only)"
    echo "  ⚠️  For production, use a strong password from .env.production"
fi

echo ""
echo "=========================================="
echo "✅ Authentication Dependencies Installed!"
echo "=========================================="
echo ""
echo "📋 Installed Libraries:"
echo "-------------------"

# Check hiredis
if pkg-config --exists hiredis; then
    echo "hiredis:      $(pkg-config --modversion hiredis)"
else
    echo "hiredis:      installed ✓"
fi

# Check argon2
if dpkg -l | grep -q libargon2-dev; then
    echo "Argon2:       $(dpkg -l | grep libargon2-dev | awk '{print $3}')"
else
    echo "Argon2:       ✗ NOT FOUND"
fi

# Check jwt-cpp
if [ -f /usr/local/include/jwt-cpp/jwt.h ]; then
    echo "JWT-CPP:      installed ✓"
else
    echo "JWT-CPP:      ✗ NOT FOUND"
fi

# Check Redis server
if command -v redis-server &> /dev/null; then
    echo "Redis Server: $(redis-server --version | awk '{print $3}')"
    echo "Redis Status: $(systemctl is-active redis-server)"
else
    echo "Redis Server: not installed (use Docker instead)"
fi

echo ""
echo "🎯 Next Steps:"
echo "-------------------"
echo "1. Rebuild the project:"
echo "   cd project/backend"
echo "   cmake ."
echo "   make -j\$(nproc)"
echo ""
echo "2. Initialize auth database schema:"
echo "   mysql -u root -p netsim < project/database/AuthSchema.sql"
echo ""
echo "3. Test Redis connection:"
echo "   redis-cli -a DevPassword123! ping"
echo ""
echo "4. Or use Docker Compose:"
echo "   docker compose up -d"
echo ""
echo "⚠️  PRODUCTION: Generate secure passwords with ./scripts/generate_secrets.sh"
echo ""

