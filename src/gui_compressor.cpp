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
#include <zip.h>
#include <cstring>

namespace fs = std::filesystem;

CompressionResult PureCppCompressor::compressFile(const std::string &inputPath, const std::string &outputPath)
{
    CompressionResult result;

    try {
        fs::path inputFile(inputPath);
        std::string extension = inputFile.extension().string();

        // Convert to lowercase
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        if (extension == ".pdf") {
            return compressPDF(inputPath, outputPath);
        } else if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" ||
                   extension == ".gif" || extension == ".bmp" || extension == ".tiff") {
            return compressImage(inputPath, outputPath);
        } else if (extension == ".txt" || extension == ".log" || extension == ".csv" ||
                   extension == ".json" || extension == ".xml" || extension == ".html" ||
                   extension == ".css" || extension == ".js" || extension == ".cpp" ||
                   extension == ".h" || extension == ".py" || extension == ".java") {
            return compressTextFile(inputPath, outputPath);
        } else {
            return compressToZip(inputPath, outputPath);
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

        // Create ZIP file
        std::string zipPath = outputPath;
        if (zipPath.find(".zip") == std::string::npos) {
            zipPath = zipPath.substr(0, zipPath.find_last_of('.')) + ".zip";
        }

        int err = 0;
        zip_t *zip = zip_open(zipPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
        if (!zip) {
            result.success = false;
            result.errorMessage = "No se pudo crear el archivo ZIP";
            return result;
        }

        // Add compressed data to ZIP
        fs::path inputFileName = fs::path(inputPath).filename();
        zip_source_t *source = zip_source_buffer(zip, compressed.data(), compressedSize, 0);
        if (zip_file_add(zip, inputFileName.string().c_str(), source, ZIP_FL_OVERWRITE) < 0) {
            zip_source_free(source);
            zip_close(zip);
            result.success = false;
            result.errorMessage = "Error al agregar archivo al ZIP";
            return result;
        }

        zip_close(zip);

        result.success = true;
        result.originalSize = content.size();
        result.compressedSize = fs::file_size(zipPath);
        result.compressionRatio = ((double)(result.originalSize - result.compressedSize) / result.originalSize) * 100.0;
        result.outputPath = zipPath;

    } catch (const std::exception &e) {
        result.success = false;
        result.errorMessage = std::string("Error: ") + e.what();
    }

    return result;
}

CompressionResult PureCppCompressor::compressBinaryFile(const std::string &inputPath, const std::string &outputPath)
{
    return compressToZip(inputPath, outputPath);
}

CompressionResult PureCppCompressor::compressPDF(const std::string &inputPath, const std::string &outputPath)
{
    CompressionResult result;

    try {
        // For PDF optimization, we'll create a compressed ZIP containing the PDF
        // In a real implementation, you would use a PDF library like Poppler or MuPDF
        // to actually optimize the PDF content

        std::string zipPath = outputPath;
        if (zipPath.find(".zip") == std::string::npos) {
            zipPath = zipPath.substr(0, zipPath.find_last_of('.')) + "_optimized.zip";
        }

        int err = 0;
        zip_t *zip = zip_open(zipPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
        if (!zip) {
            result.success = false;
            result.errorMessage = "No se pudo crear el archivo ZIP para PDF";
            return result;
        }

        // Read PDF file
        std::ifstream pdfFile(inputPath, std::ios::binary);
        if (!pdfFile.is_open()) {
            zip_close(zip);
            result.success = false;
            result.errorMessage = "No se pudo abrir el archivo PDF";
            return result;
        }

        std::vector<unsigned char> pdfContent((std::istreambuf_iterator<char>(pdfFile)),
                                             std::istreambuf_iterator<char>());
        pdfFile.close();

        // Compress PDF content
        std::vector<unsigned char> compressed;
        uLong compressedSize = compressBound(pdfContent.size());
        compressed.resize(compressedSize);

        if (compress2(compressed.data(), &compressedSize,
                     pdfContent.data(), pdfContent.size(), Z_BEST_COMPRESSION) != Z_OK) {
            zip_close(zip);
            result.success = false;
            result.errorMessage = "Error en la compresión del PDF";
            return result;
        }

        // Add compressed PDF to ZIP
        fs::path inputFileName = fs::path(inputPath).filename();
        std::string optimizedName = inputFileName.stem().string() + "_optimized.pdf";

        zip_source_t *source = zip_source_buffer(zip, compressed.data(), compressedSize, 0);
        if (zip_file_add(zip, optimizedName.c_str(), source, ZIP_FL_OVERWRITE) < 0) {
            zip_source_free(source);
            zip_close(zip);
            result.success = false;
            result.errorMessage = "Error al agregar PDF optimizado al ZIP";
            return result;
        }

        zip_close(zip);

        result.success = true;
        result.originalSize = pdfContent.size();
        result.compressedSize = fs::file_size(zipPath);
        result.compressionRatio = ((double)(result.originalSize - result.compressedSize) / result.originalSize) * 100.0;
        result.outputPath = zipPath;

    } catch (const std::exception &e) {
        result.success = false;
        result.errorMessage = std::string("Error: ") + e.what();
    }

    return result;
}

CompressionResult PureCppCompressor::compressToZip(const std::string &inputPath, const std::string &outputPath)
{
    CompressionResult result;

    try {
        std::string zipPath = outputPath;
        if (zipPath.find(".zip") == std::string::npos) {
            zipPath = zipPath.substr(0, zipPath.find_last_of('.')) + ".zip";
        }

        int err = 0;
        zip_t *zip = zip_open(zipPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
        if (!zip) {
            result.success = false;
            result.errorMessage = "No se pudo crear el archivo ZIP";
            return result;
        }

        // Read file content
        std::ifstream inputFile(inputPath, std::ios::binary);
        if (!inputFile.is_open()) {
            zip_close(zip);
            result.success = false;
            result.errorMessage = "No se pudo abrir el archivo de entrada";
            return result;
        }

        std::vector<unsigned char> content((std::istreambuf_iterator<char>(inputFile)),
                                          std::istreambuf_iterator<char>());
        inputFile.close();

        // Compress content
        std::vector<unsigned char> compressed;
        uLong compressedSize = compressBound(content.size());
        compressed.resize(compressedSize);

        if (compress2(compressed.data(), &compressedSize,
                     content.data(), content.size(), Z_BEST_COMPRESSION) != Z_OK) {
            zip_close(zip);
            result.success = false;
            result.errorMessage = "Error en la compresión";
            return result;
        }

        // Add compressed file to ZIP
        fs::path inputFileName = fs::path(inputPath).filename();
        zip_source_t *source = zip_source_buffer(zip, compressed.data(), compressedSize, 0);
        if (zip_file_add(zip, inputFileName.string().c_str(), source, ZIP_FL_OVERWRITE) < 0) {
            zip_source_free(source);
            zip_close(zip);
            result.success = false;
            result.errorMessage = "Error al agregar archivo al ZIP";
            return result;
        }

        zip_close(zip);

        result.success = true;
        result.originalSize = content.size();
        result.compressedSize = fs::file_size(zipPath);
        result.compressionRatio = ((double)(result.originalSize - result.compressedSize) / result.originalSize) * 100.0;
        result.outputPath = zipPath;

    } catch (const std::exception &e) {
        result.success = false;
        result.errorMessage = std::string("Error: ") + e.what();
    }

    return result;
}

CompressionResult PureCppCompressor::compressImage(const std::string &inputPath, const std::string &outputPath)
{
    CompressionResult result;

    try {
        std::string zipPath = outputPath;
        if (zipPath.find(".zip") == std::string::npos) {
            zipPath = zipPath.substr(0, zipPath.find_last_of('.')) + "_optimized.zip";
        }

        int err = 0;
        zip_t *zip = zip_open(zipPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
        if (!zip) {
            result.success = false;
            result.errorMessage = "No se pudo crear el archivo ZIP para imagen";
            return result;
        }

        // Read image file
        std::ifstream imageFile(inputPath, std::ios::binary);
        if (!imageFile.is_open()) {
            zip_close(zip);
            result.success = false;
            result.errorMessage = "No se pudo abrir el archivo de imagen";
            return result;
        }

        std::vector<unsigned char> imageContent((std::istreambuf_iterator<char>(imageFile)),
                                               std::istreambuf_iterator<char>());
        imageFile.close();

        // Compress image content
        std::vector<unsigned char> compressed;
        uLong compressedSize = compressBound(imageContent.size());
        compressed.resize(compressedSize);

        if (compress2(compressed.data(), &compressedSize,
                     imageContent.data(), imageContent.size(), Z_BEST_COMPRESSION) != Z_OK) {
            zip_close(zip);
            result.success = false;
            result.errorMessage = "Error en la compresión de la imagen";
            return result;
        }

        // Add compressed image to ZIP
        fs::path inputFileName = fs::path(inputPath).filename();
        std::string optimizedName = inputFileName.stem().string() + "_optimized" + inputFileName.extension().string();

        zip_source_t *source = zip_source_buffer(zip, compressed.data(), compressedSize, 0);
        if (zip_file_add(zip, optimizedName.c_str(), source, ZIP_FL_OVERWRITE) < 0) {
            zip_source_free(source);
            zip_close(zip);
            result.success = false;
            result.errorMessage = "Error al agregar imagen optimizada al ZIP";
            return result;
        }

        zip_close(zip);

        result.success = true;
        result.originalSize = imageContent.size();
        result.compressedSize = fs::file_size(zipPath);
        result.compressionRatio = ((double)(result.originalSize - result.compressedSize) / result.originalSize) * 100.0;
        result.outputPath = zipPath;

    } catch (const std::exception &e) {
        result.success = false;
        result.errorMessage = std::string("Error: ") + e.what();
    }

    return result;
}

int main(int argc, char *argv[])
{
    // This is a placeholder for the main function.
    // In a real application, you would instantiate PureCppCompressor
    // and call its methods based on user input.
    std::cout << "GUI Compressor (Pure C++)" << std::endl;
    std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;

    if (argc < 3) {
        std::cout << "Error: Missing input and/or output file arguments." << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];

    PureCppCompressor compressor;
    CompressionResult result = compressor.compressFile(inputPath, outputPath);

    if (result.success) {
        std::cout << "Compression successful!" << std::endl;
        std::cout << "Original Size: " << result.originalSize << " bytes" << std::endl;
        std::cout << "Compressed Size: " << result.compressedSize << " bytes" << std::endl;
        std::cout << "Compression Ratio: " << std::fixed << std::setprecision(2) << result.compressionRatio << "%" << std::endl;
        std::cout << "Output Path: " << result.outputPath << std::endl;
    } else {
        std::cout << "Compression failed: " << result.errorMessage << std::endl;
    }

    return 0;
}
