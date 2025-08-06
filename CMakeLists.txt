cmake_minimum_required(VERSION 3.16)
project(FileCompressor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)

# Find required libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZLIB REQUIRED zlib)

# Include directories
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

# Set include directories for the target
target_include_directories(FileCompressor PRIVATE include)

# Link libraries
target_link_libraries(FileCompressor PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    ${ZLIB_LIBRARIES}
)

# Add system frameworks for macOS
if(APPLE)
    target_link_libraries(FileCompressor PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework Carbon"
    )
endif()

# Set compiler flags
target_compile_options(FileCompressor PRIVATE
    ${ZLIB_CFLAGS_OTHER}
)

# Set linker flags
target_link_options(FileCompressor PRIVATE
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
