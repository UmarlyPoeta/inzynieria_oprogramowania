# Jak przetestować nowe algorytmy traversal

## OPCJA 1: Docker (ZALECANE - działa na Windows)

### 1. Zbuduj obraz Docker (tylko raz):
```powershell
cd "g:\Studia\IT\Sem III\Inżynieria Oprogramowania\NetSimCPP\inzynieria_oprogramowania"
docker build -t netsimcpp .
```

### 2. Uruchom kontener i zbuduj projekt:
```powershell
docker run --rm -v "${PWD}:/workspace" -w /workspace netsimcpp bash -c "cd project/backend && mkdir -p build && cd build && cmake .. && cmake --build . --target simple_traversal_test && ./simple_traversal_test"
```

### 3. Lub wejdź do kontenera interaktywnie:
```powershell
docker run --rm -it -v "${PWD}:/workspace" -w /workspace netsimcpp bash
```

Następnie w kontenerze:
```bash
cd project/backend
mkdir -p build && cd build
cmake ..
cmake --build . --target simple_traversal_test
./simple_traversal_test
```

## OPCJA 2: WSL (wymaga instalacji Ubuntu)

### 1. Zainstaluj WSL z Ubuntu:
```powershell
wsl --install -d Ubuntu-22.04
```

### 2. Uruchom WSL i zainstaluj zależności:
```bash
cd /mnt/g/Studia/IT/"Sem III"/"Inżynieria Oprogramowania"/NetSimCPP/inzynieria_oprogramowania
chmod +x scripts/install_deps.sh
sudo ./scripts/install_deps.sh
```

### 3. Zbuduj i uruchom test:
```bash
cd project/backend
mkdir -p build && cd build
cmake ..
cmake --build . --target simple_traversal_test
./simple_traversal_test
```

## Co zrobi simple_traversal_test?

Program wykona serię testów algorytmów traversal:

1. **BFS** - shortest path by hops (A->F)
2. **Dijkstra** - shortest path by delay (A->F)
3. **DFS** - any path with depth limit
4. **ECMP** - equal-cost next hops (B->F)
5. **Constrained Path** - shortest path z bandwidth constraint (min 60 Mbps)
6. **Link-State Routing** - routing table dla węzła A
7. **RPF Check** - weryfikacja Reverse Path Forwarding
8. **Multipath** - k alternatywnych ścieżek (k=3)
9. **Multi-Commodity Flow** - alokacja flow dla 3 commodities

### Przykładowy output:
```
=== TEST ALGORYTMOW TRAVERSAL ===

Utworzono siec testowa:
  Wezly: A, B, C, D, E, F
  Polaczenia:
    A-B (delay=10ms, bw=100Mbps)
    B-C (delay=10ms, bw=100Mbps)
    A-D (delay=15ms, bw=50Mbps)  <- wąskie gardło
    ...

--- TEST 1: Podstawowy ping (BFS shortest hops) ---
Wynik: OK
Sciezka A->F (BFS)            : A -> B -> C -> F

--- TEST 2: Ping z Dijkstra (minimum delay) ---
Wynik: OK
Sciezka A->F (Dijkstra)       : A -> B -> C -> F

...
```

## Problemy?

### Docker nie jest zainstalowany:
- Pobierz Docker Desktop z https://www.docker.com/products/docker-desktop/
- Zainstaluj i uruchom ponownie komputer
- Włącz WSL 2 backend w ustawieniach Docker

### WSL nie działa:
```powershell
# Sprawdź czy WSL jest włączony:
wsl --status

# Zaktualizuj WSL:
wsl --update
```

### Błędy kompilacji w Docker:
```bash
# Wyczyść build i spróbuj ponownie:
docker run --rm -v "${PWD}:/workspace" -w /workspace/project/backend netsimcpp bash -c "rm -rf build && mkdir build && cd build && cmake .. && cmake --build . --target simple_traversal_test"
```

## Test REST API (test_traversal_api)

Jeśli chcesz również przetestować REST API:

```bash
# W kontenerze Docker lub WSL:
cd project/backend/build
cmake --build . --target test_traversal_api
./test_traversal_api &  # uruchom w tle

# W innym terminalu:
curl http://localhost:8081/test/traversal/help
```

Lub użyj gotowych skryptów testowych z `TEST_API_EXAMPLES.md`.
