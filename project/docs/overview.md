# NetSimCPP - Overview

## Co już jest w kodzie

Projekt NetSimCPP to symulator sieci komputerowej napisany w C++17. Zawiera podstawowe komponenty:

### Klasy podstawowe
- **Packet**: Reprezentuje pakiet danych z polami src, dest, type, protocol, payload. Obsługuje priorytety (QoS).
- **Node**: Abstrakcyjna klasa węzła sieciowego z nazwą i IP. Ma metodę `receivePacket`.
- **DummyNode**: Prosta implementacja Node dla testów.
- **Network**: Zarządza węzłami i połączeniami. Metody: addNode, findByName, connect, getNeighbors.
- **Engine**: Silnik symulacji. Obecnie obsługuje ping między węzłami.
- **Router**: Węzeł routujący, dziedziczy po Node.
- **Host**: Węzeł końcowy z adresem i portem, dziedziczy po Node.

### Infrastruktura
- CMakeLists.txt: Konfiguracja budowania z cpprestsdk i nlohmann_json.
- GoogleTest: Framework testów jednostkowych.
- cpprestsdk: Dla przyszłego API REST.

### Funkcjonalności zaimplementowane
- Tworzenie węzłów i połączeń.
- Ping między węzłami (Engine::ping).
- Odbiór pakietów przez węzły.
- Podstawowe testy dla wszystkich klas.

## Czego brakuje według testów (TDD - Test-Driven Development)

Większość testów w `test_main.cpp` to specyfikacje dla funkcji, które jeszcze nie są zaimplementowane. Oto lista brakujących funkcjonalności:

### Routing i komunikacja
- **Routing w Router**: Tablica routingu (addRoute, getNextHop), forwardPacket.
- **Host wysyłanie pakietów**: sendPacket do Routera.
- **Network sendPacket**: Symulacja transmisji pakietów przez sieć.
- **Multicast**: Wysyłanie do wielu odbiorców (Engine::multicast).
- **Load Balancing**: Router wybiera ścieżkę na podstawie obciążenia.
- **Dynamic Routing**: Aktualizacja tras (OSPF/BGP symulacja).
- **Routing Protocols**: Wymiana informacji między routerami.

### Symulacja protokołów
- **TCP Simulation**: Handshake (SYN, SYN-ACK, ACK), retransmisje.
- **UDP Simulation**: Bezpołączeniowe wysyłanie pakietów.

### Zaawansowane funkcje sieciowe
- **Packet Fragmentation**: Dzielenie dużych pakietów na fragmenty (MTU), reassemble.
- **Wireless Networks**: Połączenia bezprzewodowe z zasięgiem i interferencjami.
- **VLAN**: Izolacja VLAN dla węzłów.
- **Firewall**: Reguły blokowania/zezwalania ruchu.
- **Bandwidth**: Symulacja przepustowości łączy.
- **Packet Loss**: Szansa na utratę pakietów.
- **Congestion Control**: Kolejki pakietów, wykrywanie przeciążenia.
- **Time-Based Simulation**: Symulacja czasu rzeczywistego (advanceTime, schedulePacketDelivery).
- **Node Failure**: Symulacja awarii węzłów.
- **Traceroute**: Pełna ścieżka pakietów (rozszerzenie ping).

### Zarządzanie siecią
- **RemoveNode**: Usuwanie węzłów z sieci.
- **DisconnectNodes**: Rozłączanie połączeń.
- **LinkDelay**: Opóźnienia między węzłami.
- **PacketStatistics**: Liczniki pakietów.
- **ExportImportTopology**: Serializacja/deserializacja sieci do JSON.
- **Network Monitoring**: Statystyki ruchu.
- **Performance Metrics**: Latency, throughput, packet loss rate (częściowo zakomentowane).

### Specjalistyczne funkcje (zakomentowane testy)
- **Cloud Integration**: Autoscaling węzłów chmurowych.
- **IoT Devices**: Symulacja urządzeń IoT z baterią.

## Workflow całego projektu

1. **Rozwój**: Pisanie kodu w `backend/src/core/`. Dodawanie nowych klas/funkcji zgodnie z testami TDD.
2. **Budowa**: `cd project/backend && mkdir -p build && cd build && cmake .. && cmake --build .`
3. **Testy**: `cd project/backend/build && ctest` lub `./netsim_tests --gtest_filter=TestName`.
4. **Uruchomienie**: `./netsim` dla symulacji (główny program w `main.cpp`).
5. **API**: Planowane dodanie REST API z cpprestsdk dla dashboardu frontendowego.
6. **Dokumentacja**: Aktualizacja UML w `docs/UML/`, architektury w `docs/architecture.md`.

### Przykład workflow dla nowej funkcji
- Napisz test w `test_main.cpp` (TDD).
- Zaimplementuj funkcję w odpowiedniej klasie.
- Zbuduj i uruchom testy.
- Jeśli test przechodzi, funkcja gotowa.

## Kompendium

- **Język**: C++17, standardy Google (nazewnictwo, style).
- **Biblioteki**: cpprestsdk (REST), nlohmann_json (JSON), GTest (testy).
- **Architektura**: Obiektowa, klasy Node dziedziczą po abstrakcyjnej Node.
- **Symulacja**: Obecnie statyczna (bez czasu), planowana czasowa.
- **Rozszerzalność**: Łatwe dodawanie nowych typów węzłów (Router, Host).
- **Bezpieczeństwo**: Brak wyjątków, walidacja wejść.
- **Testy**: Pełne pokrycie TDD, skupiające się na edge cases.

Aby dodać brakujące funkcje, zacznij od implementacji testów w kolejności priorytetu (np. routing, potem protokoły).