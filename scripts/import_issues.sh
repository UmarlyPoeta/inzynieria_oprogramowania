#!/usr/bin/env bash
# Importuje issues z .github/issues.yml do GitHuba
# Wymaga: gh, yq

set -e

ISSUES_FILE=".github/issues.yml"

if [ ! -f "$ISSUES_FILE" ]; then
  echo "❌ Plik $ISSUES_FILE nie istnieje."
  exit 1
fi

echo "🚀 Importuję issues z $ISSUES_FILE ..."

count=$(yq '.issues | length' "$ISSUES_FILE")

for i in $(seq 0 $((count - 1))); do
  title=$(yq -r ".issues[$i].title" "$ISSUES_FILE")
  body=$(yq -r ".issues[$i].body" "$ISSUES_FILE")
  labels=$(yq -r ".issues[$i].labels | join(\",\")" "$ISSUES_FILE")
  assignees=$(yq -r ".issues[$i].assignees | join(\",\")" "$ISSUES_FILE")

  echo "📄 Tworzę issue: $title"

  gh issue create \
    --title "$title" \
    --body "$body" \
    $( [ -n "$labels" ] && echo --label "$labels" ) \
    $( [ -n "$assignees" ] && echo --assignee "$assignees" )

done

echo "✅ Wszystkie issues zostały utworzone!"
