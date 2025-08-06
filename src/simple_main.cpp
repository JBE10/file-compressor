#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QImage>
#include <QImageWriter>
#include <QBuffer>
#include <QDataStream>
#include <QIODevice>
#include <zlib.h>
#include <iostream>

struct CompressionResult
{
    bool success = false;
    QString filename;
    QString outputPath;
    qint64 originalSize = 0;
    qint64 compressedSize = 0;
    double compressionRatio = 0.0;
    QString errorMessage;
};

class SimpleCompressor
{
public:
    static CompressionResult compressFile(const QString &inputPath, const QString &outputPath)
    {
        CompressionResult result;

        try {
            QFileInfo fileInfo(inputPath);
            QString extension = fileInfo.suffix().toLower();

            if (extension == "png" || extension == "jpg" || extension == "jpeg" ||
                extension == "bmp" || extension == "tiff" || extension == "tif") {
                return compressImage(inputPath, outputPath);
            } else {
                return compressGeneralFile(inputPath, outputPath);
            }
        } catch (const std::exception &e) {
            result.success = false;
            result.errorMessage = QString("Error: %1").arg(e.what());
            return result;
        }
    }

private:
    static CompressionResult compressImage(const QString &inputPath, const QString &outputPath)
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
                result.compressionRatio = ((double)(result.originalSize - result.compressedSize) / result.originalSize) * 100.0;
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

    static CompressionResult compressGeneralFile(const QString &inputPath, const QString &outputPath)
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
            QByteArray compressedData = qCompress(inputData, 9);

            outputFile.write(compressedData);

            result.success = true;
            result.originalSize = inputData.size();
            result.compressedSize = compressedData.size();
            result.compressionRatio = ((double)(result.originalSize - result.compressedSize) / result.originalSize) * 100.0;
            result.outputPath = outputPath;

        } catch (const std::exception &e) {
            result.success = false;
            result.errorMessage = QString("Error: %1").arg(e.what());
        }

        return result;
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    std::cout << "🚀 Compresor de Archivos - Versión Simple" << std::endl;
    std::cout << "==========================================" << std::endl;

    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <archivo_a_comprimir>" << std::endl;
        return 1;
    }

    QString inputFile = argv[1];
    QFileInfo fileInfo(inputFile);

    if (!fileInfo.exists()) {
        std::cout << "❌ Error: El archivo no existe: " << inputFile.toStdString() << std::endl;
        return 1;
    }

    // Create output directory
    QDir outputDir("output");
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    QString outputFile = QString("output/%1_compressed.%2")
                        .arg(fileInfo.baseName())
                        .arg(fileInfo.suffix());

    std::cout << "📁 Archivo de entrada: " << inputFile.toStdString() << std::endl;
    std::cout << "📁 Archivo de salida: " << outputFile.toStdString() << std::endl;
    std::cout << "🔨 Comprimiendo..." << std::endl;

    CompressionResult result = SimpleCompressor::compressFile(inputFile, outputFile);

    if (result.success) {
        std::cout << "✅ Compresión exitosa!" << std::endl;
        std::cout << "📊 Tamaño original: " << result.originalSize << " bytes" << std::endl;
        std::cout << "📊 Tamaño comprimido: " << result.compressedSize << " bytes" << std::endl;
        std::cout << "📈 Ratio de compresión: " << QString::number(result.compressionRatio, 'f', 2).toStdString() << "%" << std::endl;
        std::cout << "📁 Archivo guardado en: " << result.outputPath.toStdString() << std::endl;
    } else {
        std::cout << "❌ Error en la compresión: " << result.errorMessage.toStdString() << std::endl;
        return 1;
    }

    return 0;
}
