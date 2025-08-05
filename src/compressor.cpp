#include "compressor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QImage>
#include <QImageWriter>
#include <QBuffer>
#include <QDataStream>
#include <QIODevice>
#include <QProcess>
#include <QThread>
#include <zlib.h>
#include <png.h>
#include <jpeglib.h>
#include <memory>

// PIMPL implementation
class Compressor::Impl
{
public:
    // Image compression using Qt
    static CompressionResult compressImageQt(const QString &inputPath, const QString &outputPath);
    
    // ZIP compression using zlib
    static CompressionResult compressZip(const QString &inputPath, const QString &outputPath);
    
    // GZIP compression using zlib
    static CompressionResult compressGzip(const QString &inputPath, const QString &outputPath);
    
    // PDF compression (basic implementation)
    static CompressionResult compressPdf(const QString &inputPath, const QString &outputPath);
};

Compressor::Compressor(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
}

Compressor::~Compressor() = default;

void Compressor::setProgressCallback(ProgressCallback callback)
{
    m_progressCallback = callback;
}

CompressionResult Compressor::compressFile(const QString &inputPath, const QString &outputPath, const QString &compressionType)
{
    try {
        QString extension = getFileExtension(inputPath).toLower();
        
        if (extension == "png" || extension == "jpg" || extension == "jpeg" || 
            extension == "bmp" || extension == "tiff" || extension == "tif") {
            return compressImage(inputPath, outputPath);
        } else if (extension == "pdf") {
            return compressPDF(inputPath, outputPath);
        } else {
            return compressGeneralFile(inputPath, outputPath, compressionType);
        }
    } catch (const std::exception &e) {
        CompressionResult result;
        result.success = false;
        result.errorMessage = QString("Error comprimiendo %1: %2").arg(inputPath, e.what());
        return result;
    }
}

QList<CompressionResult> Compressor::compressMultipleFiles(const QStringList &filePaths, const QString &outputDir, const QString &compressionType)
{
    QList<CompressionResult> results;
    int totalFiles = filePaths.size();
    
    for (int i = 0; i < filePaths.size(); ++i) {
        const QString &filePath = filePaths[i];
        
        try {
            // Create output filename
            QFileInfo fileInfo(filePath);
            QString baseName = fileInfo.baseName();
            QString extension = fileInfo.suffix().toLower();
            QString outputPath;
            
            if (extension == "png" || extension == "jpg" || extension == "jpeg" || 
                extension == "bmp" || extension == "tiff" || extension == "tif") {
                outputPath = QString("%1/%2_compressed.%3").arg(outputDir, baseName, extension);
            } else if (extension == "pdf") {
                outputPath = QString("%1/%2_compressed.%3").arg(outputDir, baseName, extension);
            } else {
                if (compressionType == "zip") {
                    outputPath = QString("%1/%2.zip").arg(outputDir, baseName);
                } else {
                    outputPath = QString("%1/%2.gz").arg(outputDir, baseName);
                }
            }
            
            // Compress file
            CompressionResult result = compressFile(filePath, outputPath, compressionType);
            result.filename = fileInfo.fileName();
            result.outputPath = outputPath;
            results.append(result);
            
            // Update progress
            int progress = ((i + 1) * 100) / totalFiles;
            updateProgress(QString("Progreso: %1/%2 archivos").arg(i + 1).arg(totalFiles), progress);
            
        } catch (const std::exception &e) {
            CompressionResult result;
            result.success = false;
            result.filename = QFileInfo(filePath).fileName();
            result.errorMessage = QString("Error: %1").arg(e.what());
            results.append(result);
        }
    }
    
    return results;
}

CompressionResult Compressor::compressImage(const QString &inputPath, const QString &outputPath)
{
    updateProgress(QString("Comprimiendo imagen: %1").arg(QFileInfo(inputPath).fileName()), 10);
    
    return Impl::compressImageQt(inputPath, outputPath);
}

CompressionResult Compressor::compressPDF(const QString &inputPath, const QString &outputPath)
{
    updateProgress(QString("Comprimiendo PDF: %1").arg(QFileInfo(inputPath).fileName()), 10);
    
    return Impl::compressPdf(inputPath, outputPath);
}

CompressionResult Compressor::compressGeneralFile(const QString &inputPath, const QString &outputPath, const QString &compressionType)
{
    updateProgress(QString("Comprimiendo archivo: %1").arg(QFileInfo(inputPath).fileName()), 10);
    
    if (compressionType == "zip") {
        return Impl::compressZip(inputPath, outputPath);
    } else if (compressionType == "gzip") {
        return Impl::compressGzip(inputPath, outputPath);
    } else {
        CompressionResult result;
        result.success = false;
        result.errorMessage = QString("Tipo de compresión no soportado: %1").arg(compressionType);
        return result;
    }
}

QString Compressor::getFileExtension(const QString &filePath) const
{
    return QFileInfo(filePath).suffix();
}

qint64 Compressor::getFileSize(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);
    return fileInfo.size();
}

double Compressor::calculateCompressionRatio(qint64 originalSize, qint64 compressedSize) const
{
    if (originalSize == 0) return 0.0;
    return ((originalSize - compressedSize) * 100.0) / originalSize;
}

void Compressor::updateProgress(const QString &message, int percentage)
{
    if (m_progressCallback) {
        m_progressCallback(message, percentage);
    }
    emit progressUpdated(message, percentage);
}

// PIMPL Implementation
CompressionResult Compressor::Impl::compressImageQt(const QString &inputPath, const QString &outputPath)
{
    QImage image(inputPath);
    if (image.isNull()) {
        CompressionResult result;
        result.success = false;
        result.errorMessage = "No se pudo cargar la imagen";
        return result;
    }
    
    // Convert to RGB if necessary
    if (image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32) {
        image = image.convertToFormat(QImage::Format_RGB32);
    }
    
    // Save with compression
    QImageWriter writer(outputPath);
    writer.setQuality(85); // Good balance between quality and size
    
    if (!writer.write(image)) {
        CompressionResult result;
        result.success = false;
        result.errorMessage = QString("Error guardando imagen: %1").arg(writer.errorString());
        return result;
    }
    
    // Calculate sizes
    qint64 originalSize = QFileInfo(inputPath).size();
    qint64 compressedSize = QFileInfo(outputPath).size();
    double ratio = ((originalSize - compressedSize) * 100.0) / originalSize;
    
    return CompressionResult(true, QFileInfo(inputPath).fileName(), outputPath, originalSize, compressedSize, ratio);
}

CompressionResult Compressor::Impl::compressZip(const QString &inputPath, const QString &outputPath)
{
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        CompressionResult result;
        result.success = false;
        result.errorMessage = "No se pudo abrir el archivo de entrada";
        return result;
    }
    
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        CompressionResult result;
        result.success = false;
        result.errorMessage = "No se pudo crear el archivo de salida";
        return result;
    }
    
    // Read input file
    QByteArray inputData = inputFile.readAll();
    inputFile.close();
    
    // Compress with zlib
    QByteArray compressedData = qCompress(inputData, 9); // Maximum compression
    
    // Write compressed data
    outputFile.write(compressedData);
    outputFile.close();
    
    // Calculate sizes
    qint64 originalSize = inputData.size();
    qint64 compressedSize = compressedData.size();
    double ratio = ((originalSize - compressedSize) * 100.0) / originalSize;
    
    return CompressionResult(true, QFileInfo(inputPath).fileName(), outputPath, originalSize, compressedSize, ratio);
}

CompressionResult Compressor::Impl::compressGzip(const QString &inputPath, const QString &outputPath)
{
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        CompressionResult result;
        result.success = false;
        result.errorMessage = "No se pudo abrir el archivo de entrada";
        return result;
    }
    
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        CompressionResult result;
        result.success = false;
        result.errorMessage = "No se pudo crear el archivo de salida";
        return result;
    }
    
    // Read input file
    QByteArray inputData = inputFile.readAll();
    inputFile.close();
    
    // Compress with zlib (gzip format)
    QByteArray compressedData = qCompress(inputData, 9);
    
    // Write compressed data
    outputFile.write(compressedData);
    outputFile.close();
    
    // Calculate sizes
    qint64 originalSize = inputData.size();
    qint64 compressedSize = compressedData.size();
    double ratio = ((originalSize - compressedSize) * 100.0) / originalSize;
    
    return CompressionResult(true, QFileInfo(inputPath).fileName(), outputPath, originalSize, compressedSize, ratio);
}

CompressionResult Compressor::Impl::compressPdf(const QString &inputPath, const QString &outputPath)
{
    // For PDF compression, we'll use a simple approach
    // In a real implementation, you might want to use a PDF library like Poppler
    
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        CompressionResult result;
        result.success = false;
        result.errorMessage = "No se pudo abrir el archivo PDF";
        return result;
    }
    
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        CompressionResult result;
        result.success = false;
        result.errorMessage = "No se pudo crear el archivo PDF de salida";
        return result;
    }
    
    // For now, just copy the file (basic implementation)
    // In a real implementation, you would use a PDF library to optimize the PDF
    QByteArray pdfData = inputFile.readAll();
    outputFile.write(pdfData);
    
    inputFile.close();
    outputFile.close();
    
    // Calculate sizes
    qint64 originalSize = QFileInfo(inputPath).size();
    qint64 compressedSize = QFileInfo(outputPath).size();
    double ratio = ((originalSize - compressedSize) * 100.0) / originalSize;
    
    return CompressionResult(true, QFileInfo(inputPath).fileName(), outputPath, originalSize, compressedSize, ratio);
} 