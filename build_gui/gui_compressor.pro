QT += core widgets concurrent
CONFIG += c++17

TARGET = gui_compressor
TEMPLATE = app

SOURCES += ../src/gui_main.cpp \
           ../src/gui_mainwindow_simple.cpp \
           ../src/gui_compressor.cpp

HEADERS += ../include/gui_mainwindow.h \
           ../include/gui_compressor.h

INCLUDEPATH += ../include

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

# Application info
VERSION = 2.0.0
QMAKE_TARGET_COMPANY = "Juan Bautista Espino"
QMAKE_TARGET_PRODUCT = "Compresor de Archivos GUI"
QMAKE_TARGET_DESCRIPTION = "Aplicación de compresión de archivos con interfaz gráfica"
QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2024 Juan Bautista Espino"

# Icon (if available)
# ICON = resources/icon.icns

# Bundle settings for macOS
# macx {
#     QMAKE_INFO_PLIST = resources/Info.plist
# }
