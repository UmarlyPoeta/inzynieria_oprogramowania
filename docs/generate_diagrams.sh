#!/bin/bash

# Generate PlantUML diagrams to PNG using Docker
# Usage: ./generate_diagrams.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
UML_DIR="${SCRIPT_DIR}"

echo "🎨 Generating PlantUML diagrams..."
echo "📁 Directory: ${UML_DIR}"
echo ""

# Check if Docker is available
if ! command -v docker &> /dev/null; then
    echo "❌ Docker is not installed. Please install Docker first."
    echo ""
    echo "Install Docker:"
    echo "  curl -fsSL https://get.docker.com -o get-docker.sh"
    echo "  sudo sh get-docker.sh"
    exit 1
fi

# Count .puml files
PUML_COUNT=$(find "${UML_DIR}" -name "*.puml" | wc -l)
echo "📊 Found ${PUML_COUNT} PlantUML files"
echo ""

# Generate PNG files
echo "🔄 Generating PNG diagrams..."
docker run --rm -v "${UML_DIR}:/data" plantuml/plantuml:latest -tpng /data/*.puml

if [ $? -eq 0 ]; then
    echo "✅ PNG diagrams generated successfully!"
else
    echo "❌ Failed to generate PNG diagrams"
    exit 1
fi

echo ""
echo "🔄 Generating SVG diagrams..."
docker run --rm -v "${UML_DIR}:/data" plantuml/plantuml:latest -tsvg /data/*.puml

if [ $? -eq 0 ]; then
    echo "✅ SVG diagrams generated successfully!"
else
    echo "❌ Failed to generate SVG diagrams"
    exit 1
fi

echo ""
echo "📊 Generated files:"
ls -lh "${UML_DIR}"/*.png 2>/dev/null || echo "  (No PNG files)"
ls -lh "${UML_DIR}"/*.svg 2>/dev/null || echo "  (No SVG files)"

echo ""
echo "✅ All diagrams generated successfully!"
echo ""
echo "📁 Files located in: ${UML_DIR}"
echo ""
echo "🔍 To view diagrams:"
echo "  • Open PNG files in any image viewer"
echo "  • Open SVG files in browser or vector editor"
echo "  • Use VS Code PlantUML extension for live preview"
echo ""
