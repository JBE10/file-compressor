#!/bin/bash

echo "🧪 Testing GUI Buttons..."

# Set Qt5 environment
export PKG_CONFIG_PATH="/opt/homebrew/Cellar/qt@5/5.15.17/lib/pkgconfig"

# Compile test
echo "🔨 Compiling test..."
g++ -c test_gui.cpp \
    -std=c++17 \
    `pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui` \
    -o test_gui.o

# Generate MOC
echo "📦 Generating MOC..."
/opt/homebrew/share/qt/libexec/moc test_gui.cpp -o moc_test_gui.cpp

# Compile MOC
g++ -c moc_test_gui.cpp \
    -std=c++17 \
    `pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui` \
    -o moc_test_gui.o

# Link
echo "🔗 Linking..."
g++ test_gui.o moc_test_gui.o \
    -o test_gui \
    `pkg-config --libs Qt5Widgets Qt5Core Qt5Gui` \
    -framework AppKit

if [ $? -eq 0 ]; then
    echo "✅ Test compiled successfully!"
    echo "🚀 Running test..."
    ./test_gui
else
    echo "❌ Test compilation failed!"
    exit 1
fi
