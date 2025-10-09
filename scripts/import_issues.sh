#!/usr/bin/env bash
set -euo pipefail

echo "🚀 Importuję issues z .github/issues.yml ..."

# Sprawdzenie, czy wymagane programy są zainstalowane
if ! command -v gh >/dev/null 2>&1; then
  echo "❌ Brak 'gh' (GitHub CLI). Zainstaluj: sudo dnf install gh"
  exit 1
fi

if ! command -v yq >/dev/null 2>&1; then
  echo "❌ Brak 'yq'. Zainstaluj: sudo dnf install yq"
  exit 1
fi

# Pobranie repozytorium
REPO=$(gh repo view --json nameWithOwner -q .nameWithOwner)

# Liczba issues w YAML-u
count=$(yq eval '.issues | length' .github/issues.yml)

for i in $(seq 0 $((count - 1))); do
  title=$(yq eval ".issues[$i].title" .github/issues.yml)
  body=$(yq eval ".issues[$i].body // \"\"" .github/issues.yml) # obsługa pustego body

  echo "📄 Sprawdzam issue: $title"

  # Sprawdź, czy issue już istnieje
  if gh issue list --repo "$REPO" --search "$title" --limit 100 | grep -Fq "$title"; then
    echo "ℹ️  Issue już istnieje: $title, pomijam..."
    continue
  fi

  echo "📄 Tworzę nowe issue: $title"
  if ! gh issue create \
        --title "$title" \
        --body "$body" \
        --repo "$REPO" \
        >/dev/null; then
    echo "⚠️  Nie udało się utworzyć: $title"
  fi
done

echo "✅ Import zakończony!"
