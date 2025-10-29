# API Security & Rate Limiting

## Overview

This document describes the authentication, authorization, and rate limiting implemented in the NetSimCPP REST API.

## Authentication

All protected endpoints require a valid JWT token in the `Authorization` header:

```
Authorization: Bearer <your-jwt-token>
```

### Getting a Token

1. **Register** a new user:
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

2. **Login** to get a token:
```bash
curl -X POST http://localhost:8080/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "username": "myuser",
    "password": "SecurePass123"
  }'
```

Response:
```json
{
  "success": true,
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "user": {
    "id": 2,
    "username": "myuser",
    "email": "user@example.com",
    "role": "user"
  },
  "expires_in": 86400
}
```

## Protected Endpoints

### Node Management

| Endpoint | Method | Required Permission | Rate Limit |
|----------|--------|-------------------|------------|
| `/node/add` | POST | `nodes:create` | 100/min |
| `/node/remove` | POST | `nodes:delete` | 50/min |
| `/node/config` | POST | `nodes:update` | 200/min |
| `/node/fail` | POST | `nodes:update` | - |

### Link Management

| Endpoint | Method | Required Permission | Rate Limit |
|----------|--------|-------------------|------------|
| `/link/connect` | POST | `links:create` | 100/min |
| `/link/disconnect` | POST | `links:delete` | 100/min |
| `/link/delay` | POST | `links:update` | 200/min |
| `/link/bandwidth` | POST | `links:update` | 200/min |
| `/link/packetloss` | POST | `links:update` | 200/min |

### Network Configuration

| Endpoint | Method | Required Permission | Rate Limit |
|----------|--------|-------------------|------------|
| `/vlan/assign` | POST | `network:update` | 100/min |
| `/topology/import` | POST | `topology:create` | 10/min |

### Security (Firewall)

| Endpoint | Method | Required Permission | Rate Limit |
|----------|--------|-------------------|------------|
| `/firewall/rule` | POST | `firewall:create` | 50/min |

### Simulation (Read-only endpoints - no auth required)

| Endpoint | Method | Auth Required | Rate Limit |
|----------|--------|--------------|------------|
| `/ping` | POST | No | - |
| `/traceroute` | POST | No | - |
| `/multicast` | POST | No | - |
| `/tcp/connect` | POST | No | - |

## User Roles & Permissions

### Admin Role

Full access to all endpoints:
- `nodes:*` (create, read, update, delete)
- `links:*` (create, read, update, delete)
- `network:*` (create, read, update, delete)
- `topology:*` (create, read, update, delete)
- `firewall:*` (create, read, update, delete)

### User Role

Standard network operations:
- `nodes:*` (create, read, update, delete)
- `links:*` (create, read, update, delete)
- `network:update` (configure VLANs, etc.)

### Viewer Role

Read-only access:
- `nodes:read`
- `links:read`
- `network:read`

## Rate Limiting

Rate limits are enforced per user per endpoint. When exceeded, the API returns:

```json
{
  "error": "Rate limit exceeded. Please try again later."
}
```

HTTP Status: `429 Too Many Requests`

### Rate Limit Tiers

1. **Critical Operations** (10 requests/min):
   - Topology import

2. **Security Operations** (50 requests/min):
   - Firewall rules
   - Node removal

3. **Standard Operations** (100 requests/min):
   - Node creation
   - Link management

4. **Configuration Changes** (200 requests/min):
   - Node configuration
   - Link parameters (delay, bandwidth, packet loss)

## Error Responses

### 401 Unauthorized
Missing or invalid authentication token:
```json
{
  "error": "Missing authorization token"
}
```

### 403 Forbidden
Valid token but insufficient permissions:
```json
{
  "error": "Insufficient permissions"
}
```

### 429 Too Many Requests
Rate limit exceeded:
```json
{
  "error": "Rate limit exceeded. Please try again later."
}
```

### 503 Service Unavailable
Authentication service not available:
```json
{
  "error": "Authentication service not available"
}
```

## Usage Examples

### Example 1: Add a Node (Authenticated)

```bash
# Get token first
TOKEN=$(curl -s -X POST http://localhost:8080/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' \
  | jq -r '.token')

# Add a node with authentication
curl -X POST http://localhost:8080/node/add \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "name": "router1",
    "ip": "192.168.1.1",
    "type": "router"
  }'
```

### Example 2: Configure Link (Authenticated)

```bash
# Set link delay (requires links:update permission)
curl -X POST http://localhost:8080/link/delay \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "nodeA": "router1",
    "nodeB": "host1",
    "delay": 50
  }'
```

### Example 3: Add Firewall Rule (Admin Only)

```bash
# Admin login
ADMIN_TOKEN=$(curl -s -X POST http://localhost:8080/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' \
  | jq -r '.token')

# Add firewall rule
curl -X POST http://localhost:8080/firewall/rule \
  -H "Authorization: Bearer $ADMIN_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "src": "192.168.1.0/24",
    "dst": "10.0.0.0/8",
    "protocol": "tcp",
    "allow": false
  }'
```

### Example 4: Check Current User Info

```bash
curl -X GET http://localhost:8080/auth/me \
  -H "Authorization: Bearer $TOKEN"
```

Response:
```json
{
  "id": 1,
  "username": "admin",
  "email": "admin@netsim.local",
  "role": "admin",
  "permissions": [
    {"resource": "nodes", "action": "create"},
    {"resource": "nodes", "action": "read"},
    {"resource": "nodes", "action": "update"},
    {"resource": "nodes", "action": "delete"},
    {"resource": "links", "action": "create"},
    ...
  ]
}
```

## Security Best Practices

1. **Never share tokens**: Each user should have their own credentials
2. **Use HTTPS in production**: JWT tokens should be transmitted over encrypted connections
3. **Rotate secrets regularly**: Change `JWT_SECRET` periodically
4. **Monitor rate limits**: Track 429 errors to detect abuse
5. **Audit logs**: All authentication events are logged in `auth_audit_log` table
6. **Strong passwords**: Enforce password complexity in production
7. **Token expiry**: Tokens expire after 24 hours by default

## Monitoring

### Check Authentication Logs

```sql
-- Recent login attempts
SELECT user_id, event_type, ip_address, success, created_at, message
FROM auth_audit_log
WHERE event_type = 'login'
ORDER BY created_at DESC
LIMIT 20;

-- Failed login attempts
SELECT user_id, ip_address, created_at, message
FROM auth_audit_log
WHERE event_type = 'login' AND success = FALSE
ORDER BY created_at DESC;
```

### Check Rate Limits

```sql
-- Current rate limit usage
SELECT u.username, arl.endpoint, arl.request_count, arl.window_start
FROM api_rate_limits arl
JOIN users u ON arl.user_id = u.id
WHERE arl.window_start > NOW() - INTERVAL 1 MINUTE
ORDER BY arl.request_count DESC;
```

## Disabling Authentication

For development or testing, you can run the server without authentication by not setting the environment variables. The server will log a warning and run without auth:

```
[Main] Warning: Failed to initialize auth service: ...
[Main] Server will run without authentication
```

**Note**: This is NOT recommended for production environments.

## Configuration

See [AUTHENTICATION.md](AUTHENTICATION.md) for detailed configuration options and setup instructions.
