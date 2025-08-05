cmake_minimum_required(VERSION 3.16)
project(FileCompressor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Find required libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPNG REQUIRED libpng)
pkg_check_modules(JPEG REQUIRED libjpeg)
pkg_check_modules(ZLIB REQUIRED zlib)

# Include directories
include_directories(${LIBPNG_INCLUDE_DIRS})
include_directories(${JPEG_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/compressor.cpp
    src/progressdialog.cpp
)

set(HEADERS
    include/mainwindow.h
    include/compressor.h
    include/progressdialog.h
)

# Create executable
add_executable(FileCompressor ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(FileCompressor
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    ${LIBPNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

# Set compiler flags
target_compile_options(FileCompressor PRIVATE
    ${LIBPNG_CFLAGS_OTHER}
    ${JPEG_CFLAGS_OTHER}
    ${ZLIB_CFLAGS_OTHER}
)

# Set linker flags
target_link_options(FileCompressor PRIVATE
    ${LIBPNG_LDFLAGS}
    ${JPEG_LDFLAGS}
    ${ZLIB_LDFLAGS}
)

# Copy Qt plugins to build directory (for deployment)
if(WIN32)
    add_custom_command(TARGET FileCompressor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>
        $<TARGET_FILE_DIR:FileCompressor>
    )
endif()

# Installation
install(TARGETS FileCompressor
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
