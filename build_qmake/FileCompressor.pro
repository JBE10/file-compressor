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
