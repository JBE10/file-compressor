#include "gui_compressor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <zlib.h>
#include <iomanip>
#include <algorithm>

namespace fs = std::filesystem;

CompressionResult PureCppCompressor::compressFile(const std::string &inputPath, const std::string &outputPath)
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

CompressionResult PureCppCompressor::compressTextFile(const std::string &inputPath, const std::string &outputPath)
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

CompressionResult PureCppCompressor::compressBinaryFile(const std::string &inputPath, const std::string &outputPath)
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
