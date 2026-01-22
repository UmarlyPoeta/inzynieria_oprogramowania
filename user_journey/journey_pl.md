# Podróż Użytkownika (User Journey)

## Persona

**Bożena**, studentka informatyki na drugim roku. Musi przygotować projekt na zajęcia z "Sieci Komputerowych". Po raz pierwszy korzysta z aplikacji NetSim. Jest obeznana z technologią, ale nie ma czasu na czytanie długich dokumentacji.

## Scenariusz i Cel

**Scenariusz:** Bożena otwiera aplikację NetSim, aby zbudować prostą topologię sieciową składającą się z dwóch routerów i dwóch hostów.
**Cel:** Pomyślnie skonfigurować sieć i zweryfikować połączenie między hostami za pomocą polecenia `ping`.

## Mapa Podróży Użytkownika

| Faza | Działania (Actions) | Myśli (Thoughts) | Emocje (Emotions) | Punkty Bólu (Pain Points) | Szanse (Opportunities) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **1. Odkrywanie (Discovery)** | Loguje się do aplikacji. Rozgląda się po interfejsie w poszukiwaniu punktu startowego. | "OK, interfejs wygląda czysto. Gdzie jest przycisk 'Nowy Projekt' albo 'Stwórz Sieć'?" | Zaciekawiona, lekko zdezorientowana | Główny przycisk akcji (call-to-action) nie jest od razu widoczny. Nie jest jasne, od czego zacząć. | Wyróżnić przycisk "Nowy Projekt". Dodać ekran powitalny z krótkim przewodnikiem dla nowych użytkowników. |
| **2. Budowanie (Building)** | Znajduje edytor. Przeciąga ikony routerów i hostów na planszę. Próbuje je połączyć. | "Super, przeciągnij i upuść działa! Ale jak mam teraz połączyć te dwa urządzenia? Ta ikona to kabel?" | Zadowolona -> Zdezorientowana | Ikony narzędzi (np. "kabel") są niejasne i nie mają etykiet. Brak intuicyjnej metody łączenia węzłów. | Dodać etykiety (tooltips) do ikon narzędzi. Podświetlić kompatybilne porty na urządzeniach po wybraniu narzędzia "kabel". |
| **3. Konfiguracja (Configuration)** | Klika na hosta, szukając miejsca do wpisania adresu IP. Próbuje kliknąć prawym przyciskiem myszy. | "Gdzie są ustawienia? Muszę kliknąć prawym przyciskiem? A może podwójne kliknięcie?" | Sfrustrowana | Menu konfiguracyjne jest ukryte i wymaga odgadnięcia interakcji (np. podwójne kliknięcie). | Wyświetlać podstawowe opcje konfiguracji w bocznym panelu po pojedynczym kliknięciu na węzeł. |
| **4. Symulacja (Simulation)** | Znajduje i klika przycisk "Start Symulacji". Otwiera konsolę jednego z hostów. Wpisuje `ping <adres_ip>`. | "OK, symulacja ruszyła. Zobaczmy, czy pakiety dojdą. Chciałabym to zobaczyć." | Podekscytowana, pełna nadziei | Brak wizualnej informacji zwrotnej na schemacie, że symulacja jest aktywna i że pakiety są przesyłane. | Dodać animację pakietów przemieszczających się po kablach w trakcie symulacji. Zmienić kolor linków, gdy dane są przesyłane. |
| **5. Analiza (Analysis)** | Patrzy na wynik polecenia `ping` w konsoli. | "Sukces! Ping doszedł. A co jeśli by nie doszedł? Gdzie szukać problemu?" | Zadowolona, ale zamyślona | Aplikacja nie oferuje narzędzi do debugowania w przypadku niepowodzenia pingu (np. śledzenia pakietów). | Dodać tryb inspekcji, który pozwala klikać na pakiety, aby zobaczyć ich zawartość i status. Dodać panel z logami zdarzeń symulacji. |
