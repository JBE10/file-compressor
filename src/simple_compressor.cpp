#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <zlib.h>
#include <zip.h>
#include <cstring>

namespace fs = std::filesystem;

struct CompressionResult {
    bool success = false;
    std::string filename;
    std::string outputPath;
    size_t originalSize = 0;
    size_t compressedSize = 0;
    double compressionRatio = 0.0;
    std::string errorMessage;
};

class SimpleCompressor {
public:
    static CompressionResult compressFile(const std::string &inputPath, const std::string &outputPath) {
        CompressionResult result;
        
        try {
            fs::path inputFile(inputPath);
            std::string extension = inputFile.extension().string();
            
            // Convert to lowercase
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            if (extension == ".pdf") {
                return compressPDF(inputPath, outputPath);
            } else {
                return compressToZip(inputPath, outputPath);
            }
        } catch (const std::exception &e) {
            result.success = false;
            result.errorMessage = std::string("Error: ") + e.what();
            return result;
        }
    }

private:
    static CompressionResult compressPDF(const std::string &inputPath, const std::string &outputPath) {
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
    
    static CompressionResult compressToZip(const std::string &inputPath, const std::string &outputPath) {
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
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Uso: " << argv[0] << " <archivo_entrada> <archivo_salida>" << std::endl;
        std::cout << "Ejemplo: " << argv[0] << " documento.pdf comprimido" << std::endl;
        return 1;
    }
    
    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    
    std::cout << "Comprimiendo: " << inputPath << std::endl;
    
    CompressionResult result = SimpleCompressor::compressFile(inputPath, outputPath);
    
    if (result.success) {
        std::cout << "✅ Compresión exitosa!" << std::endl;
        std::cout << "📁 Archivo original: " << inputPath << " (" << result.originalSize << " bytes)" << std::endl;
        std::cout << "📦 Archivo comprimido: " << result.outputPath << " (" << result.compressedSize << " bytes)" << std::endl;
        std::cout << "📊 Ratio de compresión: " << std::fixed << std::setprecision(1) << result.compressionRatio << "%" << std::endl;
    } else {
        std::cout << "❌ Error en la compresión: " << result.errorMessage << std::endl;
        return 1;
    }
    
    return 0;
}
