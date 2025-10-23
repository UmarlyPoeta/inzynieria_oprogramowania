#include <gtest/gtest.h>
#include "core/Node.hpp"
#include "core/Packet.hpp"
#include "core/Network.hpp"
#include "core/Engine.hpp"
#include "core/Host.hpp"
#include "core/Router.hpp"

// DummyNode is defined in Network.hpp

// Test klasy Packet
// Test sprawdza podstawową konstrukcję pakietu i dostęp do pól
// Weryfikuje, czy wszystkie pola (źródło, cel, typ, protokół, payload) są prawidłowo ustawiane
TEST(PacketTest, ConstructionAndFields) {
    Packet pkt("A", "B", "data", "tcp", "Hello");
    EXPECT_EQ(pkt.src, "A");        // Sprawdź źródło pakietu
    EXPECT_EQ(pkt.dest, "B");       // Sprawdź cel pakietu
    EXPECT_EQ(pkt.payload, "Hello"); // Sprawdź zawartość payload
}

// Test sprawdza obsługę pustego payload w pakiecie
// Weryfikuje, czy pakiet może mieć pustą zawartość danych
TEST(PacketTest, EmptyPayload) {
    Packet pkt("A", "B", "data", "tcp", "");
    EXPECT_EQ(pkt.payload, ""); // Payload powinien być pusty
}

// Test sprawdza obsługę dużych payload w pakietach
// Weryfikuje, czy pakiet może zawierać duże ilości danych (1000 znaków)
TEST(PacketTest, LongPayload) {
    std::string longPayload(1000, 'x'); // Utwórz duży payload
    Packet pkt("A", "B", "data", "tcp", longPayload);
    EXPECT_EQ(pkt.payload, longPayload); // Sprawdź, czy cały payload został zachowany
}

// Test klasy Node
// Test sprawdza podstawową konstrukcję węzła sieciowego
// Weryfikuje, czy nazwa i adres IP są prawidłowo ustawiane podczas tworzenia węzła
TEST(NodeTest, Construction) {
    DummyNode node("N1", "10.0.0.1");
    EXPECT_EQ(node.getName(), "N1");     // Sprawdź nazwę węzła
    EXPECT_EQ(node.getIp(), "10.0.0.1"); // Sprawdź adres IP węzła
}

// Test sprawdza funkcjonalność odbioru pakietów przez węzeł
// Weryfikuje, czy węzeł może prawidłowo odbierać i przetwarzać pakiety
TEST(NodeTest, ReceivePacket) {
    DummyNode node("N2", "10.0.0.2");
    Packet pkt("A", "N2", "data", "tcp", "payload"); // Utwórz pakiet skierowany do węzła N2
    node.receivePacket(pkt); // Wywołaj metodę odbioru pakietu
    EXPECT_TRUE(node.received); // Sprawdź, czy pakiet został odebrany (flaga received ustawiona)
}

TEST(NodeTest, EmptyName) {
    DummyNode node("", "10.0.0.3");
    EXPECT_EQ(node.getName(), "");
}

TEST(NodeTest, EmptyIp) {
    DummyNode node("N3", "");
    EXPECT_EQ(node.getIp(), "");
}

// Test klasy Network
// Test sprawdza dodawanie węzła do sieci i jego wyszukiwanie
// Weryfikuje, czy węzeł może być dodany do sieci i znaleziony po nazwie
TEST(NetworkTest, AddAndFindNode) {
    Network net;
    auto node = net.addNode<DummyNode>("N3", "10.0.0.3"); // Dodaj węzeł do sieci
    auto found = net.findByName("N3"); // Znajdź węzeł po nazwie
    EXPECT_EQ(found, node); // Sprawdź, czy znaleziony węzeł jest tym samym obiektem
}

// Test sprawdza obsługę próby dodania węzła o duplikowanej nazwie
// Weryfikuje, czy system rzuca wyjątek przy próbie dodania węzła o istniejącej nazwie
TEST(NetworkTest, AddDuplicateNodeThrows) {
    Network net;
    net.addNode<DummyNode>("N4", "10.0.0.4"); // Dodaj pierwszy węzeł
    // Próba dodania węzła o tej samej nazwie powinna rzucić wyjątek
    EXPECT_THROW(net.addNode<DummyNode>("N4", "10.0.0.4"), std::runtime_error);
}

TEST(NetworkTest, ConnectNodes) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.5");
    net.addNode<DummyNode>("B", "10.0.0.6");
    net.connect("A", "B");
    auto neighbors = net.getNeighbors("A");
    EXPECT_EQ(neighbors.size(), 1);
    EXPECT_EQ(neighbors[0], "B");
}

TEST(NetworkTest, FindNonExistentNode) {
    Network net;
    EXPECT_THROW(net.findByName("NonExistent"), std::runtime_error);
}

TEST(NetworkTest, ConnectSameNodeThrows) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.12");
    EXPECT_THROW(net.connect("A", "A"), std::runtime_error);
}

TEST(NetworkTest, ConnectUnknownNodesThrows) {
    Network net;
    EXPECT_THROW(net.connect("A", "B"), std::runtime_error);
}

// Test klasy Engine (ping)
// Test sprawdza pomyślne wykonanie ping między połączonymi węzłami
// Weryfikuje, czy ping zwraca sukces i prawidłową ścieżkę dla połączonych węzłów
TEST(EngineTest, PingSuccess) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.7"); // Dodaj węzeł źródłowy
    net.addNode<DummyNode>("B", "10.0.0.8"); // Dodaj węzeł docelowy
    net.connect("A", "B"); // Połącz węzły
    Engine engine(net);
    std::vector<std::string> path; // Kontener na ścieżkę
    bool ok = engine.ping("A", "B", path); // Wykonaj ping
    EXPECT_TRUE(ok); // Ping powinien się powieść
    EXPECT_EQ(path.size(), 2); // Ścieżka powinna zawierać 2 węzły
    EXPECT_EQ(path[0], "A"); // Pierwszy węzeł to źródło
    EXPECT_EQ(path[1], "B"); // Drugi węzeł to cel
}

// Test sprawdza niepowodzenie ping między niepołączonymi węzłami
// Weryfikuje, czy ping prawidłowo wykrywa brak połączenia między węzłami
TEST(EngineTest, PingFail) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.9"); // Dodaj węzeł źródłowy
    net.addNode<DummyNode>("B", "10.0.0.10"); // Dodaj węzeł docelowy
    // Brak połączenia między węzłami
    Engine engine(net);
    std::vector<std::string> path; // Kontener na ścieżkę
    bool ok = engine.ping("A", "B", path); // Wykonaj ping
    EXPECT_FALSE(ok); // Ping powinien się nie powieść
}

TEST(EngineTest, PingSameNode) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.13");
    Engine engine(net);
    std::vector<std::string> path;
    bool ok = engine.ping("A", "A", path);
    EXPECT_TRUE(ok);
    EXPECT_EQ(path.size(), 1);
    EXPECT_EQ(path[0], "A");
}

TEST(EngineTest, PingUnknownNode) {
    Network net;
    Engine engine(net);
    std::vector<std::string> path;
    bool ok = engine.ping("A", "B", path);
    EXPECT_FALSE(ok);
}

// Test klasy Router
TEST(RouterTest, Construction) {
    Router router("R1", "192.168.0.1");
    EXPECT_EQ(router.getName(), "R1");
    EXPECT_EQ(router.getIp(), "192.168.0.1");
}

TEST(RouterTest, ReceivePacket) {
    Router router("R2", "192.168.0.2");
    Packet pkt("A", "B", "data", "tcp", "payload");
    router.receivePacket(pkt); // Should not throw
}

// Test klasy Host
TEST(HostTest, Construction) {
    Host host("H1", "192.168.0.2", 8080);
    EXPECT_EQ(host.getAddress(), "192.168.0.2");
    EXPECT_EQ(host.getPort(), 8080);
}

TEST(HostTest, ReceivePacket) {
    Host host("H2", "192.168.0.3", 8080);
    Packet pkt("A", "H2", "data", "tcp", "payload");
    host.receivePacket(pkt); // Should not throw
}

// TDD Tests for new features (implement these functions to make tests pass)

// Nowe pomysły na funkcjonalności - dodaj implementację, aby testy przeszły

// 11. Routing w Router: Dodaj tablicę routingu, metody addRoute, getNextHop
//    - addRoute: dodaj trasę dst -> nextHop
//    - getNextHop: zwróć następny węzeł dla dst
TEST(RouterTest, AddRoute) {
    Network net;
    auto r1 = net.addNode<Router>("R1", "192.168.0.1");
    auto r2 = net.addNode<Router>("R2", "192.168.0.2");
    Router* router = dynamic_cast<Router*>(r1.get());
    router->addRoute("10.0.0.1", r2.get());
    EXPECT_EQ(router->getNextHop("10.0.0.1"), r2.get());
}

TEST(RouterTest, ForwardPacket) {
    Network net;
    auto r1 = net.addNode<Router>("R1", "192.168.0.1");
    auto r2 = net.addNode<Router>("R2", "192.168.0.2");
    Router* router = dynamic_cast<Router*>(r1.get());
    router->addRoute("10.0.0.0/24", r2.get());
    Packet pkt("A", "10.0.0.1", "data", "tcp", "payload");
    // Assume forwardPacket method exists
    router->forwardPacket(pkt); // Should route to R2
    // But since not implemented, just check no throw
    EXPECT_NO_THROW(router->receivePacket(pkt));
}

// 12. Host wysyłanie pakietów: Dodaj metodę sendPacket w Host, która wysyła do Routera
TEST(HostTest, SendPacketToRouter) {
    Network net;
    net.addNode<Host>("H1", "192.168.0.2", 8080);
    net.addNode<Router>("R1", "192.168.0.1");
    net.connect("H1", "R1");
    // Assume Host has sendPacket method
    // Host* host = dynamic_cast<Host*>(net.findByName("H1").get());
    // host->sendPacket(Packet("H1", "10.0.0.1", "data", "tcp", "msg"));
    // Check if packet reached router
    // For now, just check connection
    EXPECT_EQ(net.getNeighbors("H1").size(), 1);
}

// 13. Symulacja transmisji: Dodaj metodę Network::sendPacket, która symuluje routing
TEST(NetworkTest, SendPacketSimulation) {
    Network net;
    net.addNode<Host>("H1", "192.168.0.2", 8080);
    net.addNode<Router>("R1", "192.168.0.1");
    net.addNode<Host>("H2", "10.0.0.1", 9090);
    net.connect("H1", "R1");
    net.connect("R1", "H2");
    // Assume sendPacket method
    // net.sendPacket(Packet("H1", "H2", "data", "tcp", "msg"));
    // Check if packet arrived at H2
    // For now, just check connectivity
    EXPECT_TRUE(net.canCommunicate("H1", "H2"));
}

// 14. Ping z opóźnieniami: Rozszerz Engine::ping o uwzględnianie delay
TEST(EngineTest, PingWithDelay) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    net.setLinkDelay("A", "B", 100); // 100ms
    Engine engine(net);
    std::vector<std::string> path;
    bool ok = engine.ping("A", "B", path);
    EXPECT_TRUE(ok);
    // Assume ping returns total delay
    // EXPECT_EQ(engine.getTotalDelay(path), 100);
}

// 15. Multicast: Dodaj metodę Engine::multicast dla wysyłania do wielu dst
TEST(EngineTest, Multicast) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.addNode<DummyNode>("C", "10.0.0.3");
    net.connect("A", "B");
    net.connect("A", "C");
    Engine engine(net);
    std::vector<std::string> destinations = {"B", "C"};
    // Assume multicast method
    // bool ok = engine.multicast("A", destinations);
    // EXPECT_TRUE(ok);
}

// 16. QoS (Quality of Service): Dodaj priorytety pakietów
// Test sprawdza funkcjonalność Quality of Service (QoS)
// Weryfikuje ustawianie i odczytywanie priorytetów pakietów
TEST(PacketTest, QoSPriority) {
    Packet pkt("A", "B", "data", "tcp", "payload"); // Utwórz pakiet
    pkt.setPriority(5); // Ustaw priorytet pakietu na 5
    EXPECT_EQ(pkt.getPriority(), 5); // Sprawdź, czy priorytet został ustawiony prawidłowo
}

// 17. Load Balancing: Router wybiera ścieżkę na podstawie obciążenia
TEST(RouterTest, LoadBalancing) {
    Network net;
    auto r1 = net.addNode<Router>("R1", "192.168.0.1");
    auto r2 = net.addNode<Router>("R2", "192.168.0.2");
    auto r3 = net.addNode<Router>("R3", "192.168.0.3");
    Router* router = dynamic_cast<Router*>(r1.get());
    router->addRoute("10.0.0.0/24", r2.get());
    router->addRoute("10.0.0.0/24", r3.get()); // Multiple routes
    // Assume getBalancedNextHop
    // std::string next = router.getBalancedNextHop("10.0.0.1");
    // EXPECT_TRUE(next == "R2" || next == "R3");
}

// 18. Network Monitoring: Dodaj metody do monitorowania ruchu
TEST(NetworkTest, NetworkMonitoring) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    // Assume getTrafficStats
    // auto stats = net.getTrafficStats();
    // EXPECT_TRUE(stats.empty()); // Initially
}

// 19. Dynamic Routing: Router aktualizuje trasy dynamicznie
TEST(RouterTest, DynamicRouting) {
    Network net;
    auto r1 = net.addNode<Router>("R1", "192.168.0.1");
    auto r2 = net.addNode<Router>("R2", "192.168.0.2");
    auto r4 = net.addNode<Router>("R4", "192.168.0.4");
    Router* router = dynamic_cast<Router*>(r1.get());
    router->addRoute("10.0.0.0/24", r2.get());
    // Assume updateRoute on failure
    // router.updateRoute("10.0.0.0/24", r4.get());
    // EXPECT_EQ(router->getNextHop("10.0.0.1"), r4.get());
}

// 20. Packet Loss Simulation: Dodaj szansę na utratę pakietu
TEST(NetworkTest, PacketLoss) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    net.setPacketLoss("A", "B", 0.1); // 10% loss, assume method
    // Assume sendPacket with loss
    // bool sent = net.sendPacket(Packet("A", "B", "data", "tcp", "msg"));
    // May fail sometimes, but for test, expect possible
}

// Nowe pomysły na funkcjonalności - dodaj implementację, aby testy przeszły

// 21. Congestion Control: Dodaj mechanizm kontroli przeciążenia
//    - Dodaj kolejki pakietów w węzłach, drop przy przeciążeniu
//    - Metody: enqueuePacket, dequeuePacket, isCongested
// Test sprawdza mechanizm kontroli przeciążenia w węzłach
// Weryfikuje kolejkowanie pakietów i wykrywanie stanu przeciążenia
TEST(NetworkTest, CongestionControl) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Węzeł z kolejką pakietów
    net.addNode<DummyNode>("B", "10.0.0.2"); // Węzeł docelowy
    net.connect("A", "B"); // Połącz węzły

    net.setQueueSize("A", 5); // Ustaw maksymalny rozmiar kolejki na 5 pakietów

    // Dodaj 7 pakietów do kolejki (więcej niż limit)
    for (int i = 0; i < 7; ++i) {
        net.enqueuePacket("A", Packet("A", "B", "data", "tcp", "msg" + std::to_string(i)));
    }

    // Węzeł powinien być przeciążony (kolejka większa niż limit)
    EXPECT_TRUE(net.isCongested("A"));

    // Usuń jeden pakiet z kolejki
    net.dequeuePacket("A");

    // Węzeł nie powinien już być przeciążony
    EXPECT_FALSE(net.isCongested("A"));
}

// 22. TCP Simulation: Dodaj symulację protokołu TCP
//    - Dodaj handshake (SYN, SYN-ACK, ACK), retransmisje
//    - Metody: initiateTCPConnection, sendTCPPacket
// Test sprawdza symulację protokołu TCP
// Weryfikuje nawiązywanie połączenia TCP i wysyłanie pakietów TCP
TEST(NetworkTest, TCPSimulation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Klient TCP
    net.addNode<DummyNode>("B", "10.0.0.2"); // Serwer TCP
    net.connect("A", "B"); // Połącz węzły

    // Nawiąż połączenie TCP (symulacja 3-way handshake)
    bool connected = net.initiateTCPConnection("A", "B");
    EXPECT_TRUE(connected); // Połączenie powinno zostać nawiązane

    // Wyślij pakiet TCP z flagą ACK
    bool sent = net.sendTCPPacket("A", "B", Packet("A", "B", "data", "tcp", "payload"));
    EXPECT_TRUE(sent); // Pakiet powinien zostać wysłany pomyślnie
}

// 23. UDP Simulation: Dodaj symulację protokołu UDP
//    - Bezpołączeniowy, bez retransmisji
//    - Metody: sendUDPPacket (bez gwarancji dostarczenia)
TEST(NetworkTest, UDPSimulation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    // Send UDP packet
    bool sent = net.sendUDPPacket("A", "B", Packet("A", "B", "data", "udp", "payload"));
    EXPECT_TRUE(sent); // Always true for UDP, no guarantee
}

// 24. Time-Based Simulation: Dodaj symulację czasu rzeczywistego dla pakietów
//    - Pakiety są planowane do dostarczenia po opóźnieniu (delayMs pakietu + linkDelay między węzłami)
//    - Metody: advanceTime(ms: int) - przesuwa czas symulacji o ms milisekund
//    - schedulePacketDelivery(pkt: Packet, delay: int) - planuje pakiet do wysłania po dodatkowym delay ms
//    - hasPacketArrived(node: string) - sprawdza, czy jakiś pakiet dotarł do węzła w bieżącym czasie
//    - Używa kolejki zdarzeń czasowych (scheduledPackets: map<time, vector<Packet>> w Network)
//    - Przy advanceTime, dostarcza pakiety, które osiągnęły zaplanowany czas (currentTime + delay + linkDelay)
//    - Symuluje rzeczywisty czas podróży pakietów w sieci
// Test sprawdza symulację czasową transmisji pakietów
// Weryfikuje planowanie dostarczenia pakietów z opóźnieniami i postęp czasu symulacji
TEST(NetworkTest, TimeBasedSimulation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Węzeł źródłowy
    net.addNode<DummyNode>("B", "10.0.0.2"); // Węzeł docelowy
    net.connect("A", "B"); // Połącz węzły
    net.setLinkDelay("A", "B", 100); // Ustaw opóźnienie łącza na 100ms

    // Zaplanuj dostarczenie pakietu z dodatkowym opóźnieniem 50ms
    net.schedulePacketDelivery(Packet("A", "B", "data", "tcp", "msg"), 50);
    // Całkowite opóźnienie: 50ms (zaplanowane) + 100ms (łącze) = 150ms

    // Przesuń czas symulacji do 100ms - pakiet jeszcze nie dotarł
    net.advanceTime(100);
    EXPECT_FALSE(net.hasPacketArrived("B")); // Pakiet nie powinien jeszcze dotrzeć

    // Przesuń czas do 160ms (100 + 60 = 160) - pakiet powinien dotrzeć (160 >= 150)
    net.advanceTime(60);
    EXPECT_TRUE(net.hasPacketArrived("B")); // Pakiet powinien już dotrzeć
}

// 25. Routing Protocols: Dodaj symulację protokołów routingu jak OSPF/BGP
//    - Router wymienia informacje o trasach z sąsiednimi routerami
//    - Metody w Router: exchangeRoutingInfo(otherRouter: Router*) - wysyła swoją tablicę routingu
//    - updateRoutingTable() - aktualizuje tablicę na podstawie otrzymanych info (np. Dijkstra dla OSPF)
//    - hasRouteTo(dst: string) - sprawdza, czy ma trasę do dst
//    - Symuluje dynamiczne uczenie się tras w sieci (jak w realnych protokołach)
//    - Używa map<string, Node*> routingTable w Router
 TEST(RouterTest, RoutingProtocols) {
     Network net;
     auto r1 = net.addNode<Router>("R1", "192.168.0.1");
     auto r2 = net.addNode<Router>("R2", "192.168.0.2");
     auto r3 = net.addNode<Router>("R3", "192.168.0.3");
     net.connect("R1", "R2");
     net.connect("R2", "R3");
     Router* router1 = dynamic_cast<Router*>(r1.get());
     Router* router2 = dynamic_cast<Router*>(r2.get());
     // R1 dodaje trasę do R3 przez R2
     router1->addRoute("192.168.0.3", r2.get());
     // Exchange info between R1 and R2
     router1->exchangeRoutingInfo(router2);
     // Assume R2 updates and now has route to R3
     EXPECT_TRUE(router2->hasRouteTo("192.168.0.3"));
 }

// 26. Packet Fragmentation: Dodaj mechanizm dzielenia dużych pakietów na fragmenty
//    - Jeśli payload > MTU (np. 500 bajtów), pakiet dzielony na mniejsze fragmenty
//    - Metody w Packet: fragmentPacket(mtu: int) -> vector<Packet> - dzieli na fragmenty z sekwencjami
//    - reassemblePacket(fragments: vector<Packet>) -> Packet - składa z powrotem (static method)
//    - Każdy fragment ma seqNum, ostatni ma flagę "last"
//    - Symuluje IP fragmentation dla dużych danych w sieci
// Test sprawdza mechanizm fragmentacji pakietów dla dużych danych
// Weryfikuje dzielenie pakietu na mniejsze fragmenty i ich ponowne składanie
TEST(PacketTest, PacketFragmentation) {
    // Utwórz pakiet z dużym payload (2000 bajtów) - większym niż MTU
    Packet largePkt("A", "B", "data", "tcp", std::string(2000, 'x'));
    int mtu = 500; // Maximum Transmission Unit - maksymalny rozmiar pakietu

    // Podziel pakiet na fragmenty
    std::vector<Packet> fragments = largePkt.fragmentPacket(mtu);
    EXPECT_EQ(fragments.size(), 4); // 2000 / 500 = 4 fragmenty

    // Sprawdź rozmiary fragmentów - każdy powinien mieć maksymalnie MTU bajtów
    EXPECT_EQ(fragments[0].payload.size(), 500); // Pierwszy fragment
    EXPECT_EQ(fragments[3].payload.size(), 500); // Ostatni fragment

    // Złóż fragmenty z powrotem w oryginalny pakiet
    Packet reassembled = Packet::reassemblePacket(fragments);

    // Sprawdź, czy reassemblacja przywróciła oryginalne dane
    EXPECT_EQ(reassembled.payload, largePkt.payload); // Payload powinien być identyczny
    EXPECT_EQ(reassembled.src, "A");     // Źródło powinno być zachowane
    EXPECT_EQ(reassembled.dest, "B");    // Cel powinien być zachowany
}

// 27. Wireless Networks: Dodaj symulację sieci bezprzewodowych (Wi-Fi)
//    - Węzły łączą się bezprzewodowo z zasięgiem i interferencjami
//    - Metody w Network: connectWireless(a: string, b: string) - łączy jeśli w zasięgu
//    - setWirelessRange(node: string, range: int) - ustawia zasięg w metrach
//    - simulateInterference(node: string, lossProb: double) - symuluje zakłócenia zwiększające packet loss
//    - isWirelessConnected(a: string, b: string) - sprawdza połączenie (zasięg + interferencje)
//    - Używa dodatkowej mapy wirelessLinks i range dla węzłów
//    - Symuluje mobilność i utratę sygnału w sieciach Wi-Fi
 TEST(NetworkTest, WirelessNetworks) {
     Network net;
     net.addNode<DummyNode>("AP", "192.168.1.1"); // Access Point
     net.addNode<DummyNode>("Client1", "192.168.1.2");
     net.addNode<DummyNode>("Client2", "192.168.1.3");
     net.setWirelessRange("AP", 50); // 50m range
     // Connect within range
     net.connectWireless("AP", "Client1"); // Assume Client1 is 30m away
     EXPECT_TRUE(net.isWirelessConnected("AP", "Client1"));
     // Simulate interference on AP
     net.simulateInterference("AP", 0.3); // 30% additional loss
     // Client2 out of range
     net.connectWireless("AP", "Client2"); // Assume 60m away
     EXPECT_FALSE(net.isWirelessConnected("AP", "Client2"));
 }

// 28. Cloud Integration: Dodaj symulację węzłów w chmurze z autoscaling
//    - Węzły chmurowe mogą się skalować dynamicznie (np. AWS EC2)
//    - Metody w Network: addCloudNode(name: string, ip: string) - dodaje węzeł chmurowy
//    - getCloudNodes() -> vector<string> - lista węzłów chmurowych
//    - scaleUp(cloudName: string) - dodaje instancję do grupy
//    - scaleDown(cloudName: string) - usuwa instancję
//    - Używa map<string, vector<string>> cloudGroups dla grup instancji
//    - Symuluje elastyczność chmury (np. zwiększanie mocy przy obciążeniu)
// TEST(NetworkTest, CloudIntegration) {
//     Network net;
//     net.addCloudNode("WebServer", "cloud.example.com");
//     EXPECT_EQ(net.getCloudNodes().size(), 1);
//     // Simulate load increase, scale up
//     net.scaleUp("WebServer"); // Add another instance
//     EXPECT_EQ(net.getCloudNodes().size(), 2);
//     // Scale down when load decreases
//     net.scaleDown("WebServer");
//     EXPECT_EQ(net.getCloudNodes().size(), 1);
// }

// 29. IoT Devices: Dodaj symulację urządzeń Internetu Rzeczy (IoT)
//    - Urządzenia z niskim zasilaniem, sporadycznymi połączeniami
//    - Metody w Network: addIoTDevice(name: string, ip: string) - dodaje urządzenie IoT
//    - hasIoTDevice(name: string) -> bool - sprawdza istnienie
//    - simulateBatteryDrain(name: string, percent: int) - zmniejsza baterię o percent%
//    - getBatteryLevel(name: string) -> int - zwraca poziom baterii (0-100)
//    - Używa map<string, int> iotBatteries dla poziomów baterii
//    - Symuluje wyzwania IoT: utrata połączenia przy niskiej baterii, energię
// TEST(NetworkTest, IoTDevices) {
//     Network net;
//     net.addIoTDevice("TempSensor", "10.0.0.10");
//     EXPECT_TRUE(net.hasIoTDevice("TempSensor"));
//     EXPECT_EQ(net.getBatteryLevel("TempSensor"), 100); // Initial 100%
//     // Simulate data transmission, battery drain
//     net.simulateBatteryDrain("TempSensor", 15); // 15% used
//     EXPECT_EQ(net.getBatteryLevel("TempSensor"), 85);
//     // If battery < 10%, device disconnects
//     net.simulateBatteryDrain("TempSensor", 80);
//     EXPECT_LT(net.getBatteryLevel("TempSensor"), 10);
// }

// 30. Performance Metrics: Dodaj zbieranie metryk wydajności sieci
//    - Mierzy latency (opóźnienie), throughput (przepustowość), packet loss rate
//    - Metody w Network: getLatency(a: string, b: string) -> int - średnie opóźnienie w ms
//    - getThroughput(a: string, b: string) -> int - liczba pakietów wysłanych na jednostkę czasu
//    - getPacketLossRate(a: string, b: string) -> double - procent utraconych pakietów (0.0-1.0)
//    - Używa map dla zbierania statystyk podczas symulacji (np. totalPackets, lostPackets, delays)
//    - Symuluje monitoring sieci (jak w narzędziach Wireshark lub SNMP)
// TEST(NetworkTest, PerformanceMetrics) {
//     Network net;
//     net.addNode<DummyNode>("A", "10.0.0.1");
//     net.addNode<DummyNode>("B", "10.0.0.2");
//     net.connect("A", "B");
//     net.setLinkDelay("A", "B", 50); // 50ms delay
//     // Simulate sending packets
//     for (int i = 0; i < 10; ++i) {
//         net.incrementPacketCount("A", "B"); // Track sent packets
//     }
//     // Check metrics
//     EXPECT_EQ(net.getLatency("A", "B"), 50); // Link delay
//     EXPECT_EQ(net.getThroughput("A", "B"), 10); // 10 packets sent
//     EXPECT_EQ(net.getPacketLossRate("A", "B"), 0.0); // No loss simulated
//     // With loss
//     net.setPacketLoss("A", "B", 0.2); // 20% loss
//     EXPECT_EQ(net.getPacketLossRate("A", "B"), 0.2);
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// 1. RemoveNode: Dodaj metodę void Network::removeNode(const std::string& name)
//    - Usuń węzeł z nodes, nodesByName, adj
//    - Rzuć wyjątek jeśli węzeł nie istnieje lub ma połączenia
// Test sprawdza usuwanie węzłów z sieci
// Weryfikuje, czy węzeł może być usunięty i czy staje się niedostępny
TEST(NetworkTest, RemoveNode) {
    Network net;
    auto n1 = net.addNode<DummyNode>("A", "10.0.0.1"); // Dodaj węzeł do sieci
    EXPECT_TRUE(net.findByName("A") != nullptr); // Węzeł powinien istnieć

    net.removeNode("A"); // Usuń węzeł z sieci

    // Próba znalezienia usuniętego węzła powinna rzucić wyjątek
    EXPECT_THROW(net.findByName("A"), std::runtime_error);
}

// 2. DisconnectNodes: Dodaj metodę void Network::disconnect(const std::string& nameA, const std::string& nameB)
//    - Usuń połączenie między węzłami w adj
// Test sprawdza rozłączanie węzłów w sieci
// Weryfikuje usuwanie połączeń między węzłami i aktualizację listy sąsiadów
TEST(NetworkTest, DisconnectNodes) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Dodaj węzeł A
    net.addNode<DummyNode>("B", "10.0.0.2"); // Dodaj węzeł B
    net.connect("A", "B"); // Połącz węzły

    // Sprawdź, czy węzeł A ma jednego sąsiada
    EXPECT_EQ(net.getNeighbors("A").size(), 1);

    // Rozłącz węzły
    net.disconnect("A", "B");

    // Sprawdź, czy węzeł A nie ma już sąsiadów
    EXPECT_EQ(net.getNeighbors("A").size(), 0);
}

// 3. LinkDelay: Dodaj mapy dla opóźnień, metody setLinkDelay/getLinkDelay
//    - setLinkDelay: ustaw opóźnienie między węzłami
//    - getLinkDelay: zwróć opóźnienie (domyślnie 0)
// Test sprawdza symulację opóźnień łączy między węzłami
// Weryfikuje ustawianie i odczytywanie opóźnień transmisji
TEST(NetworkTest, LinkDelaySimulation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Węzeł A
    net.addNode<DummyNode>("B", "10.0.0.2"); // Węzeł B
    net.connect("A", "B"); // Połącz węzły

    net.setLinkDelay("A", "B", 50); // Ustaw opóźnienie łącza na 50ms

    // Sprawdź opóźnienie w obu kierunkach (symetryczne)
    EXPECT_EQ(net.getLinkDelay("A", "B"), 50); // Z A do B
    EXPECT_EQ(net.getLinkDelay("B", "A"), 50); // Z B do A
}

// 4. PacketStatistics: Dodaj liczniki pakietów, metody getPacketCount, incrementPacketCount
//    - Inkrementuj licznik przy każdym wysłaniu pakietu
// Test sprawdza zbieranie statystyk pakietów między węzłami
// Weryfikuje liczenie pakietów przesyłanych między węzłami
TEST(NetworkTest, PacketStatistics) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Węzeł źródłowy
    net.addNode<DummyNode>("B", "10.0.0.2"); // Węzeł docelowy
    net.connect("A", "B"); // Połącz węzły

    // Sprawdź początkową liczbę pakietów (powinna być 0)
    EXPECT_EQ(net.getPacketCount("A", "B"), 0);

    // Zwiększ licznik pakietów
    net.incrementPacketCount("A", "B");

    // Sprawdź, czy licznik został zwiększony
    EXPECT_EQ(net.getPacketCount("A", "B"), 1);
}

// 5. VLAN: Dodaj mapę VLAN dla węzłów, metody assignVLAN, canCommunicate
//    - assignVLAN: przypisz VLAN do węzła
//    - canCommunicate: sprawdź czy węzły mogą się komunikować (ten sam VLAN)
// Test sprawdza izolację VLAN w sieci
// Weryfikuje, czy węzły w różnych VLAN-ach nie mogą się komunikować
TEST(NetworkTest, VLANIsolation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Dodaj węzeł A
    net.addNode<DummyNode>("B", "10.0.0.2"); // Dodaj węzeł B

    net.assignVLAN("A", 10); // Przypisz węzeł A do VLAN 10
    net.assignVLAN("B", 20); // Przypisz węzeł B do VLAN 20

    // Węzły w różnych VLAN-ach nie powinny móc się komunikować
    EXPECT_FALSE(net.canCommunicate("A", "B"));

    net.assignVLAN("B", 10); // Zmień VLAN węzła B na 10

    // Teraz węzły w tym samym VLAN-ie powinny móc się komunikować
    EXPECT_TRUE(net.canCommunicate("A", "B"));
}

// 6. ExportImportTopology: Dodaj metody exportToJson, importFromJson
//    - exportToJson: serializuj węzły i połączenia do JSON
//    - importFromJson: wczytaj z JSON i odtwórz sieć
// Test sprawdza eksport i import topologii sieci do/z formatu JSON
// Weryfikuje serializację i deserializację struktury sieci
TEST(NetworkTest, ExportImportTopology) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Dodaj węzeł A
    net.addNode<DummyNode>("B", "10.0.0.2"); // Dodaj węzeł B
    net.connect("A", "B"); // Połącz węzły

    // Eksportuj topologię do JSON
    std::string json = net.exportToJson();
    EXPECT_FALSE(json.empty()); // JSON nie powinien być pusty

    // Importuj topologię z JSON do nowej sieci
    Network net2;
    net2.importFromJson(json);

    // Sprawdź, czy węzły zostały prawidłowo zaimportowane
    EXPECT_TRUE(net2.findByName("A") != nullptr); // Węzeł A powinien istnieć
    EXPECT_TRUE(net2.findByName("B") != nullptr); // Węzeł B powinien istnieć
    EXPECT_EQ(net2.getNeighbors("A").size(), 1); // Węzeł A powinien mieć jednego sąsiada
}

// 7. NodeFailure: Dodaj flagę failed dla węzłów, metody failNode, isFailed
//    - failNode: oznacz węzeł jako uszkodzony
//    - sendPacket: rzuć wyjątek jeśli węzeł failed
// Test sprawdza symulację awarii węzłów w sieci
// Weryfikuje oznaczanie węzłów jako uszkodzone i ich wpływ na komunikację
TEST(NetworkTest, NodeFailureSimulation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Węzeł działający
    net.addNode<DummyNode>("B", "10.0.0.2"); // Węzeł, który ulegnie awarii
    net.connect("A", "B"); // Połącz węzły

    // Sprawdź, czy węzeł B jest początkowo działający
    EXPECT_FALSE(net.isFailed("B"));

    // Symuluj awarię węzła B
    net.failNode("B");

    // Sprawdź, czy węzeł B jest teraz oznaczony jako uszkodzony
    EXPECT_TRUE(net.isFailed("B"));

    // Test wysyłania pakietu do uszkodzonego węzła
    // (wymaga implementacji sendPacket w Network, która rzuca wyjątek dla failed węzłów)
    // EXPECT_THROW(net.sendPacket(Packet("A", "B", "data", "tcp", "msg")), std::runtime_error);
}

// 8. Traceroute: Rozszerz Engine::ping o traceroute (zwróć pełną ścieżkę)
//    - Dodaj metodę bool Engine::traceroute(const std::string& src, const std::string& dst, std::vector<std::string>& path)
// Test sprawdza funkcjonalność traceroute
// Weryfikuje zwracanie pełnej ścieżki pakietów przez sieć
TEST(EngineTest, Traceroute) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Węzeł startowy
    net.addNode<DummyNode>("B", "10.0.0.2"); // Węzeł pośredni
    net.addNode<DummyNode>("C", "10.0.0.3"); // Węzeł docelowy

    net.connect("A", "B"); // Połącz A z B
    net.connect("B", "C"); // Połącz B z C

    Engine engine(net);
    std::vector<std::string> path; // Kontener na ścieżkę

    // Wykonaj traceroute z A do C
    bool ok = engine.traceroute("A", "C", path);

    EXPECT_TRUE(ok); // Traceroute powinien się powieść
    // Sprawdź, czy ścieżka zawiera wszystkie węzły w prawidłowej kolejności
    EXPECT_EQ(path, std::vector<std::string>({"A", "B", "C"}));
}

// 9. Bandwidth: Dodaj bandwidth dla łączy, metody setBandwidth, getBandwidth
//    - Symuluj przepustowość (np. zmniejszaj przy każdym pakiecie)
// Test sprawdza symulację przepustowości łączy
// Weryfikuje ustawianie bandwidth i symulację jego zużywania
TEST(NetworkTest, BandwidthSimulation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Węzeł źródłowy
    net.addNode<DummyNode>("B", "10.0.0.2"); // Węzeł docelowy
    net.connect("A", "B"); // Połącz węzły

    net.setBandwidth("A", "B", 100); // Ustaw przepustowość łącza na 100 Mbps

    // Sprawdź początkową przepustowość
    EXPECT_EQ(net.getBandwidth("A", "B"), 100);

    // Zużyj część przepustowości (np. przez przesłanie pakietu)
    net.consumeBandwidth("A", "B", 10); // Zużyj 10 Mbps

    // Sprawdź pozostałą przepustowość
    EXPECT_EQ(net.getBandwidth("A", "B"), 90);
}

// 10. Firewall: Dodaj reguły firewall, metody addFirewallRule, isAllowed
//     - addFirewallRule: dodaj regułę blokowania/zezwalania
//     - sendPacket: sprawdź reguły przed wysłaniem
// Test sprawdza funkcjonalność reguł firewall
// Weryfikuje blokowanie i zezwalanie na ruch sieciowy na podstawie protokołu
TEST(NetworkTest, FirewallRules) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1"); // Węzeł źródłowy
    net.addNode<DummyNode>("B", "10.0.0.2"); // Węzeł docelowy
    net.connect("A", "B"); // Połącz węzły

    // Dodaj regułę blokującą ruch TCP między A i B
    net.addFirewallRule("A", "B", "tcp", false);

    // Sprawdź, czy ruch TCP jest blokowany
    EXPECT_FALSE(net.isAllowed("A", "B", "tcp"));

    // Sprawdź, czy ruch UDP jest domyślnie dozwolony (brak reguły blokującej)
    EXPECT_TRUE(net.isAllowed("A", "B", "udp"));
}