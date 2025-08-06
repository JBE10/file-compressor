#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <zlib.h>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <chrono>
#include <limits>

namespace fs = std::filesystem;

struct CompressionResult
{
    bool success = false;
    std::string filename;
    std::string outputPath;
    size_t originalSize = 0;
    size_t compressedSize = 0;
    double compressionRatio = 0.0;
    std::string errorMessage;
};

class InteractiveCompressor
{
public:
    void run()
    {
        while (true) {
            clearScreen();
            showMainMenu();

            int choice = getMenuChoice(1, 5);

            switch (choice) {
                case 1:
                    compressSingleFile();
                    break;
                case 2:
                    compressMultipleFiles();
                    break;
                case 3:
                    showCompressionInfo();
                    break;
                case 4:
                    showSettings();
                    break;
                case 5:
                    std::cout << "\n👋 ¡Gracias por usar el Compresor de Archivos!" << std::endl;
                    return;
            }

            if (choice != 5) {
                std::cout << "\n⏸️  Presiona Enter para continuar...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
            }
        }
    }

private:
    void clearScreen()
    {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void showMainMenu()
    {
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                🚀 COMPRESOR DE ARCHIVOS v2.0                ║" << std::endl;
        std::cout << "║                     Versión Interactiva                     ║" << std::endl;
        std::cout << "╠══════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║                                                              ║" << std::endl;
        std::cout << "║  📁 1. Comprimir un archivo                                 ║" << std::endl;
        std::cout << "║  📂 2. Comprimir múltiples archivos                         ║" << std::endl;
        std::cout << "║  ℹ️  3. Información de compresión                           ║" << std::endl;
        std::cout << "║  ⚙️  4. Configuración                                       ║" << std::endl;
        std::cout << "║  🚪 5. Salir                                                ║" << std::endl;
        std::cout << "║                                                              ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << "\n🎯 Selecciona una opción: ";
    }

    int getMenuChoice(int min, int max)
    {
        int choice;
        while (!(std::cin >> choice) || choice < min || choice > max) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "❌ Opción inválida. Por favor selecciona " << min << "-" << max << ": ";
        }
        return choice;
    }

    void compressSingleFile()
    {
        clearScreen();
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    📁 COMPRIMIR ARCHIVO                     ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;

        std::string filePath;
        std::cout << "\n📝 Ingresa la ruta del archivo a comprimir: ";
        std::cin.ignore();
        std::getline(std::cin, filePath);

        if (!fs::exists(filePath)) {
            std::cout << "\n❌ Error: El archivo no existe." << std::endl;
            return;
        }

        // Create output directory
        fs::path outputDir("output");
        if (!fs::exists(outputDir)) {
            fs::create_directories(outputDir);
        }

        fs::path inputPath(filePath);
        std::string outputPath = (outputDir / (inputPath.stem().string() + "_compressed" + inputPath.extension().string())).string();

        std::cout << "\n🔨 Comprimiendo archivo..." << std::endl;
        showProgress();

        CompressionResult result = compressFile(filePath, outputPath);

        clearScreen();
        showCompressionResult(result);
    }

    void compressMultipleFiles()
    {
        clearScreen();
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                   📂 COMPRIMIR MÚLTIPLES                    ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;

        std::vector<std::string> files;
        std::string filePath;

        std::cout << "\n📝 Ingresa las rutas de los archivos (una por línea, Enter vacío para terminar):" << std::endl;
        std::cin.ignore();

        while (true) {
            std::cout << "📁 Archivo " << (files.size() + 1) << ": ";
            std::getline(std::cin, filePath);

            if (filePath.empty()) {
                break;
            }

            if (fs::exists(filePath)) {
                files.push_back(filePath);
            } else {
                std::cout << "⚠️  El archivo no existe, se omitirá." << std::endl;
            }
        }

        if (files.empty()) {
            std::cout << "\n❌ No se seleccionaron archivos válidos." << std::endl;
            return;
        }

        // Create output directory
        fs::path outputDir("output");
        if (!fs::exists(outputDir)) {
            fs::create_directories(outputDir);
        }

        std::cout << "\n🔨 Comprimiendo " << files.size() << " archivos..." << std::endl;

        std::vector<CompressionResult> results;
        for (size_t i = 0; i < files.size(); ++i) {
            fs::path inputPath(files[i]);
            std::string outputPath = (outputDir / (inputPath.stem().string() + "_compressed" + inputPath.extension().string())).string();

            std::cout << "\n📊 Progreso: " << (i + 1) << "/" << files.size() << " - " << inputPath.filename().string() << std::endl;
            showProgress();

            CompressionResult result = compressFile(files[i], outputPath);
            results.push_back(result);
        }

        clearScreen();
        showMultipleCompressionResults(results);
    }

    void showCompressionInfo()
    {
        clearScreen();
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    ℹ️  INFORMACIÓN                           ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;

        std::cout << "\n📋 Tipos de archivo soportados:" << std::endl;
        std::cout << "   • Archivos de texto (.txt, .log, .csv, .json, .xml, .html)" << std::endl;
        std::cout << "   • Archivos de código (.cpp, .h, .py, .java, .js, .css)" << std::endl;
        std::cout << "   • Archivos binarios (cualquier otro tipo)" << std::endl;

        std::cout << "\n🔧 Algoritmo de compresión:" << std::endl;
        std::cout << "   • zlib (DEFLATE) con nivel máximo de compresión" << std::endl;
        std::cout << "   • Optimizado para archivos de texto y código" << std::endl;

        std::cout << "\n📁 Ubicación de archivos comprimidos:" << std::endl;
        std::cout << "   • Carpeta: ./output/" << std::endl;
        std::cout << "   • Formato: nombre_original_compressed.extensión" << std::endl;

        std::cout << "\n⚡ Características:" << std::endl;
        std::cout << "   • Compresión rápida y eficiente" << std::endl;
        std::cout << "   • Estadísticas detalladas" << std::endl;
        std::cout << "   • Interfaz interactiva" << std::endl;
        std::cout << "   • Soporte multiplataforma" << std::endl;
    }

    void showSettings()
    {
        clearScreen();
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    ⚙️  CONFIGURACIÓN                         ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;

        std::cout << "\n🔧 Configuración actual:" << std::endl;
        std::cout << "   • Nivel de compresión: Máximo (Z_BEST_COMPRESSION)" << std::endl;
        std::cout << "   • Carpeta de salida: ./output/" << std::endl;
        std::cout << "   • Algoritmo: zlib DEFLATE" << std::endl;

        std::cout << "\n📊 Estadísticas de uso:" << std::endl;
        std::cout << "   • Archivos comprimidos: " << getCompressedFilesCount() << std::endl;
        std::cout << "   • Espacio ahorrado: " << getTotalSpaceSaved() << " bytes" << std::endl;
    }

    void showProgress()
    {
        std::cout << "🔄 ";
        for (int i = 0; i < 20; ++i) {
            std::cout << "█";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        std::cout << " ✅" << std::endl;
    }

    void showCompressionResult(const CompressionResult &result)
    {
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    📊 RESULTADO                              ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;

        if (result.success) {
            std::cout << "\n✅ Compresión exitosa!" << std::endl;
            std::cout << "📁 Archivo: " << result.filename << std::endl;
            std::cout << "📊 Tamaño original: " << formatFileSize(result.originalSize) << std::endl;
            std::cout << "📊 Tamaño comprimido: " << formatFileSize(result.compressedSize) << std::endl;
            std::cout << "📈 Ratio de compresión: " << std::fixed << std::setprecision(2) << result.compressionRatio << "%" << std::endl;
            std::cout << "💾 Archivo guardado en: " << result.outputPath << std::endl;
        } else {
            std::cout << "\n❌ Error en la compresión: " << result.errorMessage << std::endl;
        }
    }

    void showMultipleCompressionResults(const std::vector<CompressionResult> &results)
    {
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                  📊 RESULTADOS MÚLTIPLES                    ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;

        int successful = 0;
        size_t totalOriginal = 0;
        size_t totalCompressed = 0;

        for (const auto &result : results) {
            if (result.success) {
                successful++;
                totalOriginal += result.originalSize;
                totalCompressed += result.compressedSize;
            }
        }

        std::cout << "\n📊 Resumen:" << std::endl;
        std::cout << "   • Archivos procesados: " << results.size() << std::endl;
        std::cout << "   • Comprimidos exitosamente: " << successful << std::endl;
        std::cout << "   • Errores: " << (results.size() - successful) << std::endl;

        if (successful > 0) {
            double totalRatio = ((double)(totalOriginal - totalCompressed) / totalOriginal) * 100.0;
            std::cout << "   • Tamaño total original: " << formatFileSize(totalOriginal) << std::endl;
            std::cout << "   • Tamaño total comprimido: " << formatFileSize(totalCompressed) << std::endl;
            std::cout << "   • Ratio promedio: " << std::fixed << std::setprecision(2) << totalRatio << "%" << std::endl;
        }

        std::cout << "\n📋 Detalles por archivo:" << std::endl;
        for (const auto &result : results) {
            std::cout << "   " << (result.success ? "✅" : "❌") << " " << result.filename;
            if (result.success) {
                std::cout << " (" << std::fixed << std::setprecision(2) << result.compressionRatio << "%)";
            }
            std::cout << std::endl;
        }
    }

    CompressionResult compressFile(const std::string &inputPath, const std::string &outputPath)
    {
        CompressionResult result;
        result.filename = fs::path(inputPath).filename().string();

        try {
            fs::path inputFile(inputPath);
            std::string extension = inputFile.extension().string();

            // Convert to lowercase
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            if (extension == ".txt" || extension == ".log" || extension == ".csv" ||
                extension == ".json" || extension == ".xml" || extension == ".html" ||
                extension == ".css" || extension == ".js" || extension == ".cpp" ||
                extension == ".h" || extension == ".py" || extension == ".java") {
                return compressTextFile(inputPath, outputPath);
            } else {
                return compressBinaryFile(inputPath, outputPath);
            }
        } catch (const std::exception &e) {
            result.success = false;
            result.errorMessage = std::string("Error: ") + e.what();
            return result;
        }
    }

    CompressionResult compressTextFile(const std::string &inputPath, const std::string &outputPath)
    {
        CompressionResult result;
        result.filename = fs::path(inputPath).filename().string();

        try {
            std::ifstream inputFile(inputPath, std::ios::binary);
            if (!inputFile.is_open()) {
                result.success = false;
                result.errorMessage = "No se pudo abrir el archivo de entrada";
                return result;
            }

            // Read file content
            std::string content((std::istreambuf_iterator<char>(inputFile)),
                               std::istreambuf_iterator<char>());
            inputFile.close();

            // Compress using zlib
            std::vector<unsigned char> compressed;
            uLong compressedSize = compressBound(content.size());
            compressed.resize(compressedSize);

            if (compress2(compressed.data(), &compressedSize,
                         reinterpret_cast<const unsigned char*>(content.data()),
                         content.size(), Z_BEST_COMPRESSION) != Z_OK) {
                result.success = false;
                result.errorMessage = "Error en la compresión zlib";
                return result;
            }

            // Write compressed data
            std::ofstream outputFile(outputPath, std::ios::binary);
            if (!outputFile.is_open()) {
                result.success = false;
                result.errorMessage = "No se pudo crear el archivo de salida";
                return result;
            }

            outputFile.write(reinterpret_cast<const char*>(compressed.data()), compressedSize);
            outputFile.close();

            result.success = true;
            result.originalSize = content.size();
            result.compressedSize = compressedSize;
            result.compressionRatio = ((double)(result.originalSize - result.compressedSize) / result.originalSize) * 100.0;
            result.outputPath = outputPath;

        } catch (const std::exception &e) {
            result.success = false;
            result.errorMessage = std::string("Error: ") + e.what();
        }

        return result;
    }

    CompressionResult compressBinaryFile(const std::string &inputPath, const std::string &outputPath)
    {
        CompressionResult result;
        result.filename = fs::path(inputPath).filename().string();

        try {
            std::ifstream inputFile(inputPath, std::ios::binary);
            if (!inputFile.is_open()) {
                result.success = false;
                result.errorMessage = "No se pudo abrir el archivo de entrada";
                return result;
            }

            // Read file content
            std::vector<unsigned char> content((std::istreambuf_iterator<char>(inputFile)),
                                              std::istreambuf_iterator<char>());
            inputFile.close();

            // Compress using zlib
            std::vector<unsigned char> compressed;
            uLong compressedSize = compressBound(content.size());
            compressed.resize(compressedSize);

            if (compress2(compressed.data(), &compressedSize,
                         content.data(), content.size(), Z_BEST_COMPRESSION) != Z_OK) {
                result.success = false;
                result.errorMessage = "Error en la compresión zlib";
                return result;
            }

            // Write compressed data
            std::ofstream outputFile(outputPath, std::ios::binary);
            if (!outputFile.is_open()) {
                result.success = false;
                result.errorMessage = "No se pudo crear el archivo de salida";
                return result;
            }

            outputFile.write(reinterpret_cast<const char*>(compressed.data()), compressedSize);
            outputFile.close();

            result.success = true;
            result.originalSize = content.size();
            result.compressedSize = compressedSize;
            result.compressionRatio = ((double)(result.originalSize - result.compressedSize) / result.originalSize) * 100.0;
            result.outputPath = outputPath;

        } catch (const std::exception &e) {
            result.success = false;
            result.errorMessage = std::string("Error: ") + e.what();
        }

        return result;
    }

    std::string formatFileSize(size_t bytes)
    {
        const std::vector<std::string> units = {"B", "KB", "MB", "GB"};
        double size = bytes;
        int unitIndex = 0;

        while (size >= 1024.0 && unitIndex < units.size() - 1) {
            size /= 1024.0;
            unitIndex++;
        }

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
        return oss.str();
    }

    int getCompressedFilesCount()
    {
        fs::path outputDir("output");
        if (!fs::exists(outputDir)) {
            return 0;
        }

        int count = 0;
        for (const auto &entry : fs::directory_iterator(outputDir)) {
            if (entry.is_regular_file()) {
                count++;
            }
        }
        return count;
    }

    size_t getTotalSpaceSaved()
    {
        // This is a simplified implementation
        // In a real application, you would track this in a database or file
        return 0;
    }
};

int main()
{
    InteractiveCompressor compressor;
    compressor.run();
    return 0;
}
