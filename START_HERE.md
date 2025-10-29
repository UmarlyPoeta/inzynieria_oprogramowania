# Quick Start - Testowanie Autentykacji

## Status po setup:
- ✅ MySQL działa (healthy)
- ✅ Tabele auth utworzone (users, user_permissions, api_rate_limits, auth_audit_log)
- ⚠️  Redis nie działa (wymaga uruchomienia)
- ✅ Zależności zainstalowane (hiredis, argon2, jwt-cpp)
- 🔄 Kompilacja w toku...

## KROK 1: Sprawdź Redis (WYMAGANE!)

Redis jest WYMAGANY dla sesji autentykacji. **Twój Redis używa hasła: `NetSimCPP1234`**

```bash
# Sprawdź czy Redis działa z hasłem:
redis-cli -a "NetSimCPP1234" ping

# Powinno zwrócić: PONG

# Jeśli Redis nie działa:
sudo systemctl start redis-server

# Lub:
sudo service redis-server start

# Zweryfikuj ponownie:
redis-cli -a "NetSimCPP1234" ping
# Musi zwrócić: PONG
```

## KROK 2: Sprawdź kompilację

```bash
cd /home/patryk/inzynieria_oprogramowania/project/backend

# Sprawdź czy plik wykonywalny istnieje:
ls -lh netsim

# Jeśli NIE istnieje, sprawdź błędy i przekompiluj:
make -j$(nproc) 2>&1 | tee compile_errors.log
```

## KROK 3: Uruchom serwer

**Terminal 1:**
```bash
cd /home/patryk/inzynieria_oprogramowania/project/backend/build
./run_server.sh
```

Oczekiwany output:
```
[Main] Authentication service initialized
[Main] WebSocket server started on port 9001
Listening for requests at: http://0.0.0.0:8080/
```

## KROK 4: Testuj (w drugim terminalu)

**Terminal 2:**
```bash
cd /home/patryk/inzynieria_oprogramowania
./test_auth.sh
```

Powinieneś zobaczyć zielone checkmarki (✓) dla wszystkich 9 testów!

## Szybkie testy ręczne:

### 1. Sprawdź serwer
```bash
curl http://localhost:8080/status
```

### 2. Login
```bash
TOKEN=$(curl -s -X POST http://localhost:8080/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' | jq -r '.token')

echo "Token: $TOKEN"
```

### 3. Pobierz info o użytkowniku
```bash
curl -X GET http://localhost:8080/auth/me \
  -H "Authorization: Bearer $TOKEN" | jq '.'
```

### 4. Dodaj węzeł (chroniony endpoint)
```bash
curl -X POST http://localhost:8080/node/add \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"name":"router1","ip":"192.168.1.1","type":"router"}' | jq '.'
```

### 5. Test bez autentykacji (powinien zwrócić 401)
```bash
curl -X POST http://localhost:8080/node/add \
  -H "Content-Type: application/json" \
  -d '{"name":"fail","ip":"1.1.1.1","type":"host"}'
```

## Domyślni użytkownicy

| Username | Password | Role | Uprawnienia |
|----------|----------|------|-------------|
| admin | admin123 | admin | Pełny dostęp |
| user1 | user123 | user | Standardowe operacje |
| viewer1 | viewer123 | viewer | Tylko odczyt |

## Rozwiązywanie problemów

### ❌ Redis is not running
```bash
sudo systemctl start redis-server
redis-cli -a "NetSimCPP1234" ping  # musi zwrócić PONG
```

### ❌ Authentication service not available
Sprawdź logi serwera - prawdopodobnie Redis nie działa lub MySQL nie jest dostępny.

### ❌ Can't connect to MySQL
```bash
docker ps --filter name=mysql
docker exec netsim-mysql mysqladmin ping -h localhost -pNetSimCPP1234
```

### ❌ Compilation errors
```bash
cd /home/patryk/inzynieria_oprogramowania/project/backend/build
rm -rf *
cmake ..
make -j$(nproc)
```

## Dokumentacja

- **Pełna dokumentacja autentykacji**: `docs/AUTHENTICATION.md`
- **Bezpieczeństwo API**: `docs/API_SECURITY.md`
- **Podsumowanie implementacji**: `docs/AUTH_IMPLEMENTATION_SUMMARY.md`
- **Przewodnik testowy**: `TESTING_GUIDE.md`
