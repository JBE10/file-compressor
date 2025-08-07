#ifndef GUI_COMPRESSOR_H
#define GUI_COMPRESSOR_H

#include <string>

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
    static CompressionResult compressFile(const std::string &inputPath, const std::string &outputPath);

private:
    static CompressionResult compressTextFile(const std::string &inputPath, const std::string &outputPath);
    static CompressionResult compressBinaryFile(const std::string &inputPath, const std::string &outputPath);
    static CompressionResult compressPDF(const std::string &inputPath, const std::string &outputPath);
    static CompressionResult compressToZip(const std::string &inputPath, const std::string &outputPath);
};

#endif // GUI_COMPRESSOR_H
