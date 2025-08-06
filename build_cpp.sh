#!/bin/bash

echo "🚀 Building C++ File Compressor..."

# Create build directory
mkdir -p build_qmake
cd build_qmake

# Create a simple .pro file for qmake
cat > FileCompressor.pro << 'EOF'
QT += core widgets concurrent
CONFIG += c++17

TARGET = FileCompressor
TEMPLATE = app

SOURCES += ../src/main.cpp \
           ../src/mainwindow.cpp \
           ../src/compressor_simple.cpp \
           ../src/progressdialog.cpp

HEADERS += ../include/mainwindow.h \
           ../include/compressor.h \
           ../include/progressdialog.h

INCLUDEPATH += ../include

# macOS specific
macx {
    LIBS += -lz
}

# Linux specific
unix:!macx {
    LIBS += -lz -lpng -ljpeg
}

# Windows specific
win32 {
    LIBS += -lzlib1
}
EOF

# Run qmake and make
echo "📦 Running qmake..."
qmake FileCompressor.pro

echo "🔨 Building with make..."
make

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "🎯 Executable created: ./FileCompressor"
    echo "🚀 You can run it with: ./FileCompressor"
else
    echo "❌ Build failed!"
    exit 1
fi
