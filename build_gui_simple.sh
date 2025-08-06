#!/bin/bash

echo "🚀 Building Simple GUI File Compressor..."

# Create build directory
mkdir -p build_gui_simple
cd build_gui_simple

# Create a simple .pro file for qmake
cat > SimpleGUICompressor.pro << 'EOF'
QT += core widgets
CONFIG += c++17

TARGET = gui_compressor_simple
TEMPLATE = app

SOURCES += ../src/gui_main.cpp \
           ../src/gui_mainwindow_simple.cpp \
           ../src/gui_compressor.cpp

HEADERS += ../include/gui_mainwindow.h \
           ../include/gui_compressor.h

INCLUDEPATH += ../include

# Disable AGL framework
macx {
    LIBS += -lz
    QMAKE_LFLAGS += -framework Cocoa -framework OpenGL
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
qmake SimpleGUICompressor.pro

echo "🔨 Building with make..."
make

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "🎯 GUI Application created!"
    echo ""
    echo "📝 To run the GUI application:"
    echo "   cd build_gui_simple"
    echo "   ./gui_compressor_simple"
    echo ""
    echo "🚀 Or run directly:"
    echo "   ./build_gui_simple/gui_compressor_simple"
else
    echo "❌ Build failed!"
    exit 1
fi 