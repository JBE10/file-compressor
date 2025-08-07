#!/bin/bash

echo "🚀 Launching GUI File Compressor..."

# Check if the application exists
if [ -f "./build_cmake/gui_compressor.app/Contents/MacOS/gui_compressor" ]; then
    echo "✅ Found GUI application, launching..."
    ./build_cmake/gui_compressor.app/Contents/MacOS/gui_compressor
elif [ -f "./build_gui/gui_compressor" ]; then
    echo "✅ Found GUI application (qmake build), launching..."
    ./build_gui/gui_compressor
else
    echo "❌ GUI application not found!"
    echo "Please build the application first using:"
    echo "   ./build_cmake.sh"
    echo "   or"
    echo "   ./build_gui.sh"
    exit 1
fi
