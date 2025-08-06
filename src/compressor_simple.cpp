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
            updateProgress(QString("Comprimiendo %1 (%2/%3)").arg(fileInfo.fileName()).arg(i + 1).arg(totalFiles), progress);

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

QString Compressor::getFileExtension(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);
    return fileInfo.suffix();
}

double Compressor::calculateCompressionRatio(qint64 originalSize, qint64 compressedSize) const
{
    if (originalSize == 0) return 0.0;
    return ((double)(originalSize - compressedSize) / originalSize) * 100.0;
}

void Compressor::updateProgress(const QString &message, int percentage)
{
    if (m_progressCallback) {
        m_progressCallback(message, percentage);
    }
    emit progressUpdated(message, percentage);
}

CompressionResult Compressor::compressImage(const QString &inputPath, const QString &outputPath)
{
    CompressionResult result;

    try {
        QImage image(inputPath);
        if (image.isNull()) {
            result.success = false;
            result.errorMessage = "No se pudo cargar la imagen";
            return result;
        }

        QImageWriter writer(outputPath);
        writer.setQuality(85); // Compress with 85% quality

        if (writer.write(image)) {
            QFileInfo inputInfo(inputPath);
            QFileInfo outputInfo(outputPath);

            result.success = true;
            result.originalSize = inputInfo.size();
            result.compressedSize = outputInfo.size();
            result.compressionRatio = calculateCompressionRatio(result.originalSize, result.compressedSize);
            result.outputPath = outputPath;
        } else {
            result.success = false;
            result.errorMessage = "Error al escribir la imagen comprimida";
        }
    } catch (const std::exception &e) {
        result.success = false;
        result.errorMessage = QString("Error: %1").arg(e.what());
    }

    return result;
}

CompressionResult Compressor::compressGeneralFile(const QString &inputPath, const QString &outputPath, const QString &compressionType)
{
    if (compressionType == "zip") {
        return compressZip(inputPath, outputPath);
    } else {
        return compressGzip(inputPath, outputPath);
    }
}

CompressionResult Compressor::compressZip(const QString &inputPath, const QString &outputPath)
{
    CompressionResult result;

    try {
        QFile inputFile(inputPath);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            result.success = false;
            result.errorMessage = "No se pudo abrir el archivo de entrada";
            return result;
        }

        QFile outputFile(outputPath);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            result.success = false;
            result.errorMessage = "No se pudo crear el archivo de salida";
            return result;
        }

        QByteArray inputData = inputFile.readAll();
        QByteArray compressedData;

        // Simple zlib compression
        compressedData = qCompress(inputData, 9);

        outputFile.write(compressedData);

        result.success = true;
        result.originalSize = inputData.size();
        result.compressedSize = compressedData.size();
        result.compressionRatio = calculateCompressionRatio(result.originalSize, result.compressedSize);
        result.outputPath = outputPath;

    } catch (const std::exception &e) {
        result.success = false;
        result.errorMessage = QString("Error: %1").arg(e.what());
    }

    return result;
}

CompressionResult Compressor::compressGzip(const QString &inputPath, const QString &outputPath)
{
    CompressionResult result;

    try {
        QFile inputFile(inputPath);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            result.success = false;
            result.errorMessage = "No se pudo abrir el archivo de entrada";
            return result;
        }

        QFile outputFile(outputPath);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            result.success = false;
            result.errorMessage = "No se pudo crear el archivo de salida";
            return result;
        }

        QByteArray inputData = inputFile.readAll();
        QByteArray compressedData;

        // Simple zlib compression
        compressedData = qCompress(inputData, 9);

        outputFile.write(compressedData);

        result.success = true;
        result.originalSize = inputData.size();
        result.compressedSize = compressedData.size();
        result.compressionRatio = calculateCompressionRatio(result.originalSize, result.compressedSize);
        result.outputPath = outputPath;

    } catch (const std::exception &e) {
        result.success = false;
        result.errorMessage = QString("Error: %1").arg(e.what());
    }

    return result;
}

CompressionResult Compressor::compressPDF(const QString &inputPath, const QString &outputPath)
{
    // Basic PDF compression - just copy for now
    CompressionResult result;

    try {
        QFile inputFile(inputPath);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            result.success = false;
            result.errorMessage = "No se pudo abrir el archivo PDF";
            return result;
        }

        QFile outputFile(outputPath);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            result.success = false;
            result.errorMessage = "No se pudo crear el archivo PDF de salida";
            return result;
        }

        QByteArray pdfData = inputFile.readAll();
        outputFile.write(pdfData);

        result.success = true;
        result.originalSize = pdfData.size();
        result.compressedSize = pdfData.size();
        result.compressionRatio = 0.0; // No compression for now
        result.outputPath = outputPath;

    } catch (const std::exception &e) {
        result.success = false;
        result.errorMessage = QString("Error: %1").arg(e.what());
    }

    return result;
}
