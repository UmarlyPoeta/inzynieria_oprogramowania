# NetSimCPP - Network Simulator in C++

NetSimCPP is a comprehensive network simulator written in C++ that allows you to create, modify, and test network topologies, simulate packet transmission between nodes, and analyze network behavior. It includes advanced features like congestion control, TCP/UDP simulation, time-based events, packet fragmentation, and more. It's designed for learning network fundamentals, testing network algorithms, and simulating real-world network scenarios.

## Features

- **Node Management**: Create and manage different types of network nodes (Hosts, Routers, DummyNodes).
- **Topology Creation**: Connect nodes to form network topologies, with support for removal and disconnection.
- **Packet Simulation**: Simulate packet transmission with ping, traceroute, multicast, and advanced routing.
- **Advanced Networking Features**:
  - Link delays and bandwidth simulation
  - VLAN isolation
  - Firewall rules
  - Packet loss and statistics
  - Node failure simulation
  - Congestion control with packet queuing
  - TCP/UDP protocol simulation (including 3-way handshake)
  - Time-based simulation with event scheduling
  - Packet fragmentation and reassembly
- **Topology Management**: Export/import network topologies to/from JSON.
- **REST API**: Control the simulator via HTTP endpoints.
- **Extensible Design**: Easy to add new node types, protocols, and features.
- **Unit Tests**: Comprehensive test suite with GoogleTest (47 tests covering all implemented features).

## Architecture and Classes

### Core Classes

- **Packet**: Represents network packets with fields like source, destination, type, protocol, payload, delay, TTL, priority, TCP flags (seqNum, ackNum, syn, ack), and fragmentation fields (fragmentId, isLast). Supports fragmentation and reassembly.
- **Node**: Abstract base class for all network nodes. Defines interface for receiving packets. Includes packet queuing for congestion control.
  - **Host**: Represents end-user devices (e.g., computers). Receives packets and can send pings. Constructor: Host(name, address, port).
  - **Router**: Represents routing devices. Forwards packets based on routing tables. Supports adding routes and dynamic routing.
  - **DummyNode**: Simple node for testing purposes.
- **Network**: Manages the entire network graph. Handles adding/removing nodes, connections/disconnections, packet routing, link properties (delays, bandwidth, loss), VLANs, firewalls, and advanced simulations.
- **Engine**: Core simulation engine. Performs ping, traceroute, multicast operations using BFS and supports delays.

### Key Methods

- `Network::addNode<T>(args...)`: Creates and adds a node of type T (e.g., Host, Router).
- `Network::removeNode(name)`: Removes a node from the network.
- `Network::connect(nameA, nameB)`: Connects two nodes.
- `Network::disconnect(nameA, nameB)`: Disconnects two nodes.
- `Network::setLinkDelay(nameA, nameB, delay)`: Sets link delay in ms.
- `Network::setBandwidth(nameA, nameB, bw)`: Sets link bandwidth.
- `Network::setPacketLoss(nameA, nameB, prob)`: Sets packet loss probability.
- `Network::assignVLAN(name, vlanId)`: Assigns VLAN to a node.
- `Network::addFirewallRule(src, dst, protocol, allow)`: Adds firewall rule.
- `Network::exportToJson()`: Exports topology to JSON.
- `Network::importFromJson(json)`: Imports topology from JSON.
- `Network::failNode(name)`: Simulates node failure.
- `Network::enqueuePacket(name, pkt)`: Enqueues packet for congestion control.
- `Network::initiateTCPConnection(src, dst)`: Simulates TCP 3-way handshake.
- `Network::sendUDPPacket(src, dst, pkt)`: Sends UDP packet.
- `Network::advanceTime(ms)`: Advances simulation time.
- `Network::schedulePacketDelivery(pkt, delay)`: Schedules packet delivery.
- `Engine::ping(src, dst, path)`: Simulates ping with delays, returns success and path.
- `Engine::traceroute(src, dst, path)`: Returns full path to destination.
- `Engine::multicast(src, destinations)`: Sends packet to multiple destinations.
- `Packet::fragmentPacket(mtu)`: Fragments packet into smaller packets.
- `Packet::reassemblePacket(fragments)`: Reassembles fragments into original packet.
- `Router::addRoute(dst, nextHop)`: Adds route to routing table.
- `Router::exchangeRoutingInfo(other)`: Exchanges routing info for dynamic routing.

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
   cd inzynieria_oprogramowania
   ```

2. **Build the project**:
   ```bash
   cd project/backend
   mkdir -p build && cd build
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

The project includes 47 unit tests covering all core classes and implemented features, following TDD principles.

**Run tests**:
```bash
cd project/backend/build
ctest
# or
./netsim_tests
# Run single test: ./netsim_tests --gtest_filter=TestName
```

**Test Coverage**:
- Packet construction, fields, QoS priority, and fragmentation/reassembly
- Node creation, packet reception, and queuing
- Network topology management (add/remove nodes, connect/disconnect, find nodes)
- Engine ping/traceroute/multicast simulation with delays
- Advanced features: link delays, bandwidth, packet loss, VLAN isolation, firewall rules, export/import JSON, node failure, congestion control, TCP/UDP simulation, time-based events
- Error handling and edge cases

## Documentation

- [AGENTS.md](AGENTS.md): Development guidelines, build commands, and code style conventions for AI agents.
- [Architecture Overview](project/docs/architecture.md): Detailed description of the system architecture.
- [Diagrams](project/docs/diagrams.md): UML class diagrams, sequence diagrams, use cases, and more.

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

NetSimCPP to kompleksowy symulator sieci napisany w C++, który pozwala na tworzenie, modyfikowanie i testowanie topologii sieci, symulację transmisji pakietów między węzłami oraz analizę zachowania sieci. Zawiera zaawansowane funkcje jak kontrola przeciążenia, symulacja TCP/UDP, symulacja czasowa, fragmentacja pakietów i więcej. Jest przeznaczony do nauki podstaw sieci, testowania algorytmów sieciowych oraz symulacji scenariuszy rzeczywistych sieci.

## Funkcjonalności

- **Zarządzanie Węzłami**: Tworzenie i zarządzanie różnymi typami węzłów sieciowych (Hosty, Routery, DummyNodes).
- **Tworzenie Topologii**: Łączenie węzłów w topologie sieciowe, z wsparciem dla usuwania i rozłączania.
- **Symulacja Pakietów**: Symulacja transmisji pakietów z ping, traceroute, multicast i zaawansowanym routingiem.
- **Zaawansowane Funkcje Sieciowe**:
  - Symulacja opóźnień i przepustowości łączy
  - Izolacja VLAN
  - Reguły firewall
  - Utrata pakietów i statystyki
  - Symulacja awarii węzłów
  - Kontrola przeciążenia z kolejkowaniem pakietów
  - Symulacja protokołów TCP/UDP (włącznie z 3-way handshake)
  - Symulacja czasowa z planowaniem zdarzeń
  - Fragmentacja i reassemblacja pakietów
- **Zarządzanie Topologią**: Eksport/import topologii sieci do/z JSON.
- **REST API**: Sterowanie symulatorem przez endpoints HTTP.
- **Rozszerzalny Design**: Łatwe dodawanie nowych typów węzłów, protokołów i funkcji.
- **Testy Jednostkowe**: Kompletny zestaw testów z GoogleTest (47 testów pokrywających wszystkie zaimplementowane funkcje).

## Architektura i Klasy

### Główne Klasy

- **Packet**: Reprezentuje pakiety sieciowe z polami jak źródło, cel, typ, protokół, payload, opóźnienie, TTL, priorytet, flagi TCP (seqNum, ackNum, syn, ack) oraz pola fragmentacji (fragmentId, isLast). Wspiera fragmentację i reassemblację.
- **Node**: Abstrakcyjna klasa bazowa dla wszystkich węzłów sieciowych. Definiuje interfejs do odbioru pakietów. Zawiera kolejkowanie pakietów dla kontroli przeciążenia.
  - **Host**: Reprezentuje urządzenia końcowe (np. komputery). Odbiera pakiety i może wysyłać pingi. Konstruktor: Host(name, address, port).
  - **Router**: Reprezentuje urządzenia routingu. Przesyła pakiety na podstawie tabel routingu. Wspiera dodawanie tras i dynamiczne routing.
  - **DummyNode**: Prosty węzeł do celów testowych.
- **Network**: Zarządza całym grafem sieci. Obsługuje dodawanie/usuwanie węzłów, połączenia/rozłączenia, routing pakietów, właściwości łączy (opóźnienia, przepustowość, utrata), VLANy, firewalle i zaawansowane symulacje.
- **Engine**: Główny silnik symulacji. Wykonuje operacje ping, traceroute, multicast używając BFS i wspiera opóźnienia.

### Kluczowe Metody

- `Network::addNode<T>(args...)`: Tworzy i dodaje węzeł typu T (np. Host, Router).
- `Network::removeNode(name)`: Usuwa węzeł z sieci.
- `Network::connect(nameA, nameB)`: Łączy dwa węzły.
- `Network::disconnect(nameA, nameB)`: Rozłącza dwa węzły.
- `Network::setLinkDelay(nameA, nameB, delay)`: Ustawia opóźnienie łącza w ms.
- `Network::setBandwidth(nameA, nameB, bw)`: Ustawia przepustowość łącza.
- `Network::setPacketLoss(nameA, nameB, prob)`: Ustawia prawdopodobieństwo utraty pakietów.
- `Network::assignVLAN(name, vlanId)`: Przypisuje VLAN do węzła.
- `Network::addFirewallRule(src, dst, protocol, allow)`: Dodaje regułę firewall.
- `Network::exportToJson()`: Eksportuje topologię do JSON.
- `Network::importFromJson(json)`: Importuje topologię z JSON.
- `Network::failNode(name)`: Symuluje awarię węzła.
- `Network::enqueuePacket(name, pkt)`: Kolejkuje pakiet dla kontroli przeciążenia.
- `Network::initiateTCPConnection(src, dst)`: Symuluje 3-way handshake TCP.
- `Network::sendUDPPacket(src, dst, pkt)`: Wysyła pakiet UDP.
- `Network::advanceTime(ms)`: Przesuwa czas symulacji.
- `Network::schedulePacketDelivery(pkt, delay)`: Planuje dostarczenie pakietu.
- `Engine::ping(src, dst, path)`: Symuluje ping z opóźnieniami, zwraca sukces i ścieżkę.
- `Engine::traceroute(src, dst, path)`: Zwraca pełną ścieżkę do celu.
- `Engine::multicast(src, destinations)`: Wysyła pakiet do wielu celów.
- `Packet::fragmentPacket(mtu)`: Fragmentuje pakiet na mniejsze pakiety.
- `Packet::reassemblePacket(fragments)`: Reassembluje fragmenty w oryginalny pakiet.
- `Router::addRoute(dst, nextHop)`: Dodaje trasę do tabeli routingu.
- `Router::exchangeRoutingInfo(other)`: Wymienia informacje o routing dla dynamicznego routingu.

## Wymagania

- CMake >= 3.16
- Kompilator zgodny z C++17 (GCC, Clang)
- cpprestsdk (Microsoft C++ REST SDK)
- nlohmann/json
- GoogleTest (do testów)

## Instalacja i Uruchomienie

1. **Sklonuj repozytorium**:
   ```bash
   git clone https://github.com/UmarlyPoeta/inzynieria_oprogramowania.git
   cd inzynieria_oprogramowania
   ```

2. **Zbuduj projekt**:
   ```bash
   cd project/backend
   mkdir -p build && cd build
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

Projekt zawiera 47 testów jednostkowych pokrywających wszystkie główne klasy i zaimplementowane funkcje, zgodnie z zasadami TDD.

**Uruchom testy**:
```bash
cd project/backend/build
ctest
# lub
./netsim_tests
# Uruchom pojedynczy test: ./netsim_tests --gtest_filter=NazwaTestu
```

**Pokrycie Testów**:
- Konstrukcja pakietów, pól, priorytetu QoS oraz fragmentacji/reassemblacji
- Tworzenie węzłów, odbiór pakietów i kolejkowanie
- Zarządzanie topologią sieci (dodawanie/usuwanie węzłów, łączenie/rozłączanie, wyszukiwanie węzłów)
- Symulacja ping/traceroute/multicast w Engine z opóźnieniami
- Zaawansowane funkcje: opóźnienia łączy, przepustowość, utrata pakietów, izolacja VLAN, reguły firewall, eksport/import JSON, awarie węzłów, kontrola przeciążenia, symulacja TCP/UDP, zdarzenia czasowe
- Obsługa błędów i przypadków brzegowych

## Dokumentacja

- [AGENTS.md](AGENTS.md): Wytyczne rozwoju, komendy budowania i konwencje stylu kodu dla agentów AI.
- [Przegląd Architektury](project/docs/architecture.md): Szczegółowy opis architektury systemu.
- [Diagramy](project/docs/diagrams.md): Diagramy UML klas, sekwencji, przypadków użycia i więcej.

## Jak Kontrybuować

1. Zrób fork repozytorium i utwórz gałąź funkcji (`git checkout -b feature/nowa-funkcjonalnosc`).
2. Postępuj zgodnie ze stylem kodu w `AGENTS.md`.
3. Dodaj testy dla nowych funkcji używając podejścia TDD.
4. Uruchom testy i upewnij się, że przechodzą: `ctest`.
5. Prześlij Pull Request z opisem zmian.

## Future Enhancements

- Implement routing protocols (OSPF, BGP) with full dynamic routing.
- Add wireless network simulation with interference and range.
- Integrate cloud and IoT device simulations.
- Add performance metrics collection and monitoring.
- Develop a web UI for topology visualization.
- Integrate with monitoring tools (Grafana, Prometheus).

## Licencja

Projekt jest dostępny na licencji MIT.
