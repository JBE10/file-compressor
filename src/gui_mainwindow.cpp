#include "mainwindow.h"
#include "gui_compressor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QFileInfo>
#include <QDir>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_fileListWidget(nullptr)
    , m_addFilesButton(nullptr)
    , m_clearFilesButton(nullptr)
    , m_outputPathLabel(nullptr)
    , m_selectOutputButton(nullptr)
    , m_compressionTypeGroup(nullptr)
    , m_zipRadioButton(nullptr)
    , m_gzipRadioButton(nullptr)
    , m_progressBar(nullptr)
    , m_progressLabel(nullptr)
    , m_resultsTextEdit(nullptr)
    , m_compressButton(nullptr)
    , m_exitButton(nullptr)
    // , m_compressor(nullptr)
    , m_compressorThread(nullptr)
    , m_compressionWatcher(nullptr)
{
    setupUI();
    setupConnections();

    // Set window properties
    setWindowTitle("Compresor de Archivos");
    setMinimumSize(600, 500);

    // Center window on screen
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

MainWindow::~MainWindow()
{
    if (m_compressorThread) {
        m_compressorThread->quit();
        m_compressorThread->wait();
    }
}



void MainWindow::setupConnections()
{
    connect(m_addFilesButton, &QPushButton::clicked, this, &MainWindow::addFiles);
    connect(m_clearFilesButton, &QPushButton::clicked, this, &MainWindow::clearFiles);
    connect(m_selectOutputButton, &QPushButton::clicked, this, &MainWindow::selectOutputDirectory);
    connect(m_compressButton, &QPushButton::clicked, this, &MainWindow::startCompression);
    connect(m_exitButton, &QPushButton::clicked, this, &QWidget::close);
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    
    createFileSelectionSection();
    createCompressionOptionsSection();
    createProgressSection();
    createResultsSection();
    createControlButtons();
}

void MainWindow::createFileSelectionSection()
{
    QGroupBox *fileGroup = new QGroupBox("Selección de Archivos");
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);

    // File list
    m_fileListWidget = new QListWidget;
    fileLayout->addWidget(m_fileListWidget);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    m_addFilesButton = new QPushButton("Agregar Archivos");
    m_clearFilesButton = new QPushButton("Limpiar Lista");
    buttonLayout->addWidget(m_addFilesButton);
    buttonLayout->addWidget(m_clearFilesButton);
    fileLayout->addLayout(buttonLayout);

    // Output directory
    QHBoxLayout *outputLayout = new QHBoxLayout;
    m_outputPathLabel = new QLabel("Directorio de salida: No seleccionado");
    m_selectOutputButton = new QPushButton("Seleccionar Directorio");
    outputLayout->addWidget(m_outputPathLabel);
    outputLayout->addWidget(m_selectOutputButton);
    fileLayout->addLayout(outputLayout);

    m_mainLayout->addWidget(fileGroup);
}

void MainWindow::createCompressionOptionsSection()
{
    QGroupBox *optionsGroup = new QGroupBox("Opciones de Compresión");
    QHBoxLayout *optionsLayout = new QHBoxLayout(optionsGroup);

    m_compressionTypeGroup = new QButtonGroup(this);
    m_zipRadioButton = new QRadioButton("ZIP");
    m_gzipRadioButton = new QRadioButton("GZIP");

    m_compressionTypeGroup->addButton(m_zipRadioButton);
    m_compressionTypeGroup->addButton(m_gzipRadioButton);

    m_zipRadioButton->setChecked(true);

    optionsLayout->addWidget(m_zipRadioButton);
    optionsLayout->addWidget(m_gzipRadioButton);
    optionsLayout->addStretch();

    m_mainLayout->addWidget(optionsGroup);
}

void MainWindow::createProgressSection()
{
    QGroupBox *progressGroup = new QGroupBox("Progreso");
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);

    m_progressLabel = new QLabel("Listo para comprimir");
    m_progressBar = new QProgressBar;
    m_progressBar->setVisible(false);

    progressLayout->addWidget(m_progressLabel);
    progressLayout->addWidget(m_progressBar);

    m_mainLayout->addWidget(progressGroup);
}

void MainWindow::createResultsSection()
{
    QGroupBox *resultsGroup = new QGroupBox("Resultados");
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroup);

    m_resultsTextEdit = new QTextEdit;
    m_resultsTextEdit->setReadOnly(true);
    m_resultsTextEdit->setMaximumHeight(150);

    resultsLayout->addWidget(m_resultsTextEdit);

    m_mainLayout->addWidget(resultsGroup);
}

void MainWindow::createControlButtons()
{
    QHBoxLayout *controlLayout = new QHBoxLayout;

    m_compressButton = new QPushButton("Comprimir");
    m_exitButton = new QPushButton("Salir");

    controlLayout->addWidget(m_compressButton);
    controlLayout->addWidget(m_exitButton);

    m_mainLayout->addLayout(controlLayout);
}

void MainWindow::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Seleccionar archivos para comprimir",
        QDir::homePath(),
        "Todos los archivos (*.*)"
    );

    if (!files.isEmpty()) {
        m_selectedFiles.append(files);
        updateFileList();
    }
}

void MainWindow::clearFiles()
{
    m_selectedFiles.clear();
    m_fileListWidget->clear();
}

void MainWindow::selectOutputDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "Seleccionar directorio de salida",
        QDir::homePath()
    );

    if (!dir.isEmpty()) {
        m_outputDirectory = dir;
        m_outputPathLabel->setText("Directorio de salida: " + dir);
    }
}

void MainWindow::startCompression()
{
    if (m_selectedFiles.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor selecciona al menos un archivo.");
        return;
    }

    if (m_outputDirectory.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor selecciona un directorio de salida.");
        return;
    }

    enableControls(false);
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, m_selectedFiles.size());
    m_progressBar->setValue(0);
    m_progressLabel->setText("Iniciando compresión...");

    // Create compressor (using PureCppCompressor instead of Compressor)
    // m_compressor = new Compressor(this);

    // Create watcher for async compression
    m_compressionWatcher = new QFutureWatcher<QList<CompressionResult>>(this);

    connect(m_compressionWatcher, &QFutureWatcher<QList<CompressionResult>>::finished,
            this, [this]() {
                onCompressionFinished(m_compressionWatcher->result());
            });

    // Start compression in background
    QFuture<QList<CompressionResult>> future = QtConcurrent::run([this]() {
        QList<CompressionResult> results;
        for (int i = 0; i < m_selectedFiles.size(); ++i) {
            QString inputFile = m_selectedFiles[i];
            QFileInfo fileInfo(inputFile);
            QString outputFile = m_outputDirectory + "/" + fileInfo.baseName() + ".compressed";

            CompressionResult result = PureCppCompressor::compressFile(inputFile.toStdString(), outputFile.toStdString());
            results.append(result);

            // Update progress
            emit m_compressionWatcher->progressValueChanged(i + 1);
        }
        return results;
    });

    m_compressionWatcher->setFuture(future);
}

void MainWindow::onCompressionFinished(const QList<CompressionResult> &results)
{
    displayResults(results);
    enableControls(true);
    m_progressBar->setVisible(false);
    m_progressLabel->setText("Compresión completada");

    // Cleanup
    // if (m_compressor) {
    //     m_compressor->deleteLater();
    //     m_compressor = nullptr;
    // }
}

void MainWindow::onErrorOccurred(const QString &error)
{
    QMessageBox::critical(this, "Error", error);
    enableControls(true);
    m_progressBar->setVisible(false);
    m_progressLabel->setText("Error en la compresión");
}

void MainWindow::onProgressUpdated(const QString &message, int percentage)
{
    m_progressLabel->setText(message);
    m_progressBar->setValue(percentage);
}

void MainWindow::displayResults(const QList<CompressionResult> &results)
{
    QString resultsText;
    int successCount = 0;
    int totalFiles = results.size();

    for (const CompressionResult &result : results) {
        if (result.success) {
            successCount++;
                resultsText += QString("✓ %1: %2 bytes → %3 bytes (%4% compresión)\n")
                  .arg(QString::fromStdString(result.outputPath))
                  .arg(result.originalSize)
                  .arg(result.compressedSize)
                  .arg(result.compressionRatio, 0, 'f', 1);
        } else {
            resultsText += QString("✗ Error: %1\n")
                          .arg(QString::fromStdString(result.errorMessage));
        }
    }

    resultsText += QString("\nResumen: %1/%2 archivos comprimidos exitosamente")
                  .arg(successCount).arg(totalFiles);

    m_resultsTextEdit->setText(resultsText);
}

void MainWindow::updateProgressBar(const QString &message, int percentage)
{
    m_progressLabel->setText(message);
    m_progressBar->setValue(percentage);
}

void MainWindow::enableControls(bool enable)
{
    m_addFilesButton->setEnabled(enable);
    m_clearFilesButton->setEnabled(enable);
    m_selectOutputButton->setEnabled(enable);
    m_compressButton->setEnabled(enable);
    m_zipRadioButton->setEnabled(enable);
    m_gzipRadioButton->setEnabled(enable);
}

void MainWindow::updateFileList()
{
    m_fileListWidget->clear();
    for (const QString &file : m_selectedFiles) {
        m_fileListWidget->addItem(file);
    }
}
