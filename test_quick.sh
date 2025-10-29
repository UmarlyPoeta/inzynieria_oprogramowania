#!/bin/bash

echo "🧪 Test /node/config - Nowa funkcjonalność"
echo "=========================================="
echo ""

# Test 1: Dodaj węzeł
echo "1️⃣ Dodawanie węzła TestHost1..."
curl -s -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"TestHost1","ip":"192.168.1.100","type":"host","port":8080}'
echo ""
echo ""

# Test 2: Zmiana IP
echo "2️⃣ Zmiana IP węzła TestHost1 na 192.168.1.200..."
curl -s -X POST http://localhost:8080/node/config \
  -H "Content-Type: application/json" \
  -d '{"name":"TestHost1","ip":"192.168.1.200"}'
echo ""
echo ""

# Test 3: Zmiana MTU
echo "3️⃣ Zmiana MTU węzła TestHost1 na 1400..."
curl -s -X POST http://localhost:8080/node/config \
  -H "Content-Type: application/json" \
  -d '{"name":"TestHost1","mtu":1400}'
echo ""
echo ""

# Test 4: Zmiana rozmiaru kolejki
echo "4️⃣ Zmiana rozmiaru kolejki węzła TestHost1 na 50..."
curl -s -X POST http://localhost:8080/node/config \
  -H "Content-Type: application/json" \
  -d '{"name":"TestHost1","queueSize":50}'
echo ""
echo ""

# Test 5: Pełna aktualizacja
echo "5️⃣ Pełna aktualizacja konfiguracji (IP + MTU + QueueSize)..."
curl -s -X POST http://localhost:8080/node/config \
  -H "Content-Type: application/json" \
  -d '{"name":"TestHost1","ip":"192.168.1.254","mtu":9000,"queueSize":100}'
echo ""
echo ""

# Test 6: Test błędu - nieistniejący węzeł
echo "6️⃣ Test błędu - próba aktualizacji nieistniejącego węzła..."
curl -s -X POST http://localhost:8080/node/config \
  -H "Content-Type: application/json" \
  -d '{"name":"NonExistentNode","ip":"1.1.1.1"}'
echo ""
echo ""

echo "✅ Testy zakończone!"
echo ""
echo "💡 Otwórz test_websocket.html w przeglądarce aby zobaczyć zdarzenia WebSocket"
