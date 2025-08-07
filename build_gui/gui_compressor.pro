QT += core widgets
CONFIG += c++17
CONFIG += sdk_no_version_check

TARGET = gui_compressor
TEMPLATE = app

SOURCES += ../src/gui_main.cpp \
           ../src/gui_mainwindow.cpp \
           ../src/gui_compressor.cpp

HEADERS += ../include/gui_mainwindow.h \
           ../include/gui_compressor.h

INCLUDEPATH += ../include

# Qt5 compatibility
DEFINES += QT_DEPRECATED_WARNINGS

# macOS specific
macx {
    INCLUDEPATH += /opt/homebrew/include
    LIBS += -L/opt/homebrew/lib -lz -lzip
    
    # Exclude AGL framework on newer macOS versions
    QMAKE_LFLAGS += -framework AppKit
}

# Linux specific
unix:!macx {
    LIBS += -lz -lzip
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
