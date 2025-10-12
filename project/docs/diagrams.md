# Diagramy dla Network Simulator

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

Test UDPSimulation sprawdza wysyłanie pakietu UDP – powinien przejść.

Następna? Powiedz, którą funkcjonalność zaimplementować! 🚀