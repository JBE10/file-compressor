#!/bin/bash

echo "🚀 Building GUI File Compressor..."

# Create build directory
mkdir -p build_gui
cd build_gui

# Copy the project file
cp ../gui_compressor.pro .

# Run qmake and make
echo "📦 Running qmake..."
qmake gui_compressor.pro

echo "🔨 Building with make..."
make

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "🎯 GUI Application created!"
    echo ""
    echo "📝 To run the GUI application:"
    echo "   cd build_gui"
    echo "   ./gui_compressor"
    echo ""
    echo "🚀 Or run directly:"
    echo "   ./build_gui/gui_compressor"
else
    echo "❌ Build failed!"
    exit 1
fi 