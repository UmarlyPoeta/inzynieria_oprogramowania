# NetSimCPP - Network Simulator in C++

NetSimCPP is a simple network simulator written in C++ that allows you to create, modify, and test network topologies, simulate packet transmission between nodes, and analyze network behavior. It's designed for learning network fundamentals and testing network algorithms.

## Features

- **Node Management**: Create and manage different types of network nodes (Hosts, Routers).
- **Topology Creation**: Connect nodes to form network topologies.
- **Packet Simulation**: Simulate packet transmission with ping and traceroute.
- **REST API**: Control the simulator via HTTP endpoints.
- **Extensible Design**: Easy to add new node types, protocols, and features.
- **Unit Tests**: Comprehensive test suite with GoogleTest.

## Architecture and Classes

### Core Classes

- **Packet**: Represents network packets with fields like source, destination, type, protocol, payload, delay, and TTL.
- **Node**: Abstract base class for all network nodes. Defines interface for receiving packets.
  - **Host**: Represents end-user devices (e.g., computers). Receives packets and can send pings.
  - **Router**: Represents routing devices. Forwards packets based on routing tables.
- **Network**: Manages the entire network graph. Handles adding nodes, connections, and packet routing.
- **Engine**: Core simulation engine. Performs ping and traceroute operations using BFS.

### Key Methods

- `Network::addNode<T>(args...)`: Creates and adds a node of type T (e.g., Host, Router).
- `Network::connect(nameA, nameB)`: Connects two nodes.
- `Engine::ping(src, dst, path)`: Simulates ping, returns success and path.
- `Engine::traceroute(src, dst, path)`: Returns full path to destination.

## Requirements

- CMake >= 3.16
- C++17 compatible compiler (GCC, Clang)
- cpprestsdk (Microsoft C++ REST SDK)
- nlohmann/json
- GoogleTest (for tests)

## Installation and Running

1. **Clone the repository**:
   ```bash
   git clone https://github.com/UmarlyPoeta/inzynieria_oprogramowania.git
   cd netsimcpp/project
   ```

2. **Build the project**:
   ```bash
   cd backend
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```

3. **Run the simulator**:
   ```bash
   ./netsim
   ```
   The server starts on `http://0.0.0.0:8080`.

## API Endpoints

- `GET /status`: Check server status.
- `POST /addNode`: Add a new node (JSON: `{"name": "A", "ip": "10.0.0.1"}`).
- `POST /ping`: Ping between nodes (JSON: `{"src": "A", "dst": "B"}`).

Example:
```bash
curl -X POST -H "Content-Type: application/json" -d '{"name":"A","ip":"10.0.0.1"}' http://localhost:8080/addNode
curl -X POST -H "Content-Type: application/json" -d '{"src":"A","dst":"B"}' http://localhost:8080/ping
```

## Testing

The project includes 31 unit tests covering all core classes and TDD tests for planned features.

**Run tests**:
```bash
cd backend/build
ctest
# or
./netsim_tests
```

**Test Coverage**:
- Packet construction and fields
- Node creation and packet reception
- Network topology management (add, connect, find nodes)
- Engine ping/traceroute simulation
- Error handling and edge cases
- TDD tests for future features (VLAN, firewall, etc.)

## Contributing

1. Fork the repository and create a feature branch (`git checkout -b feature/new-functionality`).
2. Follow the code style in `AGENTS.md`.
3. Add tests for new features using TDD approach.
4. Run tests and ensure they pass: `ctest`.
5. Submit a Pull Request with a description of changes.

## License

This project is licensed under the MIT License.

---

# NetSimCPP - Symulator Sieci w C++ (Wersja Polska)

NetSimCPP to prosty symulator sieci napisany w C++, który pozwala na tworzenie, modyfikowanie i testowanie topologii sieci, symulację transmisji pakietów między węzłami oraz analizę zachowania sieci. Jest przeznaczony do nauki podstaw sieci oraz testowania algorytmów sieciowych.

## Funkcjonalności

- **Zarządzanie Węzłami**: Tworzenie i zarządzanie różnymi typami węzłów sieciowych (Hosty, Routery).
- **Tworzenie Topologii**: Łączenie węzłów w topologie sieciowe.
- **Symulacja Pakietów**: Symulacja transmisji pakietów z ping i traceroute.
- **REST API**: Sterowanie symulatorem przez endpoints HTTP.
- **Rozszerzalny Design**: Łatwe dodawanie nowych typów węzłów, protokołów i funkcji.
- **Testy Jednostkowe**: Kompletny zestaw testów z GoogleTest.

## Architektura i Klasy

### Główne Klasy

- **Packet**: Reprezentuje pakiety sieciowe z polami jak źródło, cel, typ, protokół, payload, opóźnienie i TTL.
- **Node**: Abstrakcyjna klasa bazowa dla wszystkich węzłów sieciowych. Definiuje interfejs do odbioru pakietów.
  - **Host**: Reprezentuje urządzenia końcowe (np. komputery). Odbiera pakiety i może wysyłać pingi.
  - **Router**: Reprezentuje urządzenia routingu. Przesyła pakiety na podstawie tabel routingu.
- **Network**: Zarządza całym grafem sieci. Obsługuje dodawanie węzłów, połączeń i routingu pakietów.
- **Engine**: Główny silnik symulacji. Wykonuje operacje ping i traceroute używając BFS.

### Kluczowe Metody

- `Network::addNode<T>(args...)`: Tworzy i dodaje węzeł typu T (np. Host, Router).
- `Network::connect(nameA, nameB)`: Łączy dwa węzły.
- `Engine::ping(src, dst, path)`: Symuluje ping, zwraca sukces i ścieżkę.
- `Engine::traceroute(src, dst, path)`: Zwraca pełną ścieżkę do celu.

## Wymagania

- CMake >= 3.16
- Kompilator zgodny z C++17 (GCC, Clang)
- cpprestsdk (Microsoft C++ REST SDK)
- nlohmann/json
- GoogleTest (do testów)

## Instalacja i Uruchomienie

1. **Sklonuj repozytorium**:
   ```bash
   git clone <adres-repozytorium>
   cd netsimcpp/project
   ```

2. **Zbuduj projekt**:
   ```bash
   cd backend
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```

3. **Uruchom symulator**:
   ```bash
   ./netsim
   ```
   Serwer uruchamia się na `http://0.0.0.0:8080`.

## Endpoints API

- `GET /status`: Sprawdź status serwera.
- `POST /addNode`: Dodaj nowy węzeł (JSON: `{"name": "A", "ip": "10.0.0.1"}`).
- `POST /ping`: Ping między węzłami (JSON: `{"src": "A", "dst": "B"}`).

Przykład:
```bash
curl -X POST -H "Content-Type: application/json" -d '{"name":"A","ip":"10.0.0.1"}' http://localhost:8080/addNode
curl -X POST -H "Content-Type: application/json" -d '{"src":"A","dst":"B"}' http://localhost:8080/ping
```

## Testowanie

Projekt zawiera 31 testów jednostkowych pokrywających wszystkie główne klasy oraz testy TDD dla planowanych funkcji.

**Uruchom testy**:
```bash
cd backend/build
ctest
# lub
./netsim_tests
```

**Pokrycie Testów**:
- Konstrukcja pakietów i pól
- Tworzenie węzłów i odbiór pakietów
- Zarządzanie topologią sieci (dodawanie, łączenie, wyszukiwanie węzłów)
- Symulacja ping/traceroute w Engine
- Obsługa błędów i przypadków brzegowych
- Testy TDD dla przyszłych funkcji (VLAN, firewall, itp.)

## Jak Kontrybuować

1. Zrób fork repozytorium i utwórz gałąź funkcji (`git checkout -b feature/nowa-funkcjonalnosc`).
2. Postępuj zgodnie ze stylem kodu w `AGENTS.md`.
3. Dodaj testy dla nowych funkcji używając podejścia TDD.
4. Uruchom testy i upewnij się, że przechodzą: `ctest`.
5. Prześlij Pull Request z opisem zmian.

## TODO

- [ ] Zaimplementuj usuwanie węzłów (`Network::removeNode`)
- [ ] Dodaj rozłączanie łączy (`Network::disconnect`)
- [ ] Symuluj opóźnienia i przepustowość łączy
- [ ] Dodaj wsparcie dla VLAN do izolacji sieci
- [ ] Zaimplementuj reguły firewall
- [ ] Dodaj statystyki pakietów i monitorowanie
- [ ] Eksport/import topologii sieci do/z JSON
- [ ] Symulacja awarii węzłów
- [ ] Zaawansowane protokoły routingu (OSPF, RIP)
- [ ] Web UI do wizualizacji topologii
- [ ] Integracja z narzędziami monitorującymi (Grafana, Prometheus)

## Licencja

Projekt jest dostępny na licencji MIT.
