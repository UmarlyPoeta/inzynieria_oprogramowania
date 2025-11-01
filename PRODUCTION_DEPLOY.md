# NetSimCPP Production Deployment Guide

## 📋 Overview

This guide walks you through deploying NetSimCPP to production with:
- ✅ YAML scenario parser support
- ✅ Nginx reverse proxy with HTTPS/SSL
- ✅ Full monitoring stack (Prometheus + Grafana)
- ✅ Secure password management
- ✅ Auto-renewing SSL certificates
- ✅ Rate limiting and security headers

## 🔧 Prerequisites

### Required Software
```bash
# Docker & Docker Compose
docker --version  # >= 20.10
docker-compose --version  # >= 1.29

# OpenSSL (for password generation)
openssl version  # >= 1.1.1
```

### Server Requirements
- **OS**: Ubuntu 22.04 LTS or Fedora 38+ (recommended)
- **RAM**: 4GB minimum, 8GB recommended
- **CPU**: 2 cores minimum, 4 cores recommended
- **Disk**: 20GB minimum
- **Network**: Public IP with domain pointing to it

### Domain Setup
- Register a domain name (e.g., `netsim.example.com`)
- Point DNS A record to your server's public IP
- Wait for DNS propagation (check with `dig netsim.example.com`)

## 🚀 Step-by-Step Deployment

### Step 1: Clone Repository & Prepare Environment

```bash
# Clone repository
cd /opt
git clone https://github.com/your-org/netsim.git
cd netsim

# Verify all files are present
ls -la docker-compose.prod.yml
ls -la docker-compose.monitoring.yml
ls -la nginx/nginx.conf
ls -la monitoring/prometheus.yml
```

### Step 2: Generate Secure Passwords

```bash
# Run the password generator script
./scripts/generate_secrets.sh

# This will create .env.production with:
# - DB_ROOT_PASSWORD (MySQL root password)
# - DB_PASSWORD (Application database password)
# - REDIS_PASSWORD (Redis authentication)
# - JWT_SECRET (64-char hex for JWT signing)
# - GRAFANA_PASSWORD (Grafana admin password)

# IMPORTANT: Save these passwords in your password manager!
# The script will display them once.
```

**Manual Alternative** (if script fails):
```bash
# Copy template
cp .env.production.template .env.production

# Generate passwords manually
DB_ROOT_PASSWORD=$(openssl rand -base64 32)
DB_PASSWORD=$(openssl rand -base64 32)
REDIS_PASSWORD=$(openssl rand -base64 32)
JWT_SECRET=$(openssl rand -hex 32)
GRAFANA_PASSWORD=$(openssl rand -base64 32)

# Edit .env.production and replace placeholders
nano .env.production

# Set secure permissions
chmod 600 .env.production

# Add to .gitignore
echo '.env.production' >> .gitignore
```

### Step 3: Configure Domain & SSL

```bash
# Edit nginx configuration with your domain
nano nginx/nginx.conf

# Replace this line (around line 30):
# ssl_certificate /etc/letsencrypt/live/your-domain.com/fullchain.pem;
# With:
ssl_certificate /etc/letsencrypt/live/netsim.example.com/fullchain.pem;

# And this line (around line 31):
# ssl_certificate_key /etc/letsencrypt/live/your-domain.com/privkey.pem;
# With:
ssl_certificate_key /etc/letsencrypt/live/netsim.example.com/privkey.pem;
```

### Step 4: Build & Start Production Stack

```bash
# Create Docker network
docker network create netsim-network

# Build and start main services
docker-compose -f docker-compose.prod.yml up -d --build

# Wait for services to be healthy (30-60 seconds)
docker-compose -f docker-compose.prod.yml ps

# Check logs
docker-compose -f docker-compose.prod.yml logs -f app
```

Expected output:
```
✅ netsim-db-prod      - healthy
✅ netsim-redis-prod   - healthy
✅ netsim-app-prod     - healthy
✅ netsim-nginx-prod   - running
✅ netsim-certbot      - running
```

### Step 5: Obtain SSL Certificate

```bash
# Stop nginx temporarily
docker-compose -f docker-compose.prod.yml stop nginx

# Get certificate from Let's Encrypt
docker run -it --rm \
  -v $(pwd)/certbot_data:/etc/letsencrypt \
  -v $(pwd)/certbot_www:/var/www/certbot \
  -p 80:80 \
  certbot/certbot certonly \
  --standalone \
  --preferred-challenges http \
  --email your-email@example.com \
  --agree-tos \
  --no-eff-email \
  -d netsim.example.com

# Restart nginx
docker-compose -f docker-compose.prod.yml start nginx
```

**Alternative** (if standalone fails - use webroot):
```bash
# Start nginx first
docker-compose -f docker-compose.prod.yml start nginx

# Use webroot validation
docker-compose -f docker-compose.prod.yml exec certbot \
  certbot certonly \
  --webroot \
  --webroot-path /var/www/certbot \
  --email your-email@example.com \
  --agree-tos \
  --no-eff-email \
  -d netsim.example.com
```

### Step 6: Start Monitoring Stack

```bash
# Start Prometheus + Grafana + Exporters
docker-compose -f docker-compose.monitoring.yml up -d

# Verify monitoring services
docker-compose -f docker-compose.monitoring.yml ps
```

Expected services:
```
✅ netsim-prometheus      - http://your-ip:9090
✅ netsim-grafana         - http://your-ip:3000
✅ netsim-node-exporter   - http://your-ip:9100
✅ netsim-mysql-exporter  - http://your-ip:9104
✅ netsim-redis-exporter  - http://your-ip:9121
✅ netsim-cadvisor        - http://your-ip:8081
```

### Step 7: Access & Verify

#### Application API
```bash
# Test HTTPS endpoint
curl https://netsim.example.com/api/health

# Test authentication
curl -X POST https://netsim.example.com/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "username": "admin",
    "email": "admin@example.com",
    "password": "YourSecurePassword123!",
    "full_name": "Administrator"
  }'

# Login
curl -X POST https://netsim.example.com/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "username": "admin",
    "password": "YourSecurePassword123!"
  }'
```

#### Grafana Dashboard
```bash
# Open browser to:
http://your-server-ip:3000

# Login with:
# Username: admin
# Password: (from .env.production GRAFANA_PASSWORD)

# Add dashboards:
# 1. Go to Dashboards > Import
# 2. Use dashboard ID: 1860 (Node Exporter Full)
# 3. Use dashboard ID: 7362 (MySQL Overview)
# 4. Use dashboard ID: 11835 (Redis Dashboard)
```

#### Prometheus Metrics
```bash
# Open browser to:
http://your-server-ip:9090

# Query examples:
# - up{job="netsim-app"}
# - mysql_global_status_connections
# - redis_connected_clients
# - container_memory_usage_bytes
```

### Step 8: Test YAML Scenario Parser

```bash
# Create test scenario file
cat > test_scenario.yaml << 'EOF'
name: "Basic Ping Test"
description: "Test YAML parser with simple ping scenario"
version: "1.0"
author: "NetSimCPP"
tags: ["test", "yaml", "ping"]

setup:
  nodes:
    - name: "host1"
      type: "host"
      ip: "192.168.1.10"
    - name: "host2"
      type: "host"
      ip: "192.168.1.20"
  
  links:
    - from: "host1"
      to: "host2"
      delay_ms: 10
      bandwidth_mbps: 100

steps:
  - name: "Ping test"
    action: "ping"
    params:
      from: "host1"
      to: "host2"
    expect:
      success: true

validation:
  - type: "connectivity"
    params:
      from: "host1"
      to: "host2"

expected_outcome: "Successful ping between hosts"
EOF

# Convert YAML to JSON and run scenario
curl -X POST https://netsim.example.com/api/scenario/run \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_JWT_TOKEN" \
  --data-binary @test_scenario.yaml
```

## 📊 Monitoring & Maintenance

### Daily Checks
```bash
# Check service health
docker-compose -f docker-compose.prod.yml ps
docker-compose -f docker-compose.monitoring.yml ps

# Check logs for errors
docker-compose -f docker-compose.prod.yml logs --tail=100 app
docker-compose -f docker-compose.prod.yml logs --tail=100 nginx

# Check disk space
df -h
docker system df
```

### Weekly Maintenance
```bash
# Update Docker images
docker-compose -f docker-compose.prod.yml pull
docker-compose -f docker-compose.monitoring.yml pull

# Restart services
docker-compose -f docker-compose.prod.yml up -d
docker-compose -f docker-compose.monitoring.yml up -d

# Clean up unused resources
docker system prune -a --volumes --filter "until=168h"
```

### Certificate Renewal
Certbot auto-renews certificates every 12 hours (via docker-compose.prod.yml).

Manual renewal:
```bash
docker-compose -f docker-compose.prod.yml exec certbot certbot renew
docker-compose -f docker-compose.prod.yml restart nginx
```

### Database Backups
```bash
# Create backup script
cat > /opt/netsim/scripts/backup_db.sh << 'EOF'
#!/bin/bash
BACKUP_DIR="/opt/netsim/backups"
DATE=$(date +%Y%m%d_%H%M%S)
mkdir -p $BACKUP_DIR

docker exec netsim-db-prod mysqldump \
  -u root \
  -p$DB_ROOT_PASSWORD \
  netsimdb > $BACKUP_DIR/netsimdb_$DATE.sql

# Keep only last 7 days
find $BACKUP_DIR -name "netsimdb_*.sql" -mtime +7 -delete
EOF

chmod +x /opt/netsim/scripts/backup_db.sh

# Add to crontab (daily at 2 AM)
(crontab -l 2>/dev/null; echo "0 2 * * * /opt/netsim/scripts/backup_db.sh") | crontab -
```

## 🔒 Security Checklist

- ✅ Changed all default passwords (via generate_secrets.sh)
- ✅ HTTPS/TLS enabled with Let's Encrypt
- ✅ JWT secret is 64+ characters
- ✅ Rate limiting enabled (10 req/s API, 5 req/s auth)
- ✅ Security headers configured (HSTS, X-Frame-Options, etc.)
- ✅ Database passwords are strong (32+ characters)
- ✅ Redis requires authentication
- ✅ Grafana admin password changed
- ✅ .env.production is chmod 600
- ✅ .env.production is in .gitignore
- ✅ Firewall configured (only ports 80, 443, 3000, 9090 open)

### Firewall Configuration
```bash
# UFW (Ubuntu)
ufw allow 80/tcp    # HTTP (redirects to HTTPS)
ufw allow 443/tcp   # HTTPS
ufw allow 3000/tcp  # Grafana (restrict to VPN if possible)
ufw allow 9090/tcp  # Prometheus (restrict to VPN if possible)
ufw enable

# Firewalld (Fedora)
firewall-cmd --permanent --add-service=http
firewall-cmd --permanent --add-service=https
firewall-cmd --permanent --add-port=3000/tcp
firewall-cmd --permanent --add-port=9090/tcp
firewall-cmd --reload
```

## 🐛 Troubleshooting

### Issue: SSL certificate not found
```bash
# Check certificate exists
ls -la certbot_data/live/your-domain.com/

# If missing, re-run Step 5
docker-compose -f docker-compose.prod.yml stop nginx
# ... follow Step 5 commands ...
```

### Issue: Database connection failed
```bash
# Check database is healthy
docker-compose -f docker-compose.prod.yml exec db mysql -u root -p

# Reset database password
docker-compose -f docker-compose.prod.yml exec db \
  mysql -u root -p -e "ALTER USER 'netsimuser'@'%' IDENTIFIED BY 'new_password';"

# Update .env.production with new password
nano .env.production

# Restart app
docker-compose -f docker-compose.prod.yml restart app
```

### Issue: High memory usage
```bash
# Check container stats
docker stats

# Restart services
docker-compose -f docker-compose.prod.yml restart

# Clear Redis cache
docker-compose -f docker-compose.prod.yml exec redis redis-cli -a $REDIS_PASSWORD FLUSHALL
```

### Issue: YAML parsing error
```bash
# Check yaml-cpp is installed
docker-compose -f docker-compose.prod.yml exec app ldd /app/netsim | grep yaml

# Rebuild with YAML support
docker-compose -f docker-compose.prod.yml up -d --build app

# Test with JSON first
curl -X POST https://netsim.example.com/api/scenario/run \
  -H "Content-Type: application/json" \
  -d @test_scenario.json
```

## 📈 Performance Tuning

### Optimize MySQL
```bash
# Add to docker-compose.prod.yml under db.environment:
MYSQL_INNODB_BUFFER_POOL_SIZE: "1G"
MYSQL_MAX_CONNECTIONS: "200"

# Restart database
docker-compose -f docker-compose.prod.yml restart db
```

### Optimize Redis
```bash
# Add to docker-compose.prod.yml under redis.command:
redis-server --requirepass ${REDIS_PASSWORD} --maxmemory 512mb --maxmemory-policy allkeys-lru

# Restart Redis
docker-compose -f docker-compose.prod.yml restart redis
```

### Optimize Nginx
```bash
# Edit nginx/nginx.conf, add to http block:
worker_processes auto;
worker_rlimit_nofile 65535;

events {
    worker_connections 4096;
    use epoll;
    multi_accept on;
}

# Restart nginx
docker-compose -f docker-compose.prod.yml restart nginx
```

## 📞 Support & Resources

- **Documentation**: `/docs` directory
- **API Reference**: `docs/api.md`
- **Architecture**: `docs/architecture.md`
- **WebSocket API**: `docs/WEBSOCKET_API.md`
- **Issues**: GitHub Issues
- **CI/CD**: `.github/workflows`

## ✅ Deployment Checklist

Print this and check off each step:

```
Pre-Deployment:
[ ] Domain DNS configured
[ ] Server meets requirements (4GB RAM, 2 CPU, 20GB disk)
[ ] Docker & Docker Compose installed
[ ] Repository cloned to /opt/netsim

Deployment:
[ ] Generated secure passwords (./scripts/generate_secrets.sh)
[ ] Saved passwords in password manager
[ ] Configured domain in nginx.conf
[ ] Created Docker network (docker network create netsim-network)
[ ] Started production stack (docker-compose.prod.yml up -d)
[ ] Obtained SSL certificate (certbot)
[ ] Started monitoring stack (docker-compose.monitoring.yml up -d)
[ ] Tested HTTPS endpoint (curl https://your-domain.com/health)
[ ] Created admin user (POST /api/auth/register)
[ ] Tested YAML parser (scenario/run)
[ ] Accessed Grafana (http://ip:3000)
[ ] Configured firewall (ufw/firewalld)

Post-Deployment:
[ ] Set up database backups (crontab)
[ ] Documented passwords securely
[ ] Tested failover scenarios
[ ] Configured alerting (optional)
[ ] Load testing completed (optional)
```

---

**Congratulations! 🎉** Your NetSimCPP production environment is ready.

For questions or issues, check the troubleshooting section or open a GitHub issue.
