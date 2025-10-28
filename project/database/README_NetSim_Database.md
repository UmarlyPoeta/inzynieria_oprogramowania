# 🧩 NetSim Database Setup Guide

Ten plik opisuje konfigurację i uruchomienie bazy danych **NetSim** w MySQL.

---

## ⚙️ 1. Wymagania

- MySQL 8.0+ (lub XAMPP z modułem MySQL)
- Alternatywnie: Docker (zalecane dla spójności między środowiskami)

Sprawdź wersję MySQL:
```bash
mysql --version
```

---

## 🏗️ 2. Utworzenie bazy danych

1. Zaloguj się do MySQL:
   ```bash
   mysql -u root -p
   ```
2. Utwórz bazę danych:
   ```sql
   CREATE DATABASE netsim;
   USE netsim;
   ```
3. Zainportuj strukturę z pliku `schema.sql`:
   ```bash
   mysql -u root -p netsim < database/schema.sql
   ```

---

## 📁 3. Struktura folderu

```
database/
├── schema.sql         # Struktura bazy danych
├── README.md          # Ten plik z instrukcjami
└── .env.example       # Przykładowy plik konfiguracji
```

---

## 🔐 4. Konfiguracja połączenia

Utwórz plik `.env` w folderze głównym projektu:

```env
DB_HOST=127.0.0.1
DB_PORT=3306
DB_USER=root
DB_PASS=root
DB_NAME=netsim
```

Jeśli masz inne hasło MySQL, zmień `DB_PASS`.

Dodaj `.env` do `.gitignore`:
```
.env
```

---

## 🐳 5. Uruchomienie przez Dockera

Jeśli nie chcesz instalować MySQL lokalnie:

```bash
docker run --name netsim-db -e MYSQL_ROOT_PASSWORD=root -e MYSQL_DATABASE=netsim -p 3306:3306 -d mysql:8.0
```

Zaimportuj strukturę:
```bash
docker exec -i netsim-db mysql -uroot -proot netsim < database/schema.sql
```

Sprawdź tabele:
```bash
docker exec -it netsim-db mysql -uroot -proot -e "SHOW TABLES IN netsim;"
```

---

## 🧩 6. Eksport i aktualizacja

Jeśli zmienisz strukturę, wyeksportuj ją ponownie:

```bash
mysqldump -u root -p netsim > database/schema.sql
```

Nie commituj plików `.ibd`, `.frm`, `.MYD` — tylko `schema.sql`.

---

## 🧰 7. Przykładowy `.env.example`

```
DB_HOST=127.0.0.1
DB_PORT=3306
DB_USER=root
DB_PASS=root
DB_NAME=netsim
```

Każdy członek zespołu tworzy własny `.env`:
```bash
cp .env.example .env
```

---

## 🧱 8. Szybki skrót

| Zadanie | Komenda |
|----------|----------|
| Utworzenie bazy | `CREATE DATABASE netsim;` |
| Import struktury | `mysql -u root -p netsim < database/schema.sql` |
| Eksport struktury | `mysqldump -u root -p netsim > database/schema.sql` |
| Uruchomienie w Dockerze | `docker run --name netsim-db -e MYSQL_ROOT_PASSWORD=root -e MYSQL_DATABASE=netsim -p 3306:3306 -d mysql:8.0` |

---

## 📘 9. Rozwiązywanie problemów

### ❌ `Access denied for user 'root'@'localhost'`
Zmień hasło lub sposób logowania:
```sql
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'root';
```

### ❌ `Can't connect to MySQL server on '127.0.0.1'`
Upewnij się, że MySQL działa (XAMPP → Start MySQL lub `net start mysql`).

---

## ✨ 10. Autorzy

**Zespół projektu _NetSim_**  
📅 Aktualizacja: 2025-10-13  
📂 Repozytorium: [Twój link do GitHub]