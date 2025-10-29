# Authentication & Authorization - NetSimCPP

## 📋 Overview

NetSimCPP implements a complete authentication and authorization system with:

- ✅ **JWT Token-based Authentication** (stateless)
- ✅ **Password Hashing with Argon2** (PHC winner 2015)
- ✅ **Redis Session Storage** (fast, persistent sessions)
- ✅ **Role-Based Access Control** (admin, user, viewer)
- ✅ **Rate Limiting per User** (MySQL-based)
- ✅ **Audit Logging** (all auth events tracked)

---

## 🏗️ Architecture

```
┌─────────────┐     ┌──────────────┐     ┌───────────┐
│   Client    │────▶│  REST API    │────▶│   MySQL   │
│ (Browser/   │     │ (main.cpp)   │     │  (users,  │
│  App)       │     │              │     │  perms)   │
└─────────────┘     └──────────────┘     └───────────┘
       │                    │                     
       │              ┌─────▼──────┐              
       │              │   Redis    │              
       │              │ (sessions) │              
       └──────────────┴────────────┘              
```

### Components:

1. **PasswordHasher** - Argon2id password hashing
2. **JWTManager** - Token generation and validation
3. **RedisClient** - Session storage and management
4. **AuthMiddleware** - Request authentication (coming next)
5. **RateLimiter** - API rate limiting per user

---

## 🔐 Security Features

### Password Hashing (Argon2id)

```cpp
// Hash password
std::string hash = PasswordHasher::hashPassword("myPassword123");
// Output: $argon2id$v=19$m=65536,t=3,p=4$...

// Verify password
bool valid = PasswordHasher::verifyPassword("myPassword123", hash);
```

**Parameters:**
- **Memory Cost**: 64 MiB
- **Time Cost**: 3 iterations
- **Parallelism**: 4 threads
- **Salt Length**: 16 bytes (random)
- **Hash Length**: 32 bytes

### JWT Tokens

```cpp
JWTManager jwt(secret);

// Generate token
std::string token = jwt.generateToken(
    user_id,    // 1
    username,   // "admin"
    role,       // "admin"
    24          // 24 hours expiry
);

// Verify token
auto payload = jwt.verifyToken(token);
// payload = {"user_id": 1, "username": "admin", "role": "admin"}
```

**Token Structure:**
```json
{
  "iss": "netsim-cpp",
  "iat": 1640000000,
  "exp": 1640086400,
  "user_id": "1",
  "username": "admin",
  "role": "admin"
}
```

### Redis Sessions

```cpp
RedisClient redis("localhost", 6379, "NetSimCPP1234");

// Store session
redis.storeSession(user_id, token, 86400); // 24h TTL

// Get session
std::string token = redis.getSession(user_id);

// Delete session (logout)
redis.deleteSession(user_id);
```

---

## 🚀 API Endpoints

### POST /auth/register
Register a new user.

**Request:**
```json
{
  "username": "newuser",
  "email": "user@example.com",
  "password": "securePassword123",
  "role": "user"
}
```

**Response:**
```json
{
  "success": true,
  "user_id": 42,
  "username": "newuser",
  "role": "user"
}
```

### POST /auth/login
Login and receive JWT token.

**Request:**
```json
{
  "username": "admin",
  "password": "admin123"
}
```

**Response:**
```json
{
  "success": true,
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "user": {
    "id": 1,
    "username": "admin",
    "role": "admin"
  },
  "expires_in": 86400
}
```

### POST /auth/logout
Logout and invalidate token.

**Headers:**
```
Authorization: Bearer <token>
```

**Response:**
```json
{
  "success": true,
  "message": "Logged out successfully"
}
```

### GET /auth/me
Get current user information.

**Headers:**
```
Authorization: Bearer <token>
```

**Response:**
```json
{
  "id": 1,
  "username": "admin",
  "email": "admin@netsim.local",
  "role": "admin",
  "permissions": ["node:read", "node:write", "link:read", "link:write"]
}
```

---

## 👥 User Roles & Permissions

### Admin
Full access to all resources.

**Permissions:**
- ✅ node:read, node:write, node:delete
- ✅ link:read, link:write, link:delete
- ✅ topology:read, topology:write
- ✅ User management

### User
Standard user with read/write access.

**Permissions:**
- ✅ node:read, node:write
- ✅ link:read, link:write
- ✅ topology:read
- ❌ Cannot delete resources
- ❌ No user management

### Viewer
Read-only access.

**Permissions:**
- ✅ node:read
- ✅ link:read
- ✅ topology:read
- ❌ No write/delete access

---

## ⏱️ Rate Limiting

Rate limiting is enforced per user per endpoint.

**Default Limits:**
- `/node/add`: 100 requests / hour
- `/link/connect`: 200 requests / hour
- `/auth/login`: 5 requests / minute
- Default: 1000 requests / hour

**Response when rate limit exceeded:**
```json
{
  "error": "Rate limit exceeded",
  "retry_after": 3600
}
```

**HTTP Status:** 429 Too Many Requests

---

## 📊 Database Schema

### users
```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role ENUM('admin', 'user', 'viewer') DEFAULT 'user',
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP NULL
);
```

### api_rate_limits
```sql
CREATE TABLE api_rate_limits (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    endpoint VARCHAR(100) NOT NULL,
    request_count INT DEFAULT 0,
    window_start TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);
```

### auth_audit_log
```sql
CREATE TABLE auth_audit_log (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NULL,
    event_type ENUM('login', 'logout', 'failed_login', ...),
    ip_address VARCHAR(45),
    success BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

---

## 🧪 Testing

### Register a User
```bash
curl -X POST http://localhost:8080/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "username": "testuser",
    "email": "test@example.com",
    "password": "Test123!",
    "role": "user"
  }'
```

### Login
```bash
curl -X POST http://localhost:8080/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "username": "admin",
    "password": "admin123"
  }'
```

### Access Protected Endpoint
```bash
TOKEN="eyJhbGciOiJIUzI1..."

curl http://localhost:8080/auth/me \
  -H "Authorization: Bearer $TOKEN"
```

---

## 🔧 Configuration

### Environment Variables

```bash
# JWT Secret (CHANGE IN PRODUCTION!)
JWT_SECRET=your-secret-key-change-in-production

# Redis
REDIS_HOST=localhost
REDIS_PORT=6379
REDIS_PASS=NetSimCPP1234

# Database
DB_HOST=localhost
DB_USER=root
DB_PASS=NetSimCPP1234
DB_NAME=netsim
```

### Docker Compose

Authentication is fully integrated with Docker:

```bash
docker compose up -d
```

This starts:
- MySQL (users database)
- Redis (session storage)
- NetSimCPP server (with auth enabled)

---

## 📖 Usage Example

```cpp
#include "auth/PasswordHasher.hpp"
#include "auth/JWTManager.hpp"
#include "auth/RedisClient.hpp"

// 1. User Registration
std::string password_hash = PasswordHasher::hashPassword(password);
// Store user in database with hashed password

// 2. User Login
bool valid = PasswordHasher::verifyPassword(password, stored_hash);
if (valid) {
    JWTManager jwt(secret);
    std::string token = jwt.generateToken(user_id, username, role);
    
    RedisClient redis("localhost", 6379, "password");
    redis.storeSession(user_id, token, 86400);
    
    // Return token to client
}

// 3. Verify Request
std::string token = extract_from_header(request);
auto payload = jwt.verifyToken(token);
int user_id = payload["user_id"];

// Check if session exists in Redis
if (redis.sessionExists(user_id)) {
    // Authorized!
}
```

---

## 🛡️ Security Best Practices

1. **HTTPS Only** - Use TLS in production
2. **Strong Passwords** - Enforce password complexity
3. **Token Rotation** - Implement refresh tokens
4. **IP Whitelisting** - Optional for admin accounts
5. **Audit Logging** - Track all auth events
6. **Rate Limiting** - Prevent brute force attacks
7. **Secure Secrets** - Use environment variables, never hardcode

---

## 📚 Dependencies

- **libhiredis-dev** - Redis C client
- **libargon2-dev** - Argon2 password hashing
- **jwt-cpp** - Header-only JWT library
- **OpenSSL** - Crypto functions

Install with:
```bash
./scripts/install_auth_deps.sh
```

---

## 🎯 Roadmap

- [x] Basic JWT authentication
- [x] Password hashing with Argon2
- [x] Redis session storage
- [x] Role-based access control
- [x] Rate limiting
- [ ] Refresh tokens
- [ ] OAuth2 integration
- [ ] Two-factor authentication (2FA)
- [ ] IP whitelisting
- [ ] Session management UI

---

## 📝 License

MIT License - see main README.md

