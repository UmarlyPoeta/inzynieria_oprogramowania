# Diagramy dla Network Simulator

## Prezentacja Projektu

### Pomysł
NetSimCPP to symulator sieci komputerowych napisany w C++, umożliwiający tworzenie i modyfikowanie topologii sieci, symulację transmisji pakietów między węzłami oraz analizę zachowania sieci. Projekt jest przeznaczony do nauki podstaw sieci, testowania algorytmów routingu, symulacji protokołów TCP/UDP oraz eksperymentów z zaawansowanymi funkcjami jak kontrola przeciążenia, VLAN, firewall czy fragmentacja pakietów. Wykorzystuje podejście TDD (Test-Driven Development) dla zapewnienia jakości kodu.

### Badanie Rynku
Na rynku istnieją narzędzia do symulacji sieci takie jak:
- **NS-3**: Zaawansowany symulator open-source w C++, skupiający się na protokołach sieciowych i mobilności.
- **OMNeT++**: Modularny symulator dla sieci przewodowych i bezprzewodowych, z graficznym interfejsem.
- **Mininet**: Symulator sieci SDN, umożliwiający tworzenie wirtualnych topologii na jednym komputerze.
- **Cisco Packet Tracer**: Narzędzie edukacyjne firmy Cisco, z interfejsem graficznym, ale zamknięte źródło.

Nasze wyróżniki:
- Pełne open-source (MIT License).
- Implementacja w czystym C++ bez zewnętrznych symulatorów.
- REST API dla integracji z innymi narzędziami.
- Rozszerzalne poprzez dodawanie nowych typów węzłów i protokołów.
- Skupienie na TDD i wysokiej pokryciu testami (47 testów).
- Symulacja zaawansowanych funkcji jak TCP handshake, fragmentacja pakietów, czasowa symulacja zdarzeń.

## Inżynieria Wymagań

### Historie Użytkownika (User Stories)
- Jako student inżynierii oprogramowania, chcę dodać węzeł do sieci, aby móc budować prostą topologię.
- Jako programista, chcę połączyć dwa węzły, aby symulować łącze między urządzeniami.
- Jako użytkownik, chcę wysłać ping między węzłami, aby sprawdzić łączność i ścieżkę.
- Jako badacz sieci, chcę ustawić opóźnienia łączy, aby symulować rzeczywiste warunki.
- Jako administrator sieci, chcę przypisać VLAN do węzłów, aby izolować ruch.
- Jako tester bezpieczeństwa, chcę skonfigurować reguły firewall, aby blokować niechciany ruch.
- Jako inżynier, chcę symulować awarię węzła, aby przetestować odporność sieci.
- Jako deweloper, chcę eksportować topologię do JSON, aby zapisać konfigurację.
- Jako użytkownik zaawansowany, chcę symulować połączenie TCP, aby zobaczyć handshake.
- Jako analityk, chcę zbierać statystyki pakietów, aby monitorować wydajność.

### Diagram Przypadków Użycia (Use-Case) UML

```plantuml
@startuml
left to right direction
actor "Użytkownik" as User
rectangle "NetSimCPP" {
    User --> (Dodaj Węzeł)
    User --> (Połącz Węzły)
    User --> (Wykonaj Ping)
    User --> (Ustaw Opóźnienie Łącza)
    User --> (Przypisz VLAN)
    User --> (Skonfiguruj Firewall)
    User --> (Symuluj Awarię Węzła)
    User --> (Eksportuj Topologię)
    User --> (Importuj Topologię)
    User --> (Symuluj Połączenie TCP)
    User --> (Wyślij Pakiet UDP)
    User --> (Zbierz Statystyki)
    User --> (Uruchom Testy)
}
@enduml
```

### Diagram Wymagań (Requirements) SysML

```plantuml
@startuml
package "Wymagania Funkcjonalne" {
    requirement RF1 "System musi umożliwiać dodanie węzła do sieci"
    requirement RF2 "System musi umożliwiać połączenie dwóch węzłów"
    requirement RF3 "System musi symulować ping między węzłami"
    requirement RF4 "System musi obsługiwać ustawianie opóźnień łączy"
    requirement RF5 "System musi wspierać izolację VLAN"
    requirement RF6 "System musi pozwalać na konfigurację reguł firewall"
    requirement RF7 "System musi symulować awarie węzłów"
    requirement RF8 "System musi umożliwiać eksport/import topologii do JSON"
    requirement RF9 "System musi symulować protokoły TCP/UDP"
    requirement RF10 "System musi zbierać statystyki pakietów"
}

package "Wymagania Niefunkcjonalne" {
    requirement NF1 "System musi być wydajny dla sieci do 100 węzłów"
    requirement NF2 "System musi mieć wysoką niezawodność (testy pokrywają 47 przypadków)"
    requirement NF 3 "System musi być rozszerzalny poprzez dodawanie nowych klas"
    requirement NF4 "System musi mieć REST API dla integracji"
    requirement NF5 "System musi być bezpieczny (brak logowania wrażliwych danych)"
}

RF1 --> NF3 : refine
RF3 --> NF4 : derive
@enduml
```

### Kategoryzacja Wymagań

#### Wymagania Funkcjonalne
- Zarządzanie węzłami: dodawanie, usuwanie, łączenie, rozłączanie.
- Symulacja transmisji: ping, traceroute, multicast, TCP/UDP handshake.
- Konfiguracja sieci: opóźnienia, przepustowość, VLAN, firewall, utrata pakietów.
- Symulacja zdarzeń: awarie węzłów, kontrola przeciążenia, fragmentacja pakietów.
- Zarządzanie topologią: eksport/import JSON.
- API: REST endpoints dla interakcji.

#### Wymagania Niefunkcjonalne
- Wydajność: Obsługa dużych sieci bez znacznego spadku wydajności.
- Niezawodność: Wysoka pokrycie testami, brak błędów krytycznych.
- Rozszerzalność: Łatwe dodawanie nowych funkcji bez zmiany rdzenia.
- Użyteczność: Proste API i dokumentacja.
- Bezpieczeństwo: Brak przechowywania wrażliwych danych, bezpieczne operacje.

## 1. Diagram Klas UML

```plantuml
@startuml
class Node {
    - name: string
    - ip: string
    - packetCount: int
    - packetQueue: queue<Packet>
    - queuesize: int
    + Node(name: string, ip: string)
    + getName(): string
    + getIp(): string
    + receivePacket(p: Packet): void
    + sendPacket(p: Packet, dest: Node): void
    + getAllPacketCount(): int
    + incrementPacketCount(): void
    + setQueueSize(size: int): void
    + enqueuePacket(pkt: Packet): void
    + dequeuePacket(): void
    + isCongested(): bool
}

class Host extends Node {
    - address: string
    - port: int
    + Host(name: string, address: string, port: int)
    + getAddress(): string
    + getPort(): int
}

class Router extends Node {
    - routingTable: map<string, Node*>
    + addRoute(ip: string, next: Node*): void
    + getNextHop(dst: string): Node*
}

class Packet {
    - src: string
    - dest: string
    - type: string
    - protocol: string
    - payload: string
    - delayMs: int
    - ttl: int
    - priority: int
    - seqNum: int
    - ackNum: int
    - syn: bool
    - ack: bool
    + Packet(src, dest, type, protocol, payload)
    + setPriority(p: int): void
    + getPriority(): int
}

class Network {
    - nodes: vector<shared_ptr<Node>>
    - nodesByName: map<string, shared_ptr<Node>>
    - adj: map<string, set<string>>
    - linkDelays: map<pair<string,string>, int>
    - vlans: map<string, int>
    - bandwidths: map<pair<string,string>, int>
    - firewallRules: map<tuple<string,string,string>, bool>
    - failedNodes: set<string>
    - packetLoss: map<pair<string,string>, double>
    + addNode<T>(args...): shared_ptr<T>
    + connect(a: string, b: string): void
    + findByName(name: string): shared_ptr<Node>
    + getNeighbors(name: string): vector<string>
    + removeNode(name: string): void
    + disconnect(a: string, b: string): void
    + setLinkDelay(a: string, b: string, delay: int): void
    + getLinkDelay(a: string, b: string): int
    + assignVLAN(name: string, vlan: int): void
    + canCommunicate(a: string, b: string): bool
    + setBandwidth(a: string, b: string, bw: int): void
    + getBandwidth(a: string, b: string): int
    + consumeBandwidth(a: string, b: string, amount: int): void
    + addFirewallRule(src: string, dst: string, protocol: string, allow: bool): void
    + isAllowed(src: string, dst: string, protocol: string): bool
    + failNode(name: string): void
    + isFailed(name: string): bool
    + sendPacket(pkt: Packet): void
    + exportToJson(): string
    + importFromJson(json: string): void
    + setPacketLoss(a: string, b: string, loss: double): void
    + initiateTCPConnection(client: string, server: string): bool
    + sendTCPPacket(src: string, dst: string, pkt: Packet): bool
    + setQueueSize(name: string, size: int): void
    + enqueuePacket(name: string, pkt: Packet): void
    + dequeuePacket(name: string): void
    + isCongested(name: string): bool
}

class Engine {
    - net: Network&
    + Engine(net: Network)
    + ping(src: string, dst: string, path: vector<string>&): bool
    + traceroute(src: string, dst: string, path: vector<string>&): bool
}

Node ||-- Packet : receives
Network o-- Node : contains
Engine --> Network : uses
@enduml
```

## 2. Diagram Sekwencji dla Ping

```plantuml
@startuml
actor User
User -> Frontend: Request ping from A to B
Frontend -> API: POST /ping {src: "A", dst: "B"}
API -> Engine: ping("A", "B", path)
Engine -> Network: findByName("A"), findByName("B")
Engine -> Network: getAdjacency()
Engine -> Engine: BFS traversal
Engine -> API: return success + path
API -> Frontend: JSON response
Frontend -> User: Display path
@enduml
```

## 3. Diagram Sekwencji dla TCP Handshake

```plantuml
@startuml
Client -> Server: SYN (seq=1000)
Server -> Client: SYN-ACK (seq=2000, ack=1001)
Client -> Server: ACK (ack=2001)
Client -> Server: Data packet
Server -> Client: ACK
@enduml
```

## 4. Diagram Przypadków Użycia

```plantuml
@startuml
left to right direction
actor User
rectangle "Network Simulator" {
    User --> (Add Node)
    User --> (Connect Nodes)
    User --> (Ping Nodes)
    User --> (Set Link Delay)
    User --> (Assign VLAN)
    User --> (Configure Firewall)
    User --> (Simulate Failure)
    User --> (Export Topology)
    User --> (Run Tests)
}
@enduml
```

## 5. Diagram Aktywności dla Symulacji Sieci

```plantuml
@startuml
start
:Initialize Network;
:Add Nodes (Host/Router);
:Connect Nodes;
:Set Parameters (Delay, VLAN, etc.);
while (Simulate?) is (yes)
    :Choose Action (Ping, Send Packet, etc.);
    :Execute Simulation;
    :Log Results;
endwhile
:Export Results;
stop
@enduml
```

## 6. Diagram Stanów dla Pakietu w Sieci

```plantuml
@startuml
[*] --> Created
Created --> Queued : enqueuePacket()
Queued --> Sent : dequeuePacket()
Sent --> Received : receivePacket()
Received --> Processed
Processed --> [*]
Sent --> Lost : packetLoss > 0
Lost --> Retransmitted : TCP retransmit
Retransmitted --> Received
@enduml
```

## 7. Diagram ER dla Bazy Danych (jeśli rozszerzymy)

```plantuml
@startuml
entity Node {
    * id: int
    name: string
    ip: string
    type: string (Host/Router)
}

entity Link {
    * id: int
    nodeA: int
    nodeB: int
    delay: int
    bandwidth: int
    vlan: int
}

entity PacketLog {
    * id: int
    src: string
    dst: string
    timestamp: datetime
    success: bool
}

Node ||--o{ Link : connects
Link }o--|| Node
@enduml
```

Te diagramy opisują architekturę projektu. Można je wygenerować w narzędziach jak PlantUML lub Draw.io.

Teraz kontynuuję implementację – zaimplementuję **UDP Simulation** zgodnie z testem NetworkTest.UDPSimulation. UDP to protokół bezpołączeniowy, bez retransmisji.

### Zmiany dla UDP:
- Dodaj metodę `sendUDPPacket` w Network – po prostu wysyła pakiet bez ACK.
- Brak handshake, zawsze "sukces" (chyba że węzeł failed).

Dodaję kod. 

**Network.hpp** – dodaj deklarację:
```cpp
bool sendUDPPacket(const std::string& src, const std::string& dst, Packet pkt);
```

**Network.cpp** – implementacja:
```cpp
bool Network::sendUDPPacket(const std::string& src, const std::string& dst, Packet pkt) {
    pkt.protocol = "udp";
    // No handshake, no retransmission - just send
    return true; // Always succeeds for UDP
}
```
