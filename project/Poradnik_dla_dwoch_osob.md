# Poradnik dla Dwu Osób: Implementacja API i Frontendu dla Network Simulator

Cześć! Ten poradnik jest napisany bardzo prosto, krok po kroku, jakbyś tłumaczył dziecku. Mamy dwie osoby:

- **Osoba 1 (API Developer)**: Adrian – zajmuje się backendem, czyli serwerem API w C++ z cpprestsdk.
- **Osoba 2 (Frontend Developer)**: Oliwiers – zajmuje się frontendem, czyli stroną internetową w React z Vite i Tailwind.

Każda osoba ma swoje zadania. Czytajcie swoje sekcje i róbcie krok po kroku. Jeśli coś nie działa, pytajcie się nawzajem lub sprawdzajcie kod razem.

## Ogólne Wskazówki Przed Rozpoczęciem

1. **Środowisko**: Upewnijcie się, że macie zainstalowane narzędzia:
   - Dla API: CMake, C++ compiler, cpprestsdk, nlohmann/json.
   - Dla Frontend: Node.js, npm, Vite, React, Tailwind CSS, React Flow.

2. **Komunikacja**: Pracujcie razem. Osoba 1 buduje API, Osoba 2 testuje je z frontendu.

3. **Testowanie**: Po każdym kroku uruchamiajcie testy (dla API: `./netsim_tests`, dla Frontend: `npm run dev`).

4. **Commit**: Po zakończeniu zadania zróbcie commit z opisem, np. "Dodano endpoint /addNode".

Teraz idziemy do zadań!

---

## Osoba 1: API Developer (Adrian) – Backend w C++

Twoje zadanie to stworzyć serwer HTTP, który obsługuje żądania od frontendu. Używasz C++ z cpprestsdk. Wszystko budujesz przez CMake.

### Krok 1: Przygotuj Szkielet Serwera (Endpoint /status)

1. Otwórz plik `main.cpp` w `project/backend/src/`.

2. Dodaj kod do uruchomienia serwera na `http://0.0.0.0:8080`.

3. Dodaj endpoint GET `/status`:
   - Zwróć JSON: `{ "status": "ok" }`.

4. Skompiluj: `cd build && make`.

5. Uruchom: `./netsim`.

6. Test: Otwórz przeglądarkę, idź na `http://localhost:8080/status` – powinieneś zobaczyć JSON.

### Krok 2: Dodaj Endpoint /addNode

1. W `main.cpp` dodaj POST `/addNode`.

2. Odbierz JSON: `{ "name": "A", "ip": "10.0.0.1" }`.

3. Stwórz Host i dodaj do Network.

4. Zwróć: `{ "result": "node added" }`.

5. Test: Użyj curl: `curl -X POST http://localhost:8080/addNode -H "Content-Type: application/json" -d '{"name":"A","ip":"10.0.0.1"}'`.

### Krok 3: Dodaj Endpointy /connect i /ping

1. Dodaj POST `/connect`: Odbierz `{ "a": "A", "b": "B" }`, połącz węzły w Network.

2. Dodaj POST `/ping`: Odbierz `{ "src": "A", "dst": "B" }`, wywołaj Engine::ping(), zwróć ścieżkę jako JSON.

3. Test: Połącz węzły, potem pinguj.

### Krok 4: Dodaj Endpoint /network

1. Dodaj GET `/network`.

2. Zwróć JSON z węzłami i linkami, jak w issues.

3. Test: Dodaj węzły i połączenia, sprawdź JSON.

### Krok 5: Dodaj Endpoint /removeNode

1. Dodaj POST `/removeNode`: `{ "name": "NodeName" }`.

2. Wywołaj Network::removeNode(), obsłuż błędy.

3. Zwróć sukces lub błąd.

### Krok 6: Dodaj Endpoint /disconnect

1. Dodaj POST `/disconnect`: `{ "a": "A", "b": "B" }`.

2. Wywołaj Network::disconnect().

### Krok 7: Dodaj Endpoints dla Link Delay (/setLinkDelay, /getLinkDelay)

1. POST `/setLinkDelay`: `{ "a": "A", "b": "B", "delay": 50 }`.

2. GET `/getLinkDelay?a=A&b=B`: Zwróć opóźnienie.

### Krok 8: Dodaj Endpoint /packetStats

1. GET `/packetStats`: Zwróć JSON z licznikami pakietów.

### Krok 9: Dodaj Endpoints dla VLAN (/assignVLAN, /getVLAN, /canCommunicate)

1. POST `/assignVLAN`: `{ "node": "A", "vlan": 10 }`.

2. GET `/getVLAN?node=A`.

3. GET `/canCommunicate?a=A&b=B`.

### Krok 10: Dodaj Endpoints dla Eksportu/Importu (/exportTopology, /importTopology)

1. GET `/exportTopology`: Zwróć JSON topologii.

2. POST `/importTopology`: Załaduj JSON.

### Krok 11: Dodaj Endpoints dla Node Failure (/failNode, /repairNode, /isFailed)

1. POST `/failNode`: `{ "name": "NodeName" }`.

2. POST `/repairNode`: `{ "name": "NodeName" }`.

3. GET `/isFailed?name=NodeName`.

### Krok 12: Dodaj Endpoint /traceroute

1. POST `/traceroute`: `{ "src": "A", "dst": "C" }`.

2. Zwróć `{ "success": true, "path": ["A", "B", "C"] }`.

### Krok 13: Dodaj Endpoints dla Bandwidth (/setBandwidth, /getBandwidth, /consumeBandwidth)

1. POST `/setBandwidth`: `{ "a": "A", "b": "B", "bandwidth": 100 }`.

2. GET `/getBandwidth?a=A&b=B`.

3. POST `/consumeBandwidth`: `{ "a": "A", "b": "B", "amount": 10 }`.

### Krok 14: Dodaj Endpoints dla Firewall (/addFirewallRule, /isAllowed, /removeFirewallRule)

1. POST `/addFirewallRule`: `{ "node": "A", "protocol": "tcp", "allow": false }`.

2. GET `/isAllowed?node=A&protocol=tcp`.

3. DELETE `/removeFirewallRule`: `{ "node": "A", "protocol": "tcp" }`.

Po każdym kroku testuj z curl lub Postman.

---

## Osoba 2: Frontend Developer (Oliwier) – Frontend w React

Twoje zadanie to stworzyć stronę internetową, która rozmawia z API Osoby 1. Używasz React, Vite, Tailwind, React Flow.

### Krok 1: Przygotuj Szkielet React (Vite + Tailwind)

1. W folderze `project` stwórz folder `frontend`.

2. Uruchom: `npm create vite@latest frontend -- --template react`.

3. Wejdź do `frontend`, zainstaluj: `npm install`.

4. Dodaj Tailwind: `npm install -D tailwindcss postcss autoprefixer`, potem `npx tailwindcss init -p`.

5. W `tailwind.config.js` dodaj ścieżki do plików.

6. W `src/App.jsx` dodaj prosty layout: sidebar i canvas.

7. Uruchom: `npm run dev`.

8. Test: Otwórz `http://localhost:5173`, zobacz stronę.

### Krok 2: Fetch /status i Formularz /addNode

1. W komponencie dodaj funkcję fetch do `/status`.

2. Jeśli ok, pokaż zielone światło.

3. Dodaj formularz: pola name i ip, przycisk "Add Node".

4. Na kliknięcie fetch POST do `/addNode`.

5. Test: Dodaj węzeł, sprawdź w konsoli.

### Krok 3: Rysowanie Sieci z /network (React Flow)

1. Zainstaluj React Flow: `npm install @xyflow/react`.

2. W canvas użyj ReactFlow.

3. Fetch GET `/network`, parsuj JSON.

4. Rysuj węzły i krawędzie.

5. Test: Dodaj węzły przez API, odśwież stronę – zobacz graf.

### Krok 4: Animacja Ping

1. Dodaj przycisk "Ping" między węzłami.

2. Na kliknięcie fetch POST `/ping`.

3. Animuj: podświetl krawędzie na zielono po kolei.

4. Użyj setTimeout dla animacji.

### Krok 5: UI do Usuwania Węzłów

1. Przy każdym węźle dodaj przycisk "Delete".

2. Na kliknięcie potwierdź (alert), potem fetch POST `/removeNode`.

3. Odśwież graf.

### Krok 6: UI do Rozłączania Węzłów

1. Na krawędzi dodaj menu kontekstowe (klik prawy).

2. Opcja "Disconnect", fetch POST `/disconnect`.

3. Odśwież.

### Krok 7: UI do Opóźnień Łączy

1. Na krawędzi przycisk "Configure Delay".

2. Otwórz modal ze sliderem dla delay (ms).

3. Fetch POST `/setLinkDelay`.

4. Wizualnie: grubsza linia dla większego delay.

### Krok 8: Dashboard ze Statystykami Pakietów

1. Dodaj panel boczny.

2. Fetch GET `/packetStats`, pokaż tabelę.

3. Dodaj wykres (np. proste divy jako słupki).

### Krok 9: Wizualizacja VLAN

1. Koloruj węzły: fetch GET `/getVLAN`, przypisz kolory.

2. Dodaj legendę.

3. Filtr: przyciski do pokazywania tylko jednego VLAN.

### Krok 10: Zapis/Wczytywanie Topologii

1. Przycisk "Save": fetch GET `/exportTopology`, pobierz jako plik JSON.

2. Przycisk "Load": wybierz plik, przeczytaj JSON, fetch POST `/importTopology`.

3. Potwierdź przed zastąpieniem.

### Krok 11: Symulacja Awarii w UI

1. Przy węźle przycisk "Fail": fetch POST `/failNode`, zmień kolor na czerwony.

2. Przycisk "Repair": fetch POST `/repairNode`, przywróć kolor.

3. Wizualnie: przekreśl failed węzły.

### Krok 12: Wizualizacja Traceroute

1. Przycisk "Traceroute" obok "Ping".

2. Fetch POST `/traceroute`, animuj całą ścieżkę.

3. Pokaż wynik w panelu.

### Krok 13: Wizualizacja Przepustowości

1. Na krawędzi slider dla bandwidth.

2. Fetch POST `/setBandwidth`.

3. Kolor linii: zielony (wolny), czerwony (zajęty).

4. Tooltip: fetch GET `/getBandwidth`.

### Krok 14: Konfiguracja Firewall w UI

1. Przy węźle panel ustawień.

2. Lista reguł: dodaj/usuwaj.

3. Fetch POST `/addFirewallRule`, DELETE `/removeFirewallRule`.

4. Wizualnie: przerywana linia dla zablokowanych.

Po każdym kroku testuj z API Osoby 1.

---

## Końcowe Wskazówki

- **Współpraca**: Osoba 1 uruchamia serwer, Osoba 2 testuje frontend.
- **Błędy**: Jeśli coś nie działa, sprawdźcie konsolę przeglądarki i logi serwera.
- **Demo**: Na koniec nagrajcie filmik pokazujący wszystko.
- **Pytania**: Jeśli utkniecie, pytajcie się!

