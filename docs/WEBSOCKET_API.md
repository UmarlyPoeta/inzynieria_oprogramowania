# WebSocket API Documentation

## Overview

NetSimCPP provides a WebSocket server for real-time network topology and event monitoring. This enables frontend applications to receive instant notifications about network changes without polling.

**WebSocket Server**: `ws://localhost:9001`

## Connection

### Establishing Connection

```javascript
const ws = new WebSocket('ws://localhost:9001');

ws.onopen = () => {
    console.log('Connected to NetSimCPP WebSocket server');
};

ws.onmessage = (event) => {
    const message = JSON.parse(event.data);
    console.log('Received event:', message);
};

ws.onclose = () => {
    console.log('Disconnected from server');
};

ws.onerror = (error) => {
    console.error('WebSocket error:', error);
};
```

### Welcome Message

Upon connection, the server sends a welcome message:

```json
{
  "type": "connected",
  "timestamp": 1640000000,
  "data": {
    "message": "Connected to NetSimCPP WebSocket server",
    "version": "1.0.0"
  }
}
```

## Message Format

All WebSocket messages follow this structure:

```json
{
  "type": "event_type",
  "timestamp": 1640000000,
  "data": {
    // Event-specific data
  }
}
```

## Event Types

### Node Events

#### `node_added`

Broadcast when a new node is added to the network.

```json
{
  "type": "node_added",
  "timestamp": 1640000000,
  "data": {
    "name": "H1",
    "type": "host",
    "ip": "10.0.0.1"
  }
}
```

#### `node_removed`

Broadcast when a node is removed from the network.

```json
{
  "type": "node_removed",
  "timestamp": 1640000000,
  "data": {
    "name": "H1"
  }
}
```

#### `node_failed`

Broadcast when a node fails.

```json
{
  "type": "node_failed",
  "timestamp": 1640000000,
  "data": {
    "name": "H1",
    "status": "failed"
  }
}
```

#### `node_recovered`

Broadcast when a failed node recovers.

```json
{
  "type": "node_recovered",
  "timestamp": 1640000000,
  "data": {
    "name": "H1",
    "status": "active"
  }
}
```

### `node_updated` Event

Broadcast when node configuration is updated via `/node/config`.

**Example:**
```json
{
  "type": "node_updated",
  "timestamp": 1640000000000,
  "data": {
    "name": "H1",
    "nodeType": "host",
    "ip": "192.168.1.100"
  }
}
```

### Link Events

#### `link_added`

Broadcast when a link is created between two nodes.

```json
{
  "type": "link_added",
  "timestamp": 1640000000,
  "data": {
    "from": "H1",
    "to": "R1",
    "delay": 10,
    "bandwidth": 1000
  }
}
```

#### `link_removed`

Broadcast when a link is disconnected.

```json
{
  "type": "link_removed",
  "timestamp": 1640000000,
  "data": {
    "from": "H1",
    "to": "R1"
  }
}
```

### Packet Events

#### `packet_sent`

Broadcast when a packet is transmitted (real-time packet monitoring).

```json
{
  "type": "packet_sent",
  "timestamp": 1640000000,
  "data": {
    "from": "H1",
    "to": "H2",
    "protocol": "TCP",
    "size": 1500
  }
}
```

### Statistics Events

#### `statistics_update`

Broadcast when network statistics are updated.

```json
{
  "type": "statistics_update",
  "timestamp": 1640000000,
  "data": {
    "total_nodes": 10,
    "total_links": 15,
    "packets_sent": 1000,
    "packets_dropped": 5
  }
}
```

### Topology Events

#### `topology_changed`

Broadcast when the network topology changes significantly.

```json
{
  "type": "topology_changed",
  "timestamp": 1640000000,
  "data": {
    "message": "Network topology has been modified"
  }
}
```

## Client-to-Server Messages

### Ping

Send a ping to check server responsiveness:

```javascript
ws.send(JSON.stringify({ type: 'ping' }));
```

Server response:

```json
{
  "type": "pong",
  "timestamp": 1640000000,
  "data": {
    "timestamp": 1640000000
  }
}
```

### Subscribe (Future Feature)

Subscribe to specific event channels:

```javascript
ws.send(JSON.stringify({
  type: 'subscribe',
  channel: 'packets'  // 'nodes', 'links', 'packets', 'all'
}));
```

Server response:

```json
{
  "type": "subscribed",
  "timestamp": 1640000000,
  "data": {
    "channel": "packets"
  }
}
```

## Integration Example

### React Hook

```javascript
import { useEffect, useState } from 'react';

function useNetSimWebSocket(url = 'ws://localhost:9001') {
  const [events, setEvents] = useState([]);
  const [connected, setConnected] = useState(false);

  useEffect(() => {
    const ws = new WebSocket(url);

    ws.onopen = () => setConnected(true);
    ws.onclose = () => setConnected(false);
    
    ws.onmessage = (event) => {
      const message = JSON.parse(event.data);
      setEvents(prev => [message, ...prev].slice(0, 100)); // Keep last 100 events
    };

    return () => ws.close();
  }, [url]);

  return { events, connected };
}

// Usage
function NetworkMonitor() {
  const { events, connected } = useNetSimWebSocket();

  return (
    <div>
      <h2>Status: {connected ? 'Connected' : 'Disconnected'}</h2>
      <ul>
        {events.map((event, i) => (
          <li key={i}>{event.type}: {JSON.stringify(event.data)}</li>
        ))}
      </ul>
    </div>
  );
}
```

### Vue.js Component

```vue
<template>
  <div>
    <h2>Status: {{ connected ? 'Connected' : 'Disconnected' }}</h2>
    <div v-for="event in events" :key="event.timestamp">
      <strong>{{ event.type }}</strong>: {{ JSON.stringify(event.data) }}
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      ws: null,
      connected: false,
      events: []
    };
  },
  mounted() {
    this.ws = new WebSocket('ws://localhost:9001');
    
    this.ws.onopen = () => {
      this.connected = true;
    };
    
    this.ws.onclose = () => {
      this.connected = false;
    };
    
    this.ws.onmessage = (event) => {
      const message = JSON.parse(event.data);
      this.events.unshift(message);
      
      // Keep only last 100 events
      if (this.events.length > 100) {
        this.events.pop();
      }
    };
  },
  beforeUnmount() {
    if (this.ws) {
      this.ws.close();
    }
  }
};
</script>
```

## Testing WebSocket

### Using the Monitor HTML

Open `project/frontend/websocket-monitor.html` in your browser to test WebSocket connections.

### Using wscat CLI

```bash
npm install -g wscat
wscat -c ws://localhost:9001
```

### Using curl (WebSocket upgrade)

```bash
curl --include \
     --no-buffer \
     --header "Connection: Upgrade" \
     --header "Upgrade: websocket" \
     --header "Sec-WebSocket-Version: 13" \
     --header "Sec-WebSocket-Key: SGVsbG8sIHdvcmxkIQ==" \
     http://localhost:9001/
```

## Performance Considerations

- **Broadcast Throttling**: High-frequency events (like packet_sent) may be throttled to prevent overwhelming clients
- **Connection Limit**: Server can handle hundreds of concurrent WebSocket connections
- **Auto-Reconnect**: Implement exponential backoff in clients for automatic reconnection
- **Heartbeat**: Server may send periodic ping frames to keep connections alive

## Security

### CORS

WebSocket connections are not subject to CORS, but consider:
- Use `wss://` (WebSocket Secure) in production
- Implement authentication tokens
- Validate Origin headers

### Authentication (Future)

```javascript
const ws = new WebSocket('ws://localhost:9001');

ws.onopen = () => {
  // Send auth token
  ws.send(JSON.stringify({
    type: 'authenticate',
    token: 'your-jwt-token'
  }));
};
```

## Troubleshooting

### Connection Refused

- Check that WebSocket server is running on port 9001
- Verify firewall settings
- Check Docker port mapping: `docker ps | grep 9001`

### Messages Not Received

- Check browser console for WebSocket errors
- Verify message format in server logs
- Test with simple ping/pong

### High Latency

- Check network conditions
- Verify server CPU usage
- Consider implementing message batching for high-frequency events

## Docker Deployment

WebSocket is exposed in `docker-compose.yml`:

```yaml
services:
  netsim:
    ports:
      - "8080:8080"  # REST API
      - "9001:9001"  # WebSocket
```

Access from host:
- REST API: `http://localhost:8080`
- WebSocket: `ws://localhost:9001`

## Event Flow Diagram

```
REST API Call          WebSocket Broadcast
     ↓                        ↓
POST /node/add  →  [Network]  →  node_added event  →  All Clients
POST /link/connect →  [Network]  →  link_added event →  All Clients
POST /node/fail    →  [Network]  →  node_failed event →  All Clients
```

## Future Enhancements

- **Channel Subscriptions**: Subscribe to specific event types
- **Binary Protocol**: Use binary WebSocket frames for efficiency
- **Compression**: Enable per-message deflate extension
- **Replay**: Request historical events on connect
- **Authentication**: JWT-based client authentication
- **Rate Limiting**: Per-client message rate limits
