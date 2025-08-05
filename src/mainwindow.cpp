#include "mainwindow.h"
#include "progressdialog.h"
#include <QApplication>
#include <QStyle>
#include <QScreen>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_compressor(new Compressor(this))
    , m_compressorThread(new QThread(this))
    , m_compressionWatcher(new QFutureWatcher<QList<CompressionResult>>(this))
{
    setupUI();
    setupConnections();

    // Center window on screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            qApp->primaryScreen()->availableGeometry()
        )
    );
}

MainWindow::~MainWindow()
{
    if (m_compressorThread->isRunning()) {
        m_compressorThread->quit();
        m_compressorThread->wait();
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Compresor de Archivos - C++");
    setMinimumSize(800, 600);

    // Central widget
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Main layout
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Create sections
    createFileSelectionSection();
    createCompressionOptionsSection();
    createProgressSection();
    createResultsSection();
    createControlButtons();
}

void MainWindow::setupConnections()
{
    // Connect compressor signals
    connect(m_compressor, &Compressor::progressUpdated, this, &MainWindow::onProgressUpdated);
    connect(m_compressor, &Compressor::compressionFinished, this, &MainWindow::onCompressionFinished);
    connect(m_compressor, &Compressor::errorOccurred, this, &MainWindow::onErrorOccurred);

    // Connect compression watcher
    connect(m_compressionWatcher, &QFutureWatcher<QList<CompressionResult>>::finished,
            this, [this]() {
                QList<CompressionResult> results = m_compressionWatcher->result();
                onCompressionFinished(results);
            });
}

void MainWindow::createFileSelectionSection()
{
    QGroupBox *fileGroup = new QGroupBox("Selección de Archivos", this);
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);

    // File list
    m_fileListWidget = new QListWidget(this);
    m_fileListWidget->setMinimumHeight(120);
    fileLayout->addWidget(m_fileListWidget);

    // File buttons
    QHBoxLayout *fileButtonsLayout = new QHBoxLayout();
    m_addFilesButton = new QPushButton("Agregar Archivos", this);
    m_clearFilesButton = new QPushButton("Limpiar Lista", this);

    fileButtonsLayout->addWidget(m_addFilesButton);
    fileButtonsLayout->addWidget(m_clearFilesButton);
    fileButtonsLayout->addStretch();
    fileLayout->addLayout(fileButtonsLayout);

    // Output directory
    QHBoxLayout *outputLayout = new QHBoxLayout();
    QLabel *outputLabel = new QLabel("Directorio de Salida:", this);
    m_outputPathLabel = new QLabel("No seleccionado", this);
    m_outputPathLabel->setStyleSheet("color: gray;");
    m_selectOutputButton = new QPushButton("Seleccionar Directorio", this);

    outputLayout->addWidget(outputLabel);
    outputLayout->addWidget(m_outputPathLabel, 1);
    outputLayout->addWidget(m_selectOutputButton);
    fileLayout->addLayout(outputLayout);

    m_mainLayout->addWidget(fileGroup);
}

void MainWindow::createCompressionOptionsSection()
{
    QGroupBox *optionsGroup = new QGroupBox("Opciones de Compresión", this);
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);

    // Compression type
    QHBoxLayout *compressionLayout = new QHBoxLayout();
    QLabel *compressionLabel = new QLabel("Tipo de Compresión:", this);
    m_compressionTypeGroup = new QButtonGroup(this);
    m_zipRadioButton = new QRadioButton("ZIP (Recomendado)", this);
    m_gzipRadioButton = new QRadioButton("GZIP", this);

    m_zipRadioButton->setChecked(true);
    m_compressionTypeGroup->addButton(m_zipRadioButton);
    m_compressionTypeGroup->addButton(m_gzipRadioButton);

    compressionLayout->addWidget(compressionLabel);
    compressionLayout->addWidget(m_zipRadioButton);
    compressionLayout->addWidget(m_gzipRadioButton);
    compressionLayout->addStretch();
    optionsLayout->addLayout(compressionLayout);

    // Supported formats info
    QLabel *formatsLabel = new QLabel("Formatos Soportados:", this);
    formatsLabel->setStyleSheet("font-weight: bold;");
    optionsLayout->addWidget(formatsLabel);

    QLabel *formatsText = new QLabel(
        "• Imágenes: PNG, JPG, JPEG, BMP, TIFF\n"
        "• Documentos: PDF\n"
        "• Otros: Todos los formatos (ZIP/GZIP)", this);
    optionsLayout->addWidget(formatsText);

    m_mainLayout->addWidget(optionsGroup);
}

void MainWindow::createProgressSection()
{
    QGroupBox *progressGroup = new QGroupBox("Progreso", this);
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    progressLayout->addWidget(m_progressBar);

    m_progressLabel = new QLabel("Listo para comprimir", this);
    m_progressLabel->setStyleSheet("color: gray;");
    progressLayout->addWidget(m_progressLabel);

    m_mainLayout->addWidget(progressGroup);
}

void MainWindow::createResultsSection()
{
    QGroupBox *resultsGroup = new QGroupBox("Resultados", this);
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroup);

    m_resultsTextEdit = new QTextEdit(this);
    m_resultsTextEdit->setReadOnly(true);
    m_resultsTextEdit->setMinimumHeight(150);
    m_resultsTextEdit->setFont(QFont("Consolas", 9));
    resultsLayout->addWidget(m_resultsTextEdit);

    m_mainLayout->addWidget(resultsGroup, 1); // Give it more space
}

void MainWindow::createControlButtons()
{
    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    m_compressButton = new QPushButton("Comprimir Archivos", this);
    m_compressButton->setMinimumHeight(40);
    m_compressButton->setStyleSheet(
        "QPushButton { font-weight: bold; font-size: 14px; }"
    );

    m_exitButton = new QPushButton("Salir", this);
    m_exitButton->setMinimumHeight(40);
    m_exitButton->setStyleSheet(
        "QPushButton { background-color: #d32f2f; color: white; font-weight: bold; }"
        "QPushButton:hover { background-color: #b71c1c; }"
    );

    buttonsLayout->addWidget(m_compressButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_exitButton);

    m_mainLayout->addLayout(buttonsLayout);

    // Connect exit button
    connect(m_exitButton, &QPushButton::clicked, this, &QWidget::close);
}

void MainWindow::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Seleccionar archivos para comprimir",
        QDir::homePath(),
        "Todos los archivos (*.*);;Imágenes (*.png *.jpg *.jpeg *.bmp *.tiff);;PDFs (*.pdf);;Archivos de texto (*.txt *.doc *.docx)"
    );

    for (const QString &file : files) {
        if (!m_selectedFiles.contains(file)) {
            m_selectedFiles.append(file);
            m_fileListWidget->addItem(QFileInfo(file).fileName());
        }
    }
}

void MainWindow::clearFiles()
{
    m_selectedFiles.clear();
    m_fileListWidget->clear();
}

void MainWindow::selectOutputDirectory()
{
    QString directory = QFileDialog::getExistingDirectory(
        this,
        "Seleccionar directorio de salida",
        QDir::homePath()
    );

    if (!directory.isEmpty()) {
        m_outputDirectory = directory;
        m_outputPathLabel->setText(directory);
        m_outputPathLabel->setStyleSheet("color: black;");
    }
}

void MainWindow::startCompression()
{
    if (m_selectedFiles.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "Por favor selecciona al menos un archivo.");
        return;
    }

    if (m_outputDirectory.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "Por favor selecciona un directorio de salida.");
        return;
    }

    // Disable controls
    enableControls(false);

    // Clear results
    m_resultsTextEdit->clear();

    // Get compression type
    QString compressionType = m_zipRadioButton->isChecked() ? "zip" : "gzip";

    // Set progress callback
    m_compressor->setProgressCallback([this](const QString &message, int percentage) {
        QMetaObject::invokeMethod(this, [this, message, percentage]() {
            updateProgressBar(message, percentage);
        }, Qt::QueuedConnection);
    });

    // Start compression in background
    QFuture<QList<CompressionResult>> future = QtConcurrent::run([this, compressionType]() {
        return m_compressor->compressMultipleFiles(m_selectedFiles, m_outputDirectory, compressionType);
    });

    m_compressionWatcher->setFuture(future);
}

void MainWindow::onCompressionFinished(const QList<CompressionResult> &results)
{
    displayResults(results);
    enableControls(true);

    QMessageBox::information(this, "Completado", "Compresión completada exitosamente!");
}

void MainWindow::onErrorOccurred(const QString &error)
{
    QMessageBox::critical(this, "Error", error);
    enableControls(true);
}

void MainWindow::onProgressUpdated(const QString &message, int percentage)
{
    updateProgressBar(message, percentage);
}

void MainWindow::displayResults(const QList<CompressionResult> &results)
{
    m_resultsTextEdit->clear();

    qint64 totalOriginal = 0;
    qint64 totalCompressed = 0;
    int successful = 0;

    for (const CompressionResult &result : results) {
        if (result.success) {
            successful++;
            totalOriginal += result.originalSize;
            totalCompressed += result.compressedSize;

            // Format sizes
            double originalMB = result.originalSize / (1024.0 * 1024.0);
            double compressedMB = result.compressedSize / (1024.0 * 1024.0);

            m_resultsTextEdit->append(QString("✓ %1").arg(result.filename));
            m_resultsTextEdit->append(QString("   Original: %1 MB").arg(originalMB, 0, 'f', 2));
            m_resultsTextEdit->append(QString("   Comprimido: %1 MB").arg(compressedMB, 0, 'f', 2));
            m_resultsTextEdit->append(QString("   Compresión: %1%").arg(result.compressionRatio, 0, 'f', 1));
            m_resultsTextEdit->append("");
        } else {
            m_resultsTextEdit->append(QString("✗ %1: %2").arg(result.filename, result.errorMessage));
            m_resultsTextEdit->append("");
        }
    }

    // Summary
    if (successful > 0) {
        double totalOriginalMB = totalOriginal / (1024.0 * 1024.0);
        double totalCompressedMB = totalCompressed / (1024.0 * 1024.0);
        double totalCompression = ((totalOriginal - totalCompressed) * 100.0) / totalOriginal;

        m_resultsTextEdit->append(QString(50, '='));
        m_resultsTextEdit->append("RESUMEN:");
        m_resultsTextEdit->append(QString("Archivos procesados: %1/%2").arg(successful).arg(results.size()));
        m_resultsTextEdit->append(QString("Tamaño total original: %1 MB").arg(totalOriginalMB, 0, 'f', 2));
        m_resultsTextEdit->append(QString("Tamaño total comprimido: %1 MB").arg(totalCompressedMB, 0, 'f', 2));
        m_resultsTextEdit->append(QString("Compresión total: %1%").arg(totalCompression, 0, 'f', 1));
    }
}

void MainWindow::updateProgressBar(const QString &message, int percentage)
{
    m_progressBar->setValue(percentage);
    m_progressLabel->setText(message);
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
