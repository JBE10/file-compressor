# GUI File Compressor

A modern Qt-based GUI application for compressing files with support for multiple compression algorithms and file types.

## Features

- **Modern GUI Interface**: Clean, intuitive interface with dark theme
- **Multiple File Types**: Supports text files, images, PDFs, and binary files
- **Drag & Drop**: Easy file selection with drag and drop support
- **Compression Options**: Configurable compression levels and quality settings
- **Progress Tracking**: Real-time progress updates during compression
- **Results Table**: Detailed results showing compression ratios and file sizes
- **Cross-platform**: Works on macOS, Linux, and Windows

## Prerequisites

### macOS
```bash
# Install Qt5 and required libraries
brew install qt@5
brew install zlib
brew install libzip
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install qt5-default libqt5widgets5-dev
sudo apt-get install libz-dev libzip-dev
```

### Windows
- Install Qt5 from the official website
- Install MSYS2 and required packages

## Building the Application

### Option 1: CMake Build (Recommended)

```bash
# Make the build script executable
chmod +x build_cmake.sh

# Build the application
./build_cmake.sh
```

### Option 2: qmake Build

```bash
# Make the build script executable
chmod +x build_gui.sh

# Build the application
./build_gui.sh
```

## Running the Application

### Quick Start
```bash
# Make the run script executable
chmod +x run_gui.sh

# Run the application
./run_gui.sh
```

### Manual Run
```bash
# If built with CMake
./build_cmake/gui_compressor.app/Contents/MacOS/gui_compressor

# If built with qmake
./build_gui/gui_compressor
```

## Usage

1. **Add Files**: Click "Agregar Archivos" or drag and drop files into the application
2. **Select Output Directory**: Click "Seleccionar Directorio" to choose where compressed files will be saved
3. **Configure Options**:
   - **Compression Type**: Choose between Automatic, ZIP, GZIP, or Optimized
   - **Compression Level**: Adjust from 1 (fastest) to 9 (best compression)
   - **Image Quality**: For image files, set quality from 10% to 100%
   - **Options**: Preserve directory structure, overwrite existing files
4. **Start Compression**: Click "Iniciar Compresión" to begin
5. **Monitor Progress**: Watch the progress bar and status updates
6. **View Results**: Check the results table for compression statistics

## File Type Support

- **Text Files**: `.txt`, `.log`, `.csv`, `.json`, `.xml`, `.html`, `.css`, `.js`, `.cpp`, `.h`, `.py`, `.java`
- **Images**: `.jpg`, `.jpeg`, `.png`, `.gif`, `.bmp`, `.tiff`
- **Documents**: `.pdf`
- **Other**: All other file types are compressed using ZIP format

## Compression Algorithms

- **ZIP**: Standard ZIP compression using zlib
- **GZIP**: Gzip compression for single files
- **Optimized**: Specialized compression for specific file types
- **Automatic**: Chooses the best algorithm based on file type

## Project Structure

```
├── src/
│   ├── gui_main.cpp          # Main application entry point
│   ├── gui_mainwindow.cpp    # Main window implementation
│   └── gui_compressor.cpp    # Compression engine
├── include/
│   ├── gui_mainwindow.h      # Main window header
│   └── gui_compressor.h      # Compression engine header
├── build_cmake/              # CMake build output
├── build_gui/                # qmake build output
├── CMakeLists.txt            # CMake configuration
├── gui_compressor.pro        # qmake project file
├── build_cmake.sh            # CMake build script
├── build_gui.sh              # qmake build script
└── run_gui.sh                # Application launcher
```

## Troubleshooting

### Build Issues

1. **Qt5 not found**: Make sure Qt5 is installed and the path is correct
2. **MOC errors**: Ensure `CMAKE_AUTOMOC ON` is set in CMakeLists.txt
3. **Library not found**: Install missing dependencies (zlib, libzip)

### Runtime Issues

1. **Application won't start**: Check if all Qt libraries are available
2. **Compression fails**: Verify input files exist and output directory is writable
3. **Permission errors**: Ensure the application has read/write permissions

## Development

### Adding New File Types

1. Add the file extension to the `compressFile` method in `gui_compressor.cpp`
2. Implement specialized compression if needed
3. Update the UI to show the new file type

### Modifying the UI

1. Edit `gui_mainwindow.cpp` to modify the interface
2. Update `gui_mainwindow.h` for new UI components
3. Rebuild the application

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Author

Juan Bautista Espino

## Version

2.0.0
