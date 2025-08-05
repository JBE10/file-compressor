#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <functional>
#include <memory>

// Forward declarations
struct CompressionResult;

class Compressor : public QObject
{
    Q_OBJECT

public:
    explicit Compressor(QObject *parent = nullptr);
    ~Compressor();

    // Compression methods
    CompressionResult compressFile(const QString &inputPath, const QString &outputPath, const QString &compressionType = "zip");
    QList<CompressionResult> compressMultipleFiles(const QStringList &filePaths, const QString &outputDir, const QString &compressionType = "zip");

    // Progress callback type
    using ProgressCallback = std::function<void(const QString&, int)>;
    void setProgressCallback(ProgressCallback callback);

signals:
    void progressUpdated(const QString &message, int percentage);
    void compressionFinished(const QList<CompressionResult> &results);
    void errorOccurred(const QString &error);

private:
    // Private compression methods
    CompressionResult compressImage(const QString &inputPath, const QString &outputPath);
    CompressionResult compressPDF(const QString &inputPath, const QString &outputPath);
    CompressionResult compressGeneralFile(const QString &inputPath, const QString &outputPath, const QString &compressionType);

    // Helper methods
    QString getFileExtension(const QString &filePath) const;
    qint64 getFileSize(const QString &filePath) const;
    double calculateCompressionRatio(qint64 originalSize, qint64 compressedSize) const;
    void updateProgress(const QString &message, int percentage);

    // Member variables
    ProgressCallback m_progressCallback;
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

// Result structure
struct CompressionResult
{
    bool success = false;
    QString filename;
    QString outputPath;
    qint64 originalSize = 0;
    qint64 compressedSize = 0;
    double compressionRatio = 0.0;
    QString errorMessage;

    CompressionResult() = default;
    CompressionResult(bool s, const QString &f, const QString &o, qint64 orig, qint64 comp, double ratio)
        : success(s), filename(f), outputPath(o), originalSize(orig), compressedSize(comp), compressionRatio(ratio) {}
};

#endif // COMPRESSOR_H
