cmake_minimum_required(VERSION 3.16)
project(GuiCompressor VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt5 path
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Find required libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZLIB REQUIRED zlib)
pkg_check_modules(LIBZIP REQUIRED libzip)

# Set source files
set(SOURCES
    src/gui_main.cpp
    src/gui_mainwindow.cpp
    src/gui_compressor.cpp
)

set(HEADERS
    include/gui_mainwindow.h
    include/gui_compressor.h
)

# Create executable
add_executable(gui_compressor ${SOURCES} ${HEADERS})

# Set include directories
target_include_directories(gui_compressor PRIVATE
    include
    ${ZLIB_INCLUDE_DIRS}
    ${LIBZIP_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(gui_compressor
    Qt5::Core
    Qt5::Widgets
    ${ZLIB_LIBRARIES}
    ${LIBZIP_LIBRARIES}
)

# Set compile definitions
target_compile_definitions(gui_compressor PRIVATE
    QT_DEPRECATED_WARNINGS
)

# Set compiler flags
target_compile_options(gui_compressor PRIVATE
    ${ZLIB_CFLAGS_OTHER}
    ${LIBZIP_CFLAGS_OTHER}
)

# Set linker flags
target_link_options(gui_compressor PRIVATE
    ${ZLIB_LDFLAGS}
    ${LIBZIP_LDFLAGS}
)

# macOS specific settings
if(APPLE)
    set_target_properties(gui_compressor PROPERTIES
        MACOSX_BUNDLE TRUE
    )
    
    # Add AppKit framework
    target_link_libraries(gui_compressor "-framework AppKit")
endif()

# Install target
install(TARGETS gui_compressor
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
