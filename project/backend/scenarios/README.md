# Network Scenarios & Playbooks

## Overview
Network scenarios are YAML-based configuration files that define step-by-step network setups, traffic patterns, and validation rules. They enable:
- Quick testing of network configurations
- Educational demonstrations
- Repeatable experiments
- Automated validation

## Scenario Format

```yaml
name: "Scenario Name"
description: "Brief description of what this scenario demonstrates"
version: "1.0"
author: "Author Name"
tags: ["tag1", "tag2"]

# Initial network setup
setup:
  nodes:
    - name: "NodeName"
      type: "host|router"
      ip: "IP address"
      config:
        key: value
  
  links:
    - from: "Node1"
      to: "Node2"
      delay_ms: 10
      bandwidth_mbps: 100
      packet_loss: 0.01

# Execution steps
steps:
  - name: "Step description"
    action: "ping|send|configure|wait|validate"
    params:
      # Action-specific parameters
    expect:
      # Expected outcomes

# Validation rules
validation:
  - type: "connectivity|latency|packet_loss|throughput"
    params:
      # Validation parameters
    threshold:
      # Pass/fail criteria
```

## Available Actions

### ping
Test connectivity between nodes
```yaml
- name: "Test connectivity"
  action: ping
  params:
    from: "Host1"
    to: "Host2"
  expect:
    success: true
    max_latency_ms: 50
```

### send
Send packets between nodes
```yaml
- name: "Send data packets"
  action: send
  params:
    from: "Host1"
    to: "Host2"
    count: 100
    size_bytes: 1024
    protocol: "TCP|UDP"
  expect:
    delivery_rate: 0.99
```

### configure
Modify node configuration
```yaml
- name: "Update router settings"
  action: configure
  params:
    node: "Router1"
    config:
      mtu: 1500
      ttl: 64
```

### wait
Pause execution
```yaml
- name: "Wait for convergence"
  action: wait
  params:
    duration_ms: 1000
```

### validate
Check network state
```yaml
- name: "Verify routing table"
  action: validate
  params:
    type: "routing"
    node: "Router1"
  expect:
    routes_count: 3
```

## Example Scenarios

See the `examples/` directory for sample scenarios:
- `basic_ping.yaml` - Simple two-host ping test
- `vlan_isolation.yaml` - VLAN segmentation demonstration
- `router_failover.yaml` - Redundant routing scenario
- `congestion_control.yaml` - TCP congestion handling
- `qos_priority.yaml` - Quality of Service testing

## Running Scenarios

### Via API
```bash
curl -X POST http://localhost:8080/scenario/run \
  -H "Content-Type: application/json" \
  -d '{"file": "scenarios/basic_ping.yaml"}'
```

### Via CLI (future)
```bash
./netsim scenario run scenarios/basic_ping.yaml
./netsim scenario validate scenarios/basic_ping.yaml
./netsim scenario list
```

## Scenario Development

1. Start with a simple topology
2. Add incremental steps
3. Define clear validation criteria
4. Test manually first
5. Automate with scenario file
6. Document expected outcomes

## Best Practices

- **Keep scenarios focused**: One concept per scenario
- **Use descriptive names**: Both scenario and step names should be clear
- **Add validation**: Every scenario should validate outcomes
- **Document assumptions**: State prerequisites and expected environment
- **Version scenarios**: Track changes like code
- **Use tags**: Categorize for easy discovery (e.g., "beginner", "routing", "performance")
