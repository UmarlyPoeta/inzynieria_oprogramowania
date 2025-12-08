# Traversal Algorithms Test API - Przykłady użycia

## Uruchomienie serwera testowego

```bash
cd project/backend
mkdir -p build && cd build
cmake ..
make test_traversal_api
./test_traversal_api
```

Serwer uruchomi się na `http://localhost:8081/test/traversal`

## Topologia testowa

Sieć składa się z 6 hostów (A-F) połączonych w siatkę:

```
A --- B --- C
|     |     |
D --- E --- F
```

**Opóźnienia (ms):**
- A-B: 10, B-C: 10, A-D: 15, B-E: 5
- C-F: 10, D-E: 10, E-F: 5

**Bandwidth (Mbps):**
- A-B: 100, B-C: 100, A-D: 50, B-E: 150
- C-F: 100, D-E: 75, E-F: 200

---

## Endpointy

### 1. Help - Lista wszystkich endpointów

**Request:**
```bash
curl -X GET http://localhost:8081/test/traversal/help
```

**Response:**
```json
{
  "endpoints": [
    "POST /test/traversal/ecmp/next-hops - ECMP next hops",
    "POST /test/traversal/ecmp/k-paths - ECMP k paths",
    "POST /test/traversal/constrained-path - Constrained shortest path",
    "POST /test/traversal/link-state - Link-state routing",
    "POST /test/traversal/rpf - Reverse Path Forwarding check",
    "POST /test/traversal/multipath - Multipath flow-aware",
    "POST /test/traversal/multi-commodity - Multi-commodity flow"
  ]
}
```

---

### 2. ECMP Next Hops - Równokosztowe następne skoki

Zwraca listę sąsiadów będących na równokosztowych najkrótszych ścieżkach.

**Request:**
```bash
curl -X POST http://localhost:8081/test/traversal/ecmp/next-hops \
  -H "Content-Type: application/json" \
  -d '{
    "node": "B",
    "dst": "F"
  }'
```

**Response:**
```json
{
  "success": true,
  "node": "B",
  "destination": "F",
  "next_hops": ["C", "E"],
  "count": 2
}
```

---

### 3. Constrained Shortest Path - Ścieżka z ograniczeniem bandwidth

Znajduje najkrótszą ścieżkę (wg opóźnienia) używając tylko łączy z bandwidth >= min_bandwidth.

**Request:**
```bash
curl -X POST http://localhost:8081/test/traversal/constrained-path \
  -H "Content-Type: application/json" \
  -d '{
    "src": "A",
    "dst": "F",
    "min_bandwidth": 100
  }'
```

**Response:**
```json
{
  "success": true,
  "path": ["A", "B", "E", "F"],
  "hops": 4,
  "min_bandwidth": 100,
  "total_delay": 20
}
```

**Przypadek bez ścieżki:**
```bash
curl -X POST http://localhost:8081/test/traversal/constrained-path \
  -H "Content-Type: application/json" \
  -d '{
    "src": "A",
    "dst": "F",
    "min_bandwidth": 250
  }'
```

Response:
```json
{
  "success": false,
  "path": [],
  "hops": 0,
  "min_bandwidth": 250
}
```

---

### 4. Link-State Routing - Tablica routingu dla węzła

Oblicza tablicę next-hop dla danego węzła (OSPF-like).

**Request:**
```bash
curl -X POST http://localhost:8081/test/traversal/link-state \
  -H "Content-Type: application/json" \
  -d '{
    "node": "B"
  }'
```

**Response:**
```json
{
  "success": true,
  "node": "B",
  "routing_table": {
    "A": "A",
    "C": "C",
    "D": "E",
    "E": "E",
    "F": "E"
  },
  "destinations_count": 5
}
```

---

### 5. RPF - Reverse Path Forwarding Check

Sprawdza czy pakiet przychodzący z danego sąsiada spełnia warunek RPF.

**Request (prawidłowy):**
```bash
curl -X POST http://localhost:8081/test/traversal/rpf \
  -H "Content-Type: application/json" \
  -d '{
    "node": "E",
    "incoming_neighbor": "B",
    "src": "A"
  }'
```

**Response:**
```json
{
  "success": true,
  "node": "E",
  "incoming_neighbor": "B",
  "packet_source": "A",
  "rpf_check": true,
  "action": "ACCEPT"
}
```

**Request (odrzucony):**
```bash
curl -X POST http://localhost:8081/test/traversal/rpf \
  -H "Content-Type: application/json" \
  -d '{
    "node": "E",
    "incoming_neighbor": "F",
    "src": "A"
  }'
```

**Response:**
```json
{
  "success": true,
  "node": "E",
  "incoming_neighbor": "F",
  "packet_source": "A",
  "rpf_check": false,
  "action": "REJECT"
}
```

---

### 6. Multipath Flow-Aware - Podział ruchu na wiele ścieżek

Znajduje k alternatywnych ścieżek z wagami proporcjonalnymi do przepustowości.

**Request:**
```bash
curl -X POST http://localhost:8081/test/traversal/multipath \
  -H "Content-Type: application/json" \
  -d '{
    "src": "A",
    "dst": "F",
    "k": 3
  }'
```

**Response:**
```json
{
  "success": true,
  "paths_found": 3,
  "paths": [
    {
      "path": ["A", "B", "E", "F"],
      "weight": 0.571,
      "hops": 4
    },
    {
      "path": ["A", "B", "C", "F"],
      "weight": 0.286,
      "hops": 4
    },
    {
      "path": ["A", "D", "E", "F"],
      "weight": 0.143,
      "hops": 4
    }
  ]
}
```

---

### 7. Multi-Commodity Flow - Planowanie przepływów

Przydziela ścieżki dla wielu par źródło-cel z zapotrzebowaniem.

**Request:**
```bash
curl -X POST http://localhost:8081/test/traversal/multi-commodity \
  -H "Content-Type: application/json" \
  -d '{
    "commodities": [
      {"src": "A", "dst": "F", "demand": 50},
      {"src": "D", "dst": "C", "demand": 30}
    ],
    "k": 2
  }'
```

**Response:**
```json
{
  "success": true,
  "commodities_processed": 2,
  "allocations": [
    {
      "commodity": "A->F",
      "allocations": [
        {
          "path": ["A", "B", "E", "F"],
          "flow": 50
        }
      ],
      "paths_count": 1
    },
    {
      "commodity": "D->C",
      "allocations": [
        {
          "path": ["D", "E", "B", "C"],
          "flow": 30
        }
      ],
      "paths_count": 1
    }
  ]
}
```

---

## Testowanie za pomocą skryptu

```bash
chmod +x test_traversal_api.sh
./test_traversal_api.sh
```

---

## PowerShell (Windows)

### ECMP Next Hops
```powershell
Invoke-RestMethod -Method POST -Uri "http://localhost:8081/test/traversal/ecmp/next-hops" `
  -ContentType "application/json" `
  -Body '{"node": "B", "dst": "F"}' | ConvertTo-Json
```

### Constrained Path
```powershell
Invoke-RestMethod -Method POST -Uri "http://localhost:8081/test/traversal/constrained-path" `
  -ContentType "application/json" `
  -Body '{"src": "A", "dst": "F", "min_bandwidth": 100}' | ConvertTo-Json
```

### Link-State
```powershell
Invoke-RestMethod -Method POST -Uri "http://localhost:8081/test/traversal/link-state" `
  -ContentType "application/json" `
  -Body '{"node": "B"}' | ConvertTo-Json
```

### RPF
```powershell
Invoke-RestMethod -Method POST -Uri "http://localhost:8081/test/traversal/rpf" `
  -ContentType "application/json" `
  -Body '{"node": "E", "incoming_neighbor": "B", "src": "A"}' | ConvertTo-Json
```

### Multipath
```powershell
Invoke-RestMethod -Method POST -Uri "http://localhost:8081/test/traversal/multipath" `
  -ContentType "application/json" `
  -Body '{"src": "A", "dst": "F", "k": 3}' | ConvertTo-Json
```

### Multi-Commodity
```powershell
$body = @{
  commodities = @(
    @{src = "A"; dst = "F"; demand = 50},
    @{src = "D"; dst = "C"; demand = 30}
  )
  k = 2
} | ConvertTo-Json -Depth 3

Invoke-RestMethod -Method POST -Uri "http://localhost:8081/test/traversal/multi-commodity" `
  -ContentType "application/json" `
  -Body $body | ConvertTo-Json -Depth 5
```

---

## Dodanie do CMakeLists.txt

Dodaj do `project/backend/CMakeLists.txt`:

```cmake
# Test Traversal API executable
add_executable(test_traversal_api
    src/test_traversal_api.cpp
    src/core/Engine.cpp
    src/core/Network.cpp
    src/core/Node.cpp
    src/core/Host.cpp
    src/core/Router.cpp
    src/core/Packet.cpp
    src/core/TraversalAlgorithms.cpp
)

target_link_libraries(test_traversal_api
    cpprestsdk::cpprest
)
```
