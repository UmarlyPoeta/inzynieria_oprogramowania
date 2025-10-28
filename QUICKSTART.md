# NetSimCPP - Quick Start Guide

## One-Command Setup (Docker)

```bash
# Clone and start
git clone https://github.com/UmarlyPoeta/inzynieria_oprogramowania.git
cd inzynieria_oprogramowania
docker-compose up -d

# Wait for services to be ready (30 seconds)
sleep 30

# Test the server
curl http://localhost:8080/status
```

**That's it!** You now have:
-  NetSimCPP Server running on http://localhost:8080
-  MySQL Database running on localhost:3306
-  Adminer (Web GUI) running on http://localhost:8081

---

## First Network Simulation

```bash
# 1. Enable database persistence
curl -X POST http://localhost:8080/db/enable \
  -H "Content-Type: application/json" \
  -d '{
    "host": "mysql",
    "port": 3306,
    "user": "root",
    "password": "NetSimCPP1234",
    "database": "netsim"
  }'

# 2. Create hosts
curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"H1", "type":"host", "ip":"10.0.0.1"}'

curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"H2", "type":"host", "ip":"10.0.0.2"}'

# 3. Create router
curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"R1", "type":"router", "ip":"10.0.0.254"}'

# 4. Connect nodes
curl -X POST http://localhost:8080/link/connect \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"H1", "nodeB":"R1"}'

curl -X POST http://localhost:8080/link/connect \
  -H "Content-Type: application/json" \
  -d '{"nodeA":"R1", "nodeB":"H2"}'

# 5. Test connectivity
curl -X POST http://localhost:8080/ping \
  -H "Content-Type: application/json" \
  -d '{"source":"H1", "destination":"H2"}'

# 6. Save topology
curl http://localhost:8080/db/save

# 7. View in browser
# Open http://localhost:8081
# Login: Server=mysql, User=root, Password=NetSimCPP1234, Database=netsim
# You'll see your network in the database!
```

---

## Common Commands

```bash
# View all nodes
curl http://localhost:8080/nodes

# Get full topology
curl http://localhost:8080/topology

# Get statistics
curl http://localhost:8080/statistics

# Check database status
curl http://localhost:8080/db/status

# Restart with saved topology
docker-compose restart netsim
curl http://localhost:8080/db/load

# View logs
docker-compose logs -f netsim

# Stop everything
docker-compose down

# Stop and remove database (careful!)
docker-compose down -v
```

---

## What's Running?

After `docker-compose up -d`:

| Service | Port | URL | Description |
|---------|------|-----|-------------|
| NetSimCPP | 8080 | http://localhost:8080 | REST API server |
| MySQL | 3306 | localhost:3306 | Database |
| Adminer | 8081 | http://localhost:8081 | MySQL Web GUI |

---

## Troubleshooting

**Server not responding?**
```bash
docker-compose ps  # Check if services are running
docker-compose logs netsim  # View server logs
```

**Database connection failed?**
```bash
docker-compose logs mysql  # View MySQL logs
docker-compose restart mysql  # Restart MySQL
```

**Reset everything:**
```bash
docker-compose down -v  # Remove containers and volumes
docker-compose up -d    # Start fresh
```

---

## Next Steps

1. **Read the API docs**: See full README.md for all 34 endpoints
2. **Try the test script**: `./scripts/test_database.sh`
3. **Explore diagrams**: Check `project/docs/UML/` for architecture
4. **Read database guide**: `docs/INSTALL_DATABASE.md`

---

## Need Help?

- **Full README**: `README.md` (English & Polish)
- **Database Setup**: `docs/INSTALL_DATABASE.md`
- **API Reference**: `project/database/README.md`
- **Architecture**: `docs/database_integration_plan.md`


