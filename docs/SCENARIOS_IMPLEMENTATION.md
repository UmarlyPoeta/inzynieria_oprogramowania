# Network Scenarios & Playbooks - Implementation Guide

## Zaimplementowane

### ✅ Struktura plików
- `scenarios/README.md` - Dokumentacja formatu scenariuszy
- `scenarios/examples/basic_ping.yaml` - Przykład prostego scenariusza
- `scenarios/examples/vlan_isolation.yaml` - Przykład zaawansowany
- `src/scenario/ScenarioTypes.hpp` - Definicje typów
- `src/scenario/ScenarioTypes.cpp` - Parser JSON/YAML
- `src/scenario/ScenarioRunner.hpp` - Silnik wykonawczy
- `src/scenario/ScenarioRunner.cpp` - Implementacja

### ✅ Funkcjonalności

**Parser:**
- [x] Parsowanie JSON (pełne)
- [ ] Parsowanie YAML (TODO - wymaga yaml-cpp)
- [x] Konwersja Scenario → JSON
- [x] Walidacja struktury

**Actions (częściowo):**
- [x] `ping` - test połączenia (działa)
- [x] `wait` - pauza w ms (działa)
- [x] `validate` - walidacja topologii (działa)
- [ ] `send` - wysyłanie pakietów (szkielet)
- [ ] `configure` - konfiguracja węzłów (szkielet)

**Validations (częściowo):**
- [x] `connectivity` - test pingiem (działa)
- [ ] `isolation` - izolacja VLAN (szkielet)
- [ ] `latency` - pomiar opóźnień (TODO)
- [ ] `packet_loss` - test utraty pakietów (TODO)
- [ ] `throughput` - test przepustowości (TODO)

## Następne kroki

### 1. Dodaj REST API endpoint (PRIORYTET)

```cpp
// W main.cpp dodaj:

listener.support(methods::POST, [&](http_request request) {
    auto path = request.relative_uri().path();
    
    if (path == U("/scenario/run")) {
        request.extract_json().then([&](web::json::value jv) {
            try {
                // Parse scenario from JSON
                auto scenario = Scenario::fromJSON(jv);
                
                // Run scenario
                ScenarioRunner runner(net, engine);
                auto result = runner.runScenario(scenario);
                
                // Return result
                web::json::value response = result.toJSON();
                request.reply(status_codes::OK, response);
            } catch (const std::exception& e) {
                web::json::value error;
                error[U("error")] = web::json::value::string(
                    utility::conversions::to_string_t(e.what())
                );
                request.reply(status_codes::BadRequest, error);
            }
        });
    }
});
```

### 2. Dodaj load z pliku

```cpp
if (path == U("/scenario/run_file")) {
    // Load from file
    std::string filename = params.value("file", "");
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    json j = json::parse(content);
    auto scenario = Scenario::fromJSON(j);
    // ... run scenario
}
```

### 3. Implementuj brakujące akcje

**send - wysyłanie pakietów:**
```cpp
StepResult ScenarioRunner::handleSend(const json& params, const json& expect) {
    std::string from = params.value("from", "");
    std::string to = params.value("to", "");
    int count = params.value("count", 1);
    int size = params.value("size_bytes", 64);
    
    int delivered = 0;
    for (int i = 0; i < count; ++i) {
        Packet pkt(from, to, std::string(size, 'X'));
        if (m_engine.sendPacket(from, to, pkt)) {
            delivered++;
        }
    }
    
    double delivery_rate = static_cast<double>(delivered) / count;
    // Check expectations...
}
```

**configure - konfiguracja węzłów:**
```cpp
StepResult ScenarioRunner::handleConfigure(const json& params, const json& expect) {
    std::string node_name = params.value("node", "");
    auto node = m_network.findNode(node_name);
    
    if (params["config"].contains("mtu")) {
        node->setMTU(params["config"]["mtu"]);
    }
    if (params["config"].contains("ttl")) {
        node->setTTL(params["config"]["ttl"]);
    }
    // ...
}
```

### 4. Dodaj YAML support

Zainstaluj yaml-cpp:
```bash
sudo apt-get install libyaml-cpp-dev  # Ubuntu
sudo dnf install yaml-cpp-devel       # Fedora
```

```cpp
#include <yaml-cpp/yaml.h>

Scenario Scenario::fromYAML(const std::string& yaml_content) {
    YAML::Node root = YAML::Load(yaml_content);
    
    json j;
    // Convert YAML to JSON...
    
    return fromJSON(j);
}
```

### 5. Dodaj więcej przykładowych scenariuszy

- `congestion_control.yaml` - testowanie TCP congestion
- `router_failover.yaml` - redundancja routingu
- `qos_priority.yaml` - priorytety pakietów
- `multicast.yaml` - grupowa transmisja

## Testowanie

### Test manualny (JSON przez API):

```bash
curl -X POST http://localhost:8080/scenario/run \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Quick Test",
    "description": "Two hosts ping",
    "version": "1.0",
    "author": "Test",
    "tags": ["test"],
    "setup": {
      "nodes": [
        {"name": "H1", "type": "host", "ip": "10.0.0.1"},
        {"name": "H2", "type": "host", "ip": "10.0.0.2"}
      ],
      "links": [
        {"from": "H1", "to": "H2", "delay_ms": 5}
      ]
    },
    "steps": [
      {
        "name": "Test ping",
        "action": "ping",
        "params": {"from": "H1", "to": "H2"},
        "expect": {"success": true}
      }
    ],
    "validation": [],
    "expected_outcome": "Hosts should ping successfully"
  }'
```

### Unit test:

```cpp
TEST(ScenarioTest, BasicPing) {
    Network net;
    Engine engine(net);
    
    // Load scenario
    std::ifstream file("scenarios/examples/basic_ping.json");
    json j = json::parse(file);
    auto scenario = Scenario::fromJSON(j);
    
    // Run
    ScenarioRunner runner(net, engine);
    auto result = runner.runScenario(scenario);
    
    // Verify
    ASSERT_TRUE(result.success);
    ASSERT_EQ(result.step_results.size(), 3);
}
```

## Przykłady użycia

### 1. Educational Demo
Student może załadować gotowy scenariusz demonstrujący koncepcję (np. VLAN isolation) i zobaczyć krok po kroku co się dzieje.

### 2. Automated Testing
CI/CD może uruchomić zestaw scenariuszy weryfikujących różne funkcje projektu.

### 3. Research Experiments
Powtarzalne eksperymenty z różnymi parametrami topologii i ruchu.

### 4. Performance Benchmarks
Standardowe scenariusze do mierzenia wydajności różnych wersji.

## Metryki sukcesu

- [ ] 5+ przykładowych scenariuszy
- [ ] REST API endpoint działa
- [ ] Load z pliku działa
- [ ] Wszystkie akcje zaimplementowane
- [ ] Wszystkie walidacje zaimplementowane
- [ ] Dokumentacja kompletna
- [ ] Unit testy pokrywające >80%
