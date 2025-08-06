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
