# Quick Testing Guide

## Prerequisites

1. **Dependencies installed**:
   - ✓ libhiredis-dev
   - ✓ libargon2-dev  
   - ✓ jwt-cpp
   - ✓ MySQL running (Docker or local)
   - ✓ Redis running (local on port 6379)

2. **Database initialized**:
   ```bash
   # If using Docker MySQL:
   docker exec -i netsim-mysql mysql -uroot -pNetSimCPP1234 netsim < project/database/AuthSchema.sql
   
   # If using local MySQL:
   mysql -u root -p netsim < project/database/AuthSchema.sql
   ```

## Build & Run

### Step 1: Build the project
```bash
cd project/backend/build
cmake ..
make -j$(nproc)
```

### Step 2: Run the server
```bash
# Option A: Use the startup script (recommended)
./run_server.sh

# Option B: Manual startup with env vars
export JWT_SECRET="netsim-secret-key-change-in-production-12345"
export DB_HOST="localhost"
export DB_USER="root"
export DB_PASS="NetSimCPP1234"
export DB_NAME="netsim"
export REDIS_HOST="localhost"
export REDIS_PORT="6379"
export REDIS_PASS=""
./NetSimCPP
```

### Step 3: Run tests (in another terminal)
```bash
cd /home/patryk/inzynieria_oprogramowania
./test_auth.sh
```

## Manual Testing

### 1. Register a new user
```bash
curl -X POST http://localhost:8080/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "username": "myuser",
    "email": "user@example.com",
    "password": "SecurePass123",
    "role": "user"
  }'
```

### 2. Login
```bash
TOKEN=$(curl -s -X POST http://localhost:8080/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' \
  | jq -r '.token')

echo "Token: $TOKEN"
```

### 3. Get current user info
```bash
curl -X GET http://localhost:8080/auth/me \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

### 4. Use protected endpoint
```bash
curl -X POST http://localhost:8080/node/add \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "name": "router1",
    "ip": "192.168.1.1",
    "type": "router"
  }' | jq '.'
```

### 5. Test without auth (should fail with 401)
```bash
curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{
    "name": "unauthorized_node",
    "ip": "192.168.1.99",
    "type": "host"
  }'
```

### 6. Logout
```bash
curl -X POST http://localhost:8080/auth/logout \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

## Default Users

| Username | Password | Role | Description |
|----------|----------|------|-------------|
| admin | admin123 | admin | Full access |
| user1 | user123 | user | Standard operations |
| viewer1 | viewer123 | viewer | Read-only |

## Troubleshooting

### MySQL not ready
```bash
# Check MySQL status
docker ps --filter name=mysql

# Check MySQL logs
docker logs netsim-mysql

# Wait for MySQL to be ready
docker exec netsim-mysql mysqladmin ping -h localhost -pNetSimCPP1234
```

### Redis not running
```bash
# Check Redis
redis-cli ping

# Start Redis if needed
sudo systemctl start redis
```

### Compilation errors
```bash
# Check if libraries are installed
dpkg -l | grep -E 'libhiredis|libargon2'
ls -la /usr/local/include/jwt-cpp/

# Reinstall if needed
sudo ./scripts/install_auth_deps.sh
```

### Authentication not working
```bash
# Check environment variables
env | grep -E 'JWT_SECRET|DB_|REDIS_'

# Check server logs for initialization messages
# Look for: "[Main] Authentication service initialized"
```

## Expected Output

When server starts successfully, you should see:
```
[Main] Authentication service initialized
[Main] WebSocket server started on port 9001
Listening for requests at: http://0.0.0.0:8080/
```

When you run tests, you should see all green checkmarks (✓).

## Next Steps

After successful testing:
1. Review `docs/AUTHENTICATION.md` for full API documentation
2. Review `docs/API_SECURITY.md` for security details
3. Change default passwords in production!
4. Configure proper JWT_SECRET in production
5. Enable HTTPS in production environment
