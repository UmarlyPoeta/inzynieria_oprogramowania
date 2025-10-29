#!/bin/bash

echo "Checking compilation status..."

if [ -f "/home/patryk/inzynieria_oprogramowania/project/backend/build/NetSimCPP" ]; then
    echo "✓ Compilation SUCCESSFUL!"
    ls -lh /home/patryk/inzynieria_oprogramowania/project/backend/build/NetSimCPP
    echo ""
    echo "Next steps:"
    echo "  1. cd /home/patryk/inzynieria_oprogramowania/project/backend/build"
    echo "  2. ./run_server.sh"
else
    echo "✗ NetSimCPP executable not found"
    echo ""
    echo "Check for errors:"
    if [ -f "/tmp/compile.log" ]; then
        echo "Checking compile log for errors..."
        grep -i "error" /tmp/compile.log | head -10
    else
        echo "Run: cd /home/patryk/inzynieria_oprogramowania/project/backend/build && make -j$(nproc)"
    fi
fi
