# NetSimCPP - Architecture with Database

> **PlantUML Diagrams**: All diagrams are available as PlantUML source files in `project/docs/UML/`

## System Overview

 **[PlantUML Source](../project/docs/UML/system_overview.puml)**

## Data Flow - Save Topology

 **[PlantUML Source](../project/docs/UML/save_topology_sequence.puml)**


## Data Flow - Load Topology

 **[PlantUML Source](../project/docs/UML/load_topology_sequence.puml)**

## Hybrid Architecture

 **[PlantUML Source](../project/docs/UML/hybrid_architecture.puml)**

NetSimCPP uses a **hybrid memory + database** approach:

### In Memory (Fast Operations)
- Current network topology (nodes, adjacency map)
- Packet queues
- Routing computations (BFS, Dijkstra)
- Real-time simulation state

### In Database (Persistent)
- Historical network configurations
- Packet statistics over time
- Node/link metadata
- VLAN configurations
- Congestion records

### Benefits
1. **Performance**: Fast in-memory operations for simulation
2. **Persistence**: Survive server restarts
3. **Analytics**: Historical data analysis
4. **Scalability**: Future multi-user support

## Docker Deployment Architecture

 **[PlantUML Source](../project/docs/UML/docker_deployment.puml)**


## CI/CD Pipeline with Database

 **[PlantUML Source](../project/docs/UML/cicd_pipeline.puml)**

---

**Last Updated**: 2025-10-28  
**Version**: 1.0
