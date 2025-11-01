#!/bin/bash

# NetSimCPP Production Secrets Generator
# This script generates secure passwords and creates .env.production file

set -e

ENV_FILE=".env.production"

echo "NetSimCPP Production Secrets Generator"
echo "======================================="
echo ""

# Check if .env.production already exists
if [ -f "$ENV_FILE" ]; then
    read -p ".env.production already exists. Overwrite? (yes/no): " confirm
    if [ "$confirm" != "yes" ]; then
        echo "Aborted."
        exit 0
    fi
fi

# Generate passwords
echo "Generating secure passwords..."
DB_ROOT_PASSWORD=$(openssl rand -base64 32)
DB_PASSWORD=$(openssl rand -base64 32)
REDIS_PASSWORD=$(openssl rand -base64 32)
JWT_SECRET=$(openssl rand -hex 32)
GRAFANA_PASSWORD=$(openssl rand -base64 32)

# Get domain information
read -p "Enter your domain name (e.g., netsim.example.com): " DOMAIN_NAME
read -p "Enter your email for Let's Encrypt: " LETSENCRYPT_EMAIL

# Create .env.production file
cat > "$ENV_FILE" << EOF
# NetSimCPP Production Environment Variables
# Generated: $(date)
# WARNING: Keep this file secure and never commit to version control!

# ============================================
# DATABASE CONFIGURATION
# ============================================
DB_ROOT_PASSWORD=${DB_ROOT_PASSWORD}
DB_USER=netsimuser
DB_PASSWORD=${DB_PASSWORD}

# ============================================
# REDIS CONFIGURATION
# ============================================
REDIS_PASSWORD=${REDIS_PASSWORD}

# ============================================
# JWT CONFIGURATION
# ============================================
JWT_SECRET=${JWT_SECRET}
JWT_EXPIRATION_HOURS=24

# ============================================
# GRAFANA CONFIGURATION
# ============================================
GRAFANA_USER=admin
GRAFANA_PASSWORD=${GRAFANA_PASSWORD}

# ============================================
# SSL/TLS CONFIGURATION
# ============================================
DOMAIN_NAME=${DOMAIN_NAME}
LETSENCRYPT_EMAIL=${LETSENCRYPT_EMAIL}
EOF

# Set secure permissions
chmod 600 "$ENV_FILE"

echo ""
echo "✅ Production secrets generated successfully!"
echo ""
echo "📋 Generated credentials:"
echo "   Database Root Password: ${DB_ROOT_PASSWORD}"
echo "   Database User Password: ${DB_PASSWORD}"
echo "   Redis Password: ${REDIS_PASSWORD}"
echo "   JWT Secret: ${JWT_SECRET}"
echo "   Grafana Password: ${GRAFANA_PASSWORD}"
echo ""
echo "⚠️  IMPORTANT:"
echo "   1. Save these credentials in a secure password manager"
echo "   2. File saved to: ${ENV_FILE} (mode 600)"
echo "   3. Never commit this file to version control"
echo "   4. Backup this file securely"
echo ""
echo "🔐 Add to .gitignore:"
echo "   echo '.env.production' >> .gitignore"
echo ""
