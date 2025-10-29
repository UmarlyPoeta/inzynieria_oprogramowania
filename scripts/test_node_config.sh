#!/bin/bash

# NetSimCPP WebSocket & /node/config Test Script
# Ten skrypt testuje nową funkcjonalność /node/config oraz WebSocket

REST_API="http://localhost:8080"
WS_URL="ws://localhost:9001"

echo "🧪 NetSimCPP - Test WebSocket i /node/config"
echo "=============================================="
echo ""

# Kolory
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Funkcja do testowania REST API
test_rest() {
    local endpoint=$1
    local method=$2
    local data=$3
    local description=$4
    
    echo -e "${YELLOW}Test: ${description}${NC}"
    echo "Endpoint: ${method} ${endpoint}"
    
    if [ -n "$data" ]; then
        echo "Data: ${data}"
        response=$(curl -s -X ${method} "${REST_API}${endpoint}" \
            -H "Content-Type: application/json" \
            -d "${data}")
    else
        response=$(curl -s -X ${method} "${REST_API}${endpoint}")
    fi
    
    echo -e "${GREEN}Odpowiedź:${NC}"
    echo "$response" | jq '.' 2>/dev/null || echo "$response"
    echo ""
}

# Test 1: Sprawdź status serwera
echo "=== Test 1: Status serwera ==="
test_rest "/status" "GET" "" "Sprawdzenie statusu serwera"

# Test 2: Dodaj węzeł Host
echo "=== Test 2: Dodaj węzeł Host ==="
test_rest "/node/add" "POST" '{"name":"TestHost1","ip":"192.168.1.100","type":"host","port":8080}' "Dodawanie węzła typu Host"

# Test 3: Dodaj węzeł Router
echo "=== Test 3: Dodaj węzeł Router ==="
test_rest "/node/add" "POST" '{"name":"TestRouter1","ip":"192.168.1.1","type":"router"}' "Dodawanie węzła typu Router"

# Test 4: Lista węzłów
echo "=== Test 4: Lista wszystkich węzłów ==="
test_rest "/nodes" "GET" "" "Pobieranie listy węzłów"

# Test 5: NOWA FUNKCJONALNOŚĆ - Konfiguracja węzła (zmiana IP)
echo "=== Test 5: ⭐ Konfiguracja węzła - zmiana IP ==="
test_rest "/node/config" "POST" '{"name":"TestHost1","ip":"192.168.1.200"}' "Zmiana IP węzła TestHost1"

# Test 6: NOWA FUNKCJONALNOŚĆ - Konfiguracja węzła (zmiana MTU)
echo "=== Test 6: ⭐ Konfiguracja węzła - zmiana MTU ==="
test_rest "/node/config" "POST" '{"name":"TestHost1","mtu":1400}' "Zmiana MTU węzła TestHost1"

# Test 7: NOWA FUNKCJONALNOŚĆ - Konfiguracja węzła (zmiana rozmiaru kolejki)
echo "=== Test 7: ⭐ Konfiguracja węzła - zmiana queueSize ==="
test_rest "/node/config" "POST" '{"name":"TestHost1","queueSize":50}' "Zmiana rozmiaru kolejki węzła TestHost1"

# Test 8: NOWA FUNKCJONALNOŚĆ - Pełna konfiguracja węzła
echo "=== Test 8: ⭐ Pełna konfiguracja węzła ==="
test_rest "/node/config" "POST" '{"name":"TestRouter1","ip":"192.168.1.254","mtu":9000,"queueSize":100}' "Pełna aktualizacja konfiguracji routera"

# Test 9: Połącz węzły
echo "=== Test 9: Połącz węzły ==="
test_rest "/link/connect" "POST" '{"nodeA":"TestHost1","nodeB":"TestRouter1","delay":10,"bandwidth":1000}' "Łączenie węzłów"

# Test 10: Topologia
echo "=== Test 10: Pobierz topologię ==="
test_rest "/topology" "GET" "" "Pobieranie pełnej topologii sieci"

# Test 11: Usunięcie węzła
echo "=== Test 11: Usuń węzeł ==="
test_rest "/node/remove" "POST" '{"name":"TestHost1"}' "Usuwanie węzła TestHost1"

# Test 12: Finalna lista węzłów
echo "=== Test 12: Finalna lista węzłów ==="
test_rest "/nodes" "GET" "" "Pobieranie listy węzłów po usunięciu"

echo ""
echo -e "${GREEN}✅ Testy REST API zakończone!${NC}"
echo ""
echo "================================================"
echo "📡 Test WebSocket"
echo "================================================"
echo ""
echo "Aby przetestować WebSocket:"
echo "1. Upewnij się, że serwer działa (./netsim)"
echo "2. Otwórz plik test_websocket.html w przeglądarce"
echo "3. Kliknij 'Połącz z WebSocket'"
echo "4. Wykonaj operacje i obserwuj zdarzenia w czasie rzeczywistym"
echo ""
echo "Zdarzenia WebSocket do obserwowania:"
echo "  - node_added (dodano węzeł)"
echo "  - node_removed (usunięto węzeł)"
echo "  - node_updated ⭐ (zaktualizowano konfigurację węzła)"
echo "  - node_failed (awaria węzła)"
echo "  - link_added (dodano łącze)"
echo ""
echo -e "${YELLOW}Aby otworzyć test WebSocket:${NC}"
echo "  xdg-open project/backend/test_websocket.html"
echo "  # lub otwórz plik ręcznie w przeglądarce"
