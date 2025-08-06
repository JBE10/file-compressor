#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <zlib.h>
#include <iomanip>

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

class PureCppCompressor
{
public:
    static CompressionResult compressFile(const std::string &inputPath, const std::string &outputPath)
    {
        CompressionResult result;

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

private:
    static CompressionResult compressTextFile(const std::string &inputPath, const std::string &outputPath)
    {
        CompressionResult result;

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

    static CompressionResult compressBinaryFile(const std::string &inputPath, const std::string &outputPath)
    {
        CompressionResult result;

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
};

void printUsage(const char* programName)
{
    std::cout << "🚀 Compresor de Archivos - Versión C++ Pura" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Uso: " << programName << " <archivo_a_comprimir>" << std::endl;
    std::cout << std::endl;
    std::cout << "Ejemplos:" << std::endl;
    std::cout << "  " << programName << " test.txt" << std::endl;
    std::cout << "  " << programName << " document.pdf" << std::endl;
    std::cout << "  " << programName << " image.jpg" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputFile = argv[1];
    fs::path inputPath(inputFile);

    if (!fs::exists(inputPath)) {
        std::cout << "❌ Error: El archivo no existe: " << inputFile << std::endl;
        return 1;
    }

    // Create output directory
    fs::path outputDir("output");
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    std::string outputFile = (outputDir / (inputPath.stem().string() + "_compressed" + inputPath.extension().string())).string();

    std::cout << "📁 Archivo de entrada: " << inputFile << std::endl;
    std::cout << "📁 Archivo de salida: " << outputFile << std::endl;
    std::cout << "🔨 Comprimiendo..." << std::endl;

    CompressionResult result = PureCppCompressor::compressFile(inputFile, outputFile);

    if (result.success) {
        std::cout << "✅ Compresión exitosa!" << std::endl;
        std::cout << "📊 Tamaño original: " << result.originalSize << " bytes" << std::endl;
        std::cout << "📊 Tamaño comprimido: " << result.compressedSize << " bytes" << std::endl;
        std::cout << "📈 Ratio de compresión: " << std::fixed << std::setprecision(2) << result.compressionRatio << "%" << std::endl;
        std::cout << "📁 Archivo guardado en: " << result.outputPath << std::endl;
    } else {
        std::cout << "❌ Error en la compresión: " << result.errorMessage << std::endl;
        return 1;
    }

    return 0;
}
