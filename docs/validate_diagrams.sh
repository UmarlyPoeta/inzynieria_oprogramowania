#!/bin/bash

# Validate PlantUML diagrams syntax
# Usage: ./validate_diagrams.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
UML_DIR="${SCRIPT_DIR}"

echo "🔍 Validating PlantUML diagrams syntax..."
echo "📁 Directory: ${UML_DIR}"
echo ""

# Check if Docker is available
if ! command -v docker &> /dev/null; then
    echo "❌ Docker is not installed. Please install Docker first."
    exit 1
fi

# Count .puml files
PUML_COUNT=$(find "${UML_DIR}" -name "*.puml" | wc -l)
echo "📊 Found ${PUML_COUNT} PlantUML files"
echo ""

SUCCESS_COUNT=0
ERROR_COUNT=0

# Validate each .puml file
for file in "${UML_DIR}"/*.puml; do
    if [ -f "$file" ]; then
        filename=$(basename "$file")
        echo -n "Validating ${filename}... "
        
        # Try to generate PNG (syntax check)
        if docker run --rm -v "${UML_DIR}:/data" plantuml/plantuml:latest -tpng -syntax "/data/${filename}" > /dev/null 2>&1; then
            echo "✅ OK"
            ((SUCCESS_COUNT++))
        else
            echo "❌ SYNTAX ERROR"
            ((ERROR_COUNT++))
            # Show detailed error
            docker run --rm -v "${UML_DIR}:/data" plantuml/plantuml:latest -tpng -syntax "/data/${filename}"
        fi
    fi
done

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📊 Validation Summary:"
echo "   Total files: ${PUML_COUNT}"
echo "   ✅ Valid: ${SUCCESS_COUNT}"
echo "   ❌ Errors: ${ERROR_COUNT}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

if [ ${ERROR_COUNT} -eq 0 ]; then
    echo "✅ All PlantUML diagrams are valid!"
    exit 0
else
    echo "❌ Some diagrams have syntax errors. Please fix them."
    exit 1
fi
