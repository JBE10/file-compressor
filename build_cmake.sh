#!/bin/bash

echo "🚀 Building GUI File Compressor with CMake..."

# Set Qt5 environment
export PKG_CONFIG_PATH="/opt/homebrew/Cellar/qt@5/5.15.17/lib/pkgconfig"

# Create build directory
mkdir -p build_cmake
cd build_cmake

# Clean previous build
rm -rf *

# Configure with CMake
echo "📦 Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

# Build
echo "🔨 Building with make..."
make -j$(sysctl -n hw.ncpu)

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "🎯 GUI Application created!"
    echo ""
    echo "📝 To run the GUI application:"
    echo "   cd build_cmake"
    echo "   ./gui_compressor"
    echo ""
    echo "🚀 Or run directly:"
    echo "   ./build_cmake/gui_compressor"
else
    echo "❌ Build failed!"
    exit 1
fi
