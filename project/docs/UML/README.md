# PlantUML Diagrams - Net### 3. Save Topology Sequ### 5. Docker Deployment (`docker_deployment.puml`)
- **Type**: Deployment Diagram
- **Purpose**: Shows Docker Compose architecture
- **Includes**:
  - Docker Host with netsim-network bridge
  - 3 containers (netsim, mysql, adminer)
  - Volume (mysql_data)
  - Port mappings (8080, 3306, 8081)
  - Container connections

### 6. CI/CD Pipeline (`cicd_pipeline.puml`)_topology_sequence.puml`)
- **Type**: Sequence Diagram
- **Purpose**: Shows the flow of saving network topology to database
- **Includes**:
  - Client → REST API → Network → DatabaseManager → Repositories → MySQL
  - Transaction handling (BEGIN → INSERT operations → COMMIT)
  - All actors and their interactions

### 4. Load Topology Sequence (`load_topology_sequence.puml`)chitecture

This directory contains PlantUML source files for NetSimCPP architecture diagrams.

## 📋 Available Diagrams

### 1. System Overview (`system_overview.puml`)
- **Type**: Component + Package Diagram
- **Purpose**: Shows the complete system architecture with all layers
- **Includes**:
  - Client Layer (Web Browser, curl, Custom Apps)
  - Application Layer (REST API, Network Manager, Engine, DatabaseManager, Repositories)
  - Persistence Layer (MySQL database with simplified schema)
  - All connections between components

### 2. Database Schema (`database_schema.puml`)
- **Type**: Entity Relationship Diagram (ERD)
- **Purpose**: Shows complete MySQL database schema with relationships
- **Includes**:
  - All 5 tables (nodes, links, packet_stats, vlans, congestion)
  - Primary keys, foreign keys, indexes
  - Cascade rules and constraints
  - Data types and default values
  - Relationships with cardinality

### 3. Save Topology Sequence (`save_topology_sequence.puml`)
- **Type**: Sequence Diagram
- **Purpose**: Shows the flow of saving network topology to database
- **Includes**:
  - Client → REST API → Network → DatabaseManager → Repositories → MySQL
  - Transaction handling (BEGIN → INSERT operations → COMMIT)
  - All actors and their interactions

### 4. Load Topology Sequence (`load_topology_sequence.puml`)
- **Type**: Sequence Diagram
- **Purpose**: Shows the flow of loading network topology from database
- **Includes**:
  - Client → REST API → Network → Repositories → MySQL
  - Clear in-memory topology
  - Rebuild nodes and links from database
  - Restore adjacency map

### 5. Docker Deployment (`docker_deployment.puml`)
- **Type**: Deployment Diagram
- **Purpose**: Shows Docker Compose architecture
- **Includes**:
  - Docker Host with netsim-network bridge
  - 3 containers (netsim, mysql, adminer)
  - Volume (mysql_data)
  - Port mappings (8080, 3306, 8081)
  - Container connections

### 6. CI/CD Pipeline (`cicd_pipeline.puml`)
- **Type**: Activity Diagram
- **Purpose**: Shows GitHub Actions CI/CD workflow
- **Includes**:
  - All jobs (build-and-test, performance-tests, docker-build, code-quality)
  - MySQL service setup
  - Database schema loading
  - Build and test steps
  - Integration tests

### 7. Hybrid Architecture (`hybrid_architecture.puml`)
- **Type**: Component Diagram
- **Purpose**: Illustrates the hybrid in-memory + database approach
- **Includes**:
  - In-Memory components (Network Topology, Packet Queues, Routing Engine, Simulation State)
  - Database components (Historical data, Statistics, Metadata, VLANs, Congestion)
  - Synchronization layer with save/load operations
  - Benefits of each approach

## 🚀 How to Generate PNG/SVG from PlantUML

### Validate Diagrams First

Before generating images, validate syntax:
```bash
cd project/docs/UML
./validate_diagrams.sh
```

This script checks all `.puml` files for syntax errors using Docker.

### Option 1: VS Code Extension (Recommended)
1. Install extension: `jebbs.plantuml`
2. Open any `.puml` file
3. Press `Alt+D` to preview
4. Right-click → Export Current Diagram → Choose format (PNG/SVG)

### Option 2: Command Line
```bash
# Install PlantUML
sudo apt-get install plantuml

# Generate PNG
plantuml system_overview.puml

# Generate SVG
plantuml -tsvg system_overview.puml

# Generate all diagrams
plantuml *.puml
```

### Option 3: Docker
```bash
# Generate all diagrams to PNG
docker run --rm -v $(pwd):/data plantuml/plantuml *.puml

# Generate to SVG
docker run --rm -v $(pwd):/data plantuml/plantuml -tsvg *.puml
```

### Option 4: Online Editor
1. Visit: https://www.plantuml.com/plantuml/uml/
2. Copy-paste `.puml` content
3. Download generated image

## 📊 Viewing Diagrams in GitHub

GitHub doesn't natively render PlantUML, but you can:

### Method 1: Generate and commit PNGs
```bash
# Generate all PNGs
plantuml *.puml

# Commit both .puml and .png files
git add *.puml *.png
git commit -m "Add/update architecture diagrams"
```

### Method 2: Use PlantUML Proxy
In your Markdown files, use:
```markdown
![Diagram](http://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/UmarlyPoeta/inzynieria_oprogramowania/main/project/docs/UML/system_overview.puml)
```

### Method 3: GitHub Actions (Automated)
Create `.github/workflows/plantuml.yml`:
```yaml
name: Generate PlantUML
on: [push]
jobs:
  plantuml:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Generate PlantUML diagrams
        uses: grassedge/generate-plantuml-action@v1.5
        with:
          path: project/docs/UML
          message: "Auto-generated PlantUML diagrams"
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

## 🎨 Customization

All diagrams use:
- `!theme plain` - Clean, professional look
- Color-coded sections (green for memory, blue for DB, yellow for sync)
- Consistent styling across all diagrams
- Comments and notes for clarification

To customize:
1. Edit `.puml` files
2. See PlantUML documentation: https://plantuml.com/
3. Test changes with live preview (VS Code extension or online editor)

## 📚 PlantUML Resources

- **Official Documentation**: https://plantuml.com/
- **Cheat Sheet**: https://ogom.github.io/draw_uml/plantuml/
- **Real World Examples**: https://real-world-plantuml.com/
- **VS Code Extension**: https://marketplace.visualstudio.com/items?itemName=jebbs.plantuml

## 🔄 Updating Diagrams

When architecture changes:
1. Update corresponding `.puml` file
2. Regenerate PNG/SVG if needed
3. Update `ARCHITECTURE_WITH_DB.md` if structure changes
4. Commit both source and generated files

---

**Last Updated**: 2025-10-28  
**Version**: 1.0  
**Maintained by**: NetSimCPP Team
