#!/bin/bash

echo "🚀 Building GUI File Compressor (Manual Build)..."

# Set Qt5 environment
export PKG_CONFIG_PATH="/opt/homebrew/Cellar/qt@5/5.15.17/lib/pkgconfig"

# Create build directory
mkdir -p build_gui_manual
cd build_gui_manual

# Clean previous build
rm -rf *

# Generate MOC files
echo "📦 Generating MOC files..."
/opt/homebrew/share/qt/libexec/moc ../include/gui_mainwindow.h -o moc_gui_mainwindow.cpp

# Compile source files
echo "🔨 Compiling source files..."

# Compile gui_main.cpp
g++ -c ../src/gui_main.cpp \
    -I../include \
    -I/opt/homebrew/include \
    -std=c++17 \
    `pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui Qt5Concurrent` \
    -o gui_main.o

# Compile gui_mainwindow.cpp
g++ -c ../src/gui_mainwindow.cpp \
    -I../include \
    -I/opt/homebrew/include \
    -std=c++17 \
    `pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui Qt5Concurrent` \
    -o gui_mainwindow.o

# Compile gui_compressor.cpp
g++ -c ../src/gui_compressor.cpp \
    -I../include \
    -I/opt/homebrew/include \
    -std=c++17 \
    -o gui_compressor.o

# Compile MOC file
g++ -c moc_gui_mainwindow.cpp \
    -I../include \
    -I/opt/homebrew/include \
    -std=c++17 \
    `pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui Qt5Concurrent` \
    -o moc_gui_mainwindow.o

# Link everything together
echo "🔗 Linking..."
g++ gui_main.o gui_mainwindow.o gui_compressor.o moc_gui_mainwindow.o \
    -o gui_compressor \
    -L/opt/homebrew/lib \
    -lz -lzip \
    `pkg-config --libs Qt5Widgets Qt5Core Qt5Gui Qt5Concurrent` \
    -framework AppKit

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "🎯 GUI Application created!"
    echo ""
    echo "📝 To run the GUI application:"
    echo "   cd build_gui_manual"
    echo "   ./gui_compressor"
    echo ""
    echo "🚀 Or run directly:"
    echo "   ./build_gui_manual/gui_compressor"
else
    echo "❌ Build failed!"
    exit 1
fi
