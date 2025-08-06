#!/bin/bash

echo "🚀 Building Simple C++ File Compressor..."

# Create build directory
mkdir -p build_simple
cd build_simple

# Create a simple .pro file for qmake
cat > SimpleCompressor.pro << 'EOF'
QT += core
CONFIG += c++17 console

TARGET = SimpleCompressor
TEMPLATE = app

SOURCES += ../src/simple_main.cpp

# macOS specific
macx {
    LIBS += -lz
}

# Linux specific
unix:!macx {
    LIBS += -lz
}

# Windows specific
win32 {
    LIBS += -lzlib1
}
EOF

# Run qmake and make
echo "📦 Running qmake..."
qmake SimpleCompressor.pro

echo "🔨 Building with make..."
make

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "🎯 Executable created: ./SimpleCompressor"
    echo "🚀 You can run it with: ./SimpleCompressor <archivo>"
    echo ""
    echo "📝 Example usage:"
    echo "   ./SimpleCompressor ../test.txt"
    echo "   ./SimpleCompressor ../image.jpg"
else
    echo "❌ Build failed!"
    exit 1
fi
