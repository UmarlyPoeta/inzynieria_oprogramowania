# NetSimCPP REST API Documentation

## Overview

NetSimCPP provides a comprehensive REST API for controlling and monitoring network simulations. The server runs on `http://0.0.0.0:8080` by default.

## Base URL

```
http://localhost:8080
```

## Response Format

All responses are in JSON format. Successful operations return:
```json
{
  "result": "operation successful",
  ...additional fields
}
```

Error responses return:
```json
{
  "error": "error message"
}
```

---

## Node Management

### Add Node
**POST** `/node/add`

Add a new node to the network (Host or Router).

**Request Body:**
```json
{
  "name": "A",
  "ip": "10.0.0.1",
  "type": "host",
  "port": 8080
}
```

**Parameters:**
- `name` (string, required): Node name/identifier
- `ip` (string, optional): IP address (default: "127.0.0.1")
- `type` (string, optional): Node type - "host" or "router" (default: "host")
- `port` (integer, optional): Port number for hosts (default: 8080)

**Example:**
```bash
# Add a host
curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"HostA","ip":"10.0.0.1","type":"host","port":8080}'

# Add a router
curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"Router1","ip":"10.0.0.254","type":"router"}'
```

**Response:**
```json
{
  "result": "node added",
  "name": "HostA",
  "type": "host"
}
```

---

### Remove Node
**POST** `/node/remove`

Remove a node from the network.

**Request Body:**
```json
{
  "name": "A"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/node/remove \
  -H "Content-Type: application/json" \
  -d '{"name":"HostA"}'
```

---

### Fail Node
**POST** `/node/fail`

Simulate a node failure.

**Request Body:**
```json
{
  "name": "A"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/node/fail \
  -H "Content-Type: application/json" \
  -d '{"name":"Router1"}'
```

---

### List All Nodes
**GET** `/nodes`

Get a list of all nodes in the network.

**Example:**
```bash
curl http://localhost:8080/nodes
```

**Response:**
```json
{
  "nodes": ["HostA", "HostB", "Router1"],
  "count": 3
}
```

---

## Network Topology

### Connect Nodes
**POST** `/link/connect`

Create a link between two nodes.

**Request Body:**
```json
{
  "nodeA": "HostA",
  "nodeB": "Router1"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/link/connect \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"HostA","nodeB":"Router1"}'
```

---

### Disconnect Nodes
**POST** `/link/disconnect`

Remove the link between two nodes.

**Request Body:**
```json
{
  "nodeA": "HostA",
  "nodeB": "Router1"
}
```

---

### Set Link Delay
**POST** `/link/delay`

Set propagation delay on a link (in milliseconds).

**Request Body:**
```json
{
  "nodeA": "HostA",
  "nodeB": "Router1",
  "delay": 10
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/link/delay \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"HostA","nodeB":"Router1","delay":10}'
```

---

### Set Link Bandwidth
**POST** `/link/bandwidth`

Set bandwidth capacity for a link.

**Request Body:**
```json
{
  "nodeA": "HostA",
  "nodeB": "Router1",
  "bandwidth": 1000
}
```

---

### Set Packet Loss
**POST** `/link/packetloss`

Set packet loss probability for a link.

**Request Body:**
```json
{
  "nodeA": "HostA",
  "nodeB": "Router1",
  "probability": 0.1
}
```

**Parameters:**
- `probability`: Value between 0.0 and 1.0 (e.g., 0.1 = 10% packet loss)

---

### Export Topology
**GET** `/topology`

Export the entire network topology as JSON.

**Example:**
```bash
curl http://localhost:8080/topology
```

**Response:**
```json
{
  "nodes": [...],
  "links": [...],
  "vlans": {...},
  ...
}
```

---

### Import Topology
**POST** `/topology/import`

Import a network topology from JSON.

**Request Body:**
```json
{
  "nodes": [...],
  "links": [...]
}
```

---

## Network Simulation

### Ping
**POST** `/ping`

Simulate ICMP ping between two nodes.

**Request Body:**
```json
{
  "src": "HostA",
  "dst": "HostB"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/ping \
  -H "Content-Type: application/json" \
  -d '{"src":"HostA","dst":"HostB"}'
```

**Response:**
```json
{
  "success": true,
  "path": ["HostA", "Router1", "HostB"],
  "hops": 3
}
```

---

### Traceroute
**POST** `/traceroute`

Trace the route from source to destination.

**Request Body:**
```json
{
  "src": "HostA",
  "dst": "HostB"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/traceroute \
  -H "Content-Type: application/json" \
  -d '{"src":"HostA","dst":"HostB"}'
```

**Response:**
```json
{
  "success": true,
  "path": ["HostA", "Router1", "Router2", "HostB"],
  "hops": 4
}
```

---

### Multicast
**POST** `/multicast`

Send a packet from one source to multiple destinations.

**Request Body:**
```json
{
  "src": "HostA",
  "destinations": ["HostB", "HostC", "HostD"]
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/multicast \
  -H "Content-Type: application/json" \
  -d '{"src":"HostA","destinations":["HostB","HostC","HostD"]}'
```

**Response:**
```json
{
  "success": true,
  "destinations": 3
}
```

---

### TCP Connection
**POST** `/tcp/connect`

Initiate a TCP 3-way handshake simulation.

**Request Body:**
```json
{
  "client": "HostA",
  "server": "HostB"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/tcp/connect \
  -H "Content-Type: application/json" \
  -d '{"client":"HostA","server":"HostB"}'
```

**Response:**
```json
{
  "success": true,
  "message": "TCP connection established"
}
```

---

## VLAN & Firewall

### Assign VLAN
**POST** `/vlan/assign`

Assign a VLAN ID to a node for network isolation.

**Request Body:**
```json
{
  "name": "HostA",
  "vlanId": 10
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/vlan/assign \
  -H "Content-Type: application/json" \
  -d '{"name":"HostA","vlanId":10}'
```

---

### Add Firewall Rule
**POST** `/firewall/rule`

Add a firewall rule to allow or deny traffic.

**Request Body:**
```json
{
  "src": "HostA",
  "dst": "HostB",
  "protocol": "TCP",
  "allow": true
}
```

**Parameters:**
- `allow`: `true` to allow traffic, `false` to deny

**Example:**
```bash
# Allow TCP traffic
curl -X POST http://localhost:8080/firewall/rule \
  -H "Content-Type: application/json" \
  -d '{"src":"HostA","dst":"HostB","protocol":"TCP","allow":true}'

# Deny UDP traffic
curl -X POST http://localhost:8080/firewall/rule \
  -H "Content-Type: application/json" \
  -d '{"src":"HostC","dst":"HostD","protocol":"UDP","allow":false}'
```

---

## Wireless Networks

### Set Wireless Range
**POST** `/wireless/range`

Set the wireless transmission range for a node.

**Request Body:**
```json
{
  "name": "WifiAP",
  "range": 100
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/wireless/range \
  -H "Content-Type: application/json" \
  -d '{"name":"WifiAP","range":100}'
```

---

### Simulate Interference
**POST** `/wireless/interference`

Simulate wireless interference affecting a node.

**Request Body:**
```json
{
  "name": "WifiAP",
  "lossProb": 0.2
}
```

**Parameters:**
- `lossProb`: Packet loss probability due to interference (0.0 to 1.0)

**Example:**
```bash
curl -X POST http://localhost:8080/wireless/interference \
  -H "Content-Type: application/json" \
  -d '{"name":"WifiAP","lossProb":0.2}'
```

---

## Cloud Integration

### Add Cloud Node
**POST** `/cloud/add`

Add a cloud computing node to the network.

**Request Body:**
```json
{
  "name": "CloudServer1",
  "ip": "10.0.1.1"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/cloud/add \
  -H "Content-Type: application/json" \
  -d '{"name":"CloudServer1","ip":"10.0.1.1"}'
```

---

### Scale Up Cloud
**POST** `/cloud/scaleup`

Increase cloud resources (auto-scaling simulation).

**Request Body:**
```json
{
  "name": "CloudServer1"
}
```

---

### Scale Down Cloud
**POST** `/cloud/scaledown`

Decrease cloud resources.

**Request Body:**
```json
{
  "name": "CloudServer1"
}
```

---

### List Cloud Nodes
**GET** `/cloudnodes`

Get all cloud nodes in the network.

**Example:**
```bash
curl http://localhost:8080/cloudnodes
```

**Response:**
```json
{
  "cloudNodes": ["CloudServer1", "CloudServer2"],
  "count": 2
}
```

---

## IoT Devices

### Add IoT Device
**POST** `/iot/add`

Add an IoT device to the network.

**Request Body:**
```json
{
  "name": "Sensor1",
  "ip": "10.0.2.1"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/iot/add \
  -H "Content-Type: application/json" \
  -d '{"name":"Sensor1","ip":"10.0.2.1"}'
```

---

### Simulate Battery Drain
**POST** `/iot/battery`

Simulate battery drain on an IoT device.

**Request Body:**
```json
{
  "name": "Sensor1",
  "percent": 10
}
```

**Parameters:**
- `percent`: Battery percentage to drain

**Example:**
```bash
curl -X POST http://localhost:8080/iot/battery \
  -H "Content-Type: application/json" \
  -d '{"name":"Sensor1","percent":10}'
```

**Response:**
```json
{
  "result": "battery drained",
  "name": "Sensor1",
  "batteryLevel": 90
}
```

---

## Statistics & Metrics

### Get Network Statistics
**GET** `/statistics`

Get overall network statistics.

**Example:**
```bash
curl http://localhost:8080/statistics
```

**Response:**
```json
{
  "totalPackets": 1500,
  "totalPacketsSent": 750,
  "totalPacketsReceived": 745,
  "mostActiveNode": "Router1"
}
```

---

### Reset Statistics
**POST** `/statistics/reset`

Reset network statistics (all or for a specific node).

**Request Body (reset all):**
```json
{}
```

**Request Body (reset specific node):**
```json
{
  "node": "HostA"
}
```

**Example:**
```bash
# Reset all statistics
curl -X POST http://localhost:8080/statistics/reset \
  -H "Content-Type: application/json" \
  -d '{}'

# Reset specific node
curl -X POST http://localhost:8080/statistics/reset \
  -H "Content-Type: application/json" \
  -d '{"node":"HostA"}'
```

---

### Get Performance Metrics
**POST** `/metrics/performance`

Get latency, throughput, and packet loss metrics between two nodes.

**Request Body:**
```json
{
  "nodeA": "HostA",
  "nodeB": "HostB"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/metrics/performance \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"HostA","nodeB":"HostB"}'
```

**Response:**
```json
{
  "latency": 15,
  "throughput": 950,
  "packetLossRate": 0.05
}
```

---

## Server Status

### Check Status
**GET** `/status`

Check if the server is running.

**Example:**
```bash
curl http://localhost:8080/status
```

**Response:**
```json
{
  "status": "ok",
  "message": "NetSimCPP REST API v1.0"
}
```

---

## Complete Example Workflow

Here's a complete example of creating a simple network topology:

```bash
# 1. Check server status
curl http://localhost:8080/status

# 2. Add nodes
curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"PC1","ip":"192.168.1.10","type":"host"}'

curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"PC2","ip":"192.168.1.11","type":"host"}'

curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"Router1","ip":"192.168.1.1","type":"router"}'

# 3. Connect nodes
curl -X POST http://localhost:8080/link/connect \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"PC1","nodeB":"Router1"}'

curl -X POST http://localhost:8080/link/connect \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"Router1","nodeB":"PC2"}'

# 4. Configure link properties
curl -X POST http://localhost:8080/link/delay \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"PC1","nodeB":"Router1","delay":5}'

curl -X POST http://localhost:8080/link/bandwidth \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"PC1","nodeB":"Router1","bandwidth":1000}'

# 5. Test connectivity
curl -X POST http://localhost:8080/ping \
  -H "Content-Type: application/json" \
  -d '{"src":"PC1","dst":"PC2"}'

curl -X POST http://localhost:8080/traceroute \
  -H "Content-Type: application/json" \
  -d '{"src":"PC1","dst":"PC2"}'

# 6. Get network statistics
curl http://localhost:8080/statistics

# 7. Export topology
curl http://localhost:8080/topology > topology.json

# 8. List all nodes
curl http://localhost:8080/nodes
```

---

## Error Handling

All endpoints return appropriate HTTP status codes:

- `200 OK`: Successful operation
- `400 Bad Request`: Invalid request parameters
- `404 Not Found`: Endpoint or resource not found
- `500 Internal Server Error`: Server-side error

Error response format:
```json
{
  "error": "Detailed error message"
}
```

---

## Notes

- All POST requests require `Content-Type: application/json` header
- Node names must be unique within the network
- Link operations require both nodes to exist
- VLAN nodes can only communicate with nodes in the same VLAN
- IoT devices automatically disconnect when battery reaches 0%
- Cloud nodes support auto-scaling operations
- Wireless range is measured in abstract units
- Packet loss probability is a value between 0.0 and 1.0

---

## Support

For issues, questions, or contributions, please visit the [GitHub repository](https://github.com/UmarlyPoeta/inzynieria_oprogramowania).
