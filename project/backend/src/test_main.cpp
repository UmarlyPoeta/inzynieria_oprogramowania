#include <gtest/gtest.h>
#include "core/Node.hpp"
#include "core/Packet.hpp"
#include "core/Network.hpp"
#include "core/Engine.hpp"
#include "core/Host.hpp"
#include "core/Router.hpp"

// DummyNode is defined in Network.hpp

// Test klasy Packet
TEST(PacketTest, ConstructionAndFields) {
    Packet pkt("A", "B", "data", "tcp", "Hello");
    EXPECT_EQ(pkt.src, "A");
    EXPECT_EQ(pkt.dest, "B");
    EXPECT_EQ(pkt.payload, "Hello");
}

TEST(PacketTest, EmptyPayload) {
    Packet pkt("A", "B", "data", "tcp", "");
    EXPECT_EQ(pkt.payload, "");
}

TEST(PacketTest, LongPayload) {
    std::string longPayload(1000, 'x');
    Packet pkt("A", "B", "data", "tcp", longPayload);
    EXPECT_EQ(pkt.payload, longPayload);
}

// Test klasy Node
TEST(NodeTest, Construction) {
    DummyNode node("N1", "10.0.0.1");
    EXPECT_EQ(node.getName(), "N1");
    EXPECT_EQ(node.getIp(), "10.0.0.1");
}

TEST(NodeTest, ReceivePacket) {
    DummyNode node("N2", "10.0.0.2");
    Packet pkt("A", "N2", "data", "tcp", "payload");
    node.receivePacket(pkt);
    EXPECT_TRUE(node.received);
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
TEST(NetworkTest, AddAndFindNode) {
    Network net;
    auto node = net.addNode<DummyNode>("N3", "10.0.0.3");
    auto found = net.findByName("N3");
    EXPECT_EQ(found, node);
}

TEST(NetworkTest, AddDuplicateNodeThrows) {
    Network net;
    net.addNode<DummyNode>("N4", "10.0.0.4");
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
TEST(EngineTest, PingSuccess) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.7");
    net.addNode<DummyNode>("B", "10.0.0.8");
    net.connect("A", "B");
    Engine engine(net);
    std::vector<std::string> path;
    bool ok = engine.ping("A", "B", path);
    EXPECT_TRUE(ok);
    EXPECT_EQ(path.size(), 2);
    EXPECT_EQ(path[0], "A");
    EXPECT_EQ(path[1], "B");
}

TEST(EngineTest, PingFail) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.9");
    net.addNode<DummyNode>("B", "10.0.0.10");
    // brak połączenia
    Engine engine(net);
    std::vector<std::string> path;
    bool ok = engine.ping("A", "B", path);
    EXPECT_FALSE(ok);
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
    // router.forwardPacket(pkt); // Should route to R2
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
TEST(PacketTest, QoSPriority) {
    Packet pkt("A", "B", "data", "tcp", "payload");
    pkt.setPriority(5); // Assume method
    EXPECT_EQ(pkt.getPriority(), 5);
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// 1. RemoveNode: Dodaj metodę void Network::removeNode(const std::string& name)
//    - Usuń węzeł z nodes, nodesByName, adj
//    - Rzuć wyjątek jeśli węzeł nie istnieje lub ma połączenia
TEST(NetworkTest, RemoveNode) {
    Network net;
    auto n1 = net.addNode<DummyNode>("A", "10.0.0.1");
    EXPECT_TRUE(net.findByName("A") != nullptr);
    net.removeNode("A");
    EXPECT_THROW(net.findByName("A"), std::runtime_error);
}

// 2. DisconnectNodes: Dodaj metodę void Network::disconnect(const std::string& nameA, const std::string& nameB)
//    - Usuń połączenie między węzłami w adj
TEST(NetworkTest, DisconnectNodes) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    EXPECT_EQ(net.getNeighbors("A").size(), 1);
    net.disconnect("A", "B");
    EXPECT_EQ(net.getNeighbors("A").size(), 0);
}

// 3. LinkDelay: Dodaj mapy dla opóźnień, metody setLinkDelay/getLinkDelay
//    - setLinkDelay: ustaw opóźnienie między węzłami
//    - getLinkDelay: zwróć opóźnienie (domyślnie 0)
TEST(NetworkTest, LinkDelaySimulation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    net.setLinkDelay("A", "B", 50);
    EXPECT_EQ(net.getLinkDelay("A", "B"), 50);
    EXPECT_EQ(net.getLinkDelay("B", "A"), 50); // symetryczne
}

// 4. PacketStatistics: Dodaj liczniki pakietów, metody getPacketCount, incrementPacketCount
//    - Inkrementuj licznik przy każdym wysłaniu pakietu
TEST(NetworkTest, PacketStatistics) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    EXPECT_EQ(net.getPacketCount("A", "B"), 0);
    net.incrementPacketCount("A", "B");
    EXPECT_EQ(net.getPacketCount("A", "B"), 1);
}

// 5. VLAN: Dodaj mapę VLAN dla węzłów, metody assignVLAN, canCommunicate
//    - assignVLAN: przypisz VLAN do węzła
//    - canCommunicate: sprawdź czy węzły mogą się komunikować (ten sam VLAN)
TEST(NetworkTest, VLANIsolation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.assignVLAN("A", 10);
    net.assignVLAN("B", 20);
    EXPECT_FALSE(net.canCommunicate("A", "B"));
    net.assignVLAN("B", 10);
    EXPECT_TRUE(net.canCommunicate("A", "B"));
}

// 6. ExportImportTopology: Dodaj metody exportToJson, importFromJson
//    - exportToJson: serializuj węzły i połączenia do JSON
//    - importFromJson: wczytaj z JSON i odtwórz sieć
TEST(NetworkTest, ExportImportTopology) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    std::string json = net.exportToJson();
    EXPECT_FALSE(json.empty());
    Network net2;
    net2.importFromJson(json);
    EXPECT_TRUE(net2.findByName("A") != nullptr);
    EXPECT_TRUE(net2.findByName("B") != nullptr);
    EXPECT_EQ(net2.getNeighbors("A").size(), 1);
}

// 7. NodeFailure: Dodaj flagę failed dla węzłów, metody failNode, isFailed
//    - failNode: oznacz węzeł jako uszkodzony
//    - sendPacket: rzuć wyjątek jeśli węzeł failed
TEST(NetworkTest, NodeFailureSimulation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    EXPECT_FALSE(net.isFailed("B"));
    net.failNode("B");
    EXPECT_TRUE(net.isFailed("B"));
    // Test wysyłania pakietu do failed węzła - wymaga implementacji sendPacket w Network
    // EXPECT_THROW(net.sendPacket(Packet("A", "B", "data", "tcp", "msg")), std::runtime_error);
}

// 8. Traceroute: Rozszerz Engine::ping o traceroute (zwróć pełną ścieżkę)
//    - Dodaj metodę bool Engine::traceroute(const std::string& src, const std::string& dst, std::vector<std::string>& path)
TEST(EngineTest, Traceroute) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.addNode<DummyNode>("C", "10.0.0.3");
    net.connect("A", "B");
    net.connect("B", "C");
    Engine engine(net);
    std::vector<std::string> path;
    bool ok = engine.traceroute("A", "C", path);
    EXPECT_TRUE(ok);
    EXPECT_EQ(path, std::vector<std::string>({"A", "B", "C"}));
}

// 9. Bandwidth: Dodaj bandwidth dla łączy, metody setBandwidth, getBandwidth
//    - Symuluj przepustowość (np. zmniejszaj przy każdym pakiecie)
TEST(NetworkTest, BandwidthSimulation) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    net.setBandwidth("A", "B", 100); // 100 Mbps
    EXPECT_EQ(net.getBandwidth("A", "B"), 100);
    net.consumeBandwidth("A", "B", 10); // zużyj 10
    EXPECT_EQ(net.getBandwidth("A", "B"), 90);
}

// 10. Firewall: Dodaj reguły firewall, metody addFirewallRule, isAllowed
//     - addFirewallRule: dodaj regułę blokowania/zezwalania
//     - sendPacket: sprawdź reguły przed wysłaniem
TEST(NetworkTest, FirewallRules) {
    Network net;
    net.addNode<DummyNode>("A", "10.0.0.1");
    net.addNode<DummyNode>("B", "10.0.0.2");
    net.connect("A", "B");
    net.addFirewallRule("A", "B", "tcp", false); // blokuj TCP
    EXPECT_FALSE(net.isAllowed("A", "B", "tcp"));
    EXPECT_TRUE(net.isAllowed("A", "B", "udp")); // domyślnie zezwól
}