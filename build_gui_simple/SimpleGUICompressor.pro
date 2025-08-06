QT += core widgets concurrent
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
    QMAKE_LFLAGS += -framework Cocoa -framework OpenGL -framework QtConcurrent
}

# Linux specific
unix:!macx {
    LIBS += -lz
}

# Windows specific
win32 {
    LIBS += -lzlib1
}
