#include "gui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QApplication>
#include <QScreen>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_compressionWatcher(nullptr)
    , m_isCompressing(false)
{
    setWindowTitle("🚀 Compresor de Archivos - Versión GUI");
    setMinimumSize(800, 600);
    resize(1000, 700);
    
    // Center window on screen
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    setupUI();
    setupConnections();
    loadSettings();
    
    // Set window icon
    setWindowIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Header
    createHeader(mainLayout);
    
    // File selection section
    createFileSelectionSection(mainLayout);
    
    // Compression options section
    createCompressionOptionsSection(mainLayout);
    
    // Progress section
    createProgressSection(mainLayout);
    
    // Results section
    createResultsSection(mainLayout);
    
    // Control buttons
    createControlButtons(mainLayout);
    
    // Status bar
    createStatusBar();
}

void MainWindow::createHeader(QVBoxLayout *mainLayout)
{
    QFrame *headerFrame = new QFrame;
    headerFrame->setFrameStyle(QFrame::StyledPanel);
    headerFrame->setStyleSheet("QFrame { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2c3e50, stop:1 #34495e); border-radius: 10px; }");
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    
    QLabel *titleLabel = new QLabel("🚀 Compresor de Archivos");
    titleLabel->setStyleSheet("QLabel { color: white; font-size: 24px; font-weight: bold; }");
    
    QLabel *versionLabel = new QLabel("v2.0 - GUI");
    versionLabel->setStyleSheet("QLabel { color: #bdc3c7; font-size: 14px; }");
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(versionLabel);
    
    mainLayout->addWidget(headerFrame);
}

void MainWindow::createFileSelectionSection(QVBoxLayout *mainLayout)
{
    QGroupBox *fileGroup = new QGroupBox("📁 Selección de Archivos");
    fileGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; }");
    
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);
    
    // File list
    m_fileListWidget = new QListWidget;
    m_fileListWidget->setMinimumHeight(150);
    m_fileListWidget->setStyleSheet("QListWidget { background-color: #2c3e50; border: 2px solid #34495e; border-radius: 5px; }");
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    
    m_addFilesButton = new QPushButton("➕ Agregar Archivos");
    m_addFilesButton->setStyleSheet("QPushButton { background-color: #27ae60; color: white; border: none; padding: 10px; border-radius: 5px; font-weight: bold; } QPushButton:hover { background-color: #2ecc71; }");
    
    m_clearFilesButton = new QPushButton("🗑️ Limpiar Lista");
    m_clearFilesButton->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border: none; padding: 10px; border-radius: 5px; font-weight: bold; } QPushButton:hover { background-color: #c0392b; }");
    
    m_selectOutputButton = new QPushButton("📂 Carpeta de Salida");
    m_selectOutputButton->setStyleSheet("QPushButton { background-color: #3498db; color: white; border: none; padding: 10px; border-radius: 5px; font-weight: bold; } QPushButton:hover { background-color: #2980b9; }");
    
    buttonLayout->addWidget(m_addFilesButton);
    buttonLayout->addWidget(m_clearFilesButton);
    buttonLayout->addWidget(m_selectOutputButton);
    buttonLayout->addStretch();
    
    // Output path label
    m_outputPathLabel = new QLabel("📁 Salida: output/");
    m_outputPathLabel->setStyleSheet("QLabel { color: #95a5a6; font-size: 12px; }");
    
    fileLayout->addWidget(m_fileListWidget);
    fileLayout->addLayout(buttonLayout);
    fileLayout->addWidget(m_outputPathLabel);
    
    mainLayout->addWidget(fileGroup);
}

void MainWindow::createCompressionOptionsSection(QVBoxLayout *mainLayout)
{
    QGroupBox *optionsGroup = new QGroupBox("⚙️ Opciones de Compresión");
    optionsGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; }");
    
    QGridLayout *optionsLayout = new QGridLayout(optionsGroup);
    
    // Compression type
    QLabel *typeLabel = new QLabel("Tipo de Compresión:");
    m_compressionTypeCombo = new QComboBox;
    m_compressionTypeCombo->addItems({"ZIP", "GZIP", "BZIP2"});
    m_compressionTypeCombo->setStyleSheet("QComboBox { background-color: #34495e; border: 2px solid #2c3e50; border-radius: 5px; padding: 5px; color: white; }");
    
    // Compression level
    QLabel *levelLabel = new QLabel("Nivel de Compresión:");
    m_compressionLevelSlider = new QSlider(Qt::Horizontal);
    m_compressionLevelSlider->setRange(1, 9);
    m_compressionLevelSlider->setValue(6);
    m_compressionLevelSlider->setStyleSheet("QSlider::groove:horizontal { border: 1px solid #2c3e50; height: 8px; background: #34495e; border-radius: 4px; } QSlider::handle:horizontal { background: #3498db; border: 1px solid #2980b9; width: 18px; margin: -2px 0; border-radius: 9px; }");
    
    m_compressionLevelLabel = new QLabel("6");
    m_compressionLevelLabel->setStyleSheet("QLabel { color: #3498db; font-weight: bold; }");
    
    // Quality for images
    QLabel *qualityLabel = new QLabel("Calidad de Imagen:");
    m_imageQualitySlider = new QSlider(Qt::Horizontal);
    m_imageQualitySlider->setRange(10, 100);
    m_imageQualitySlider->setValue(85);
    m_imageQualitySlider->setStyleSheet("QSlider::groove:horizontal { border: 1px solid #2c3e50; height: 8px; background: #34495e; border-radius: 4px; } QSlider::handle:horizontal { background: #e67e22; border: 1px solid #d35400; width: 18px; margin: -2px 0; border-radius: 9px; }");
    
    m_imageQualityLabel = new QLabel("85%");
    m_imageQualityLabel->setStyleSheet("QLabel { color: #e67e22; font-weight: bold; }");
    
    // Options
    m_preserveStructureCheck = new QCheckBox("Preservar estructura de carpetas");
    m_preserveStructureCheck->setStyleSheet("QCheckBox { color: white; } QCheckBox::indicator { width: 18px; height: 18px; }");
    
    m_overwriteCheck = new QCheckBox("Sobrescribir archivos existentes");
    m_overwriteCheck->setStyleSheet("QCheckBox { color: white; } QCheckBox::indicator { width: 18px; height: 18px; }");
    
    // Layout
    optionsLayout->addWidget(typeLabel, 0, 0);
    optionsLayout->addWidget(m_compressionTypeCombo, 0, 1);
    optionsLayout->addWidget(levelLabel, 1, 0);
    optionsLayout->addWidget(m_compressionLevelSlider, 1, 1);
    optionsLayout->addWidget(m_compressionLevelLabel, 1, 2);
    optionsLayout->addWidget(qualityLabel, 2, 0);
    optionsLayout->addWidget(m_imageQualitySlider, 2, 1);
    optionsLayout->addWidget(m_imageQualityLabel, 2, 2);
    optionsLayout->addWidget(m_preserveStructureCheck, 3, 0, 1, 2);
    optionsLayout->addWidget(m_overwriteCheck, 4, 0, 1, 2);
    
    mainLayout->addWidget(optionsGroup);
}

void MainWindow::createProgressSection(QVBoxLayout *mainLayout)
{
    QGroupBox *progressGroup = new QGroupBox("📊 Progreso");
    progressGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; }");
    
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);
    
    m_progressBar = new QProgressBar;
    m_progressBar->setStyleSheet("QProgressBar { border: 2px solid #34495e; border-radius: 5px; text-align: center; background-color: #2c3e50; } QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #27ae60, stop:1 #2ecc71); border-radius: 3px; }");
    
    m_progressLabel = new QLabel("Listo para comprimir");
    m_progressLabel->setStyleSheet("QLabel { color: #95a5a6; font-size: 12px; }");
    
    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_progressLabel);
    
    mainLayout->addWidget(progressGroup);
}

void MainWindow::createResultsSection(QVBoxLayout *mainLayout)
{
    QGroupBox *resultsGroup = new QGroupBox("📈 Resultados");
    resultsGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; }");
    
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroup);
    
    // Results table
    m_resultsTable = new QTableWidget;
    m_resultsTable->setColumnCount(5);
    m_resultsTable->setHorizontalHeaderLabels({"Archivo", "Tamaño Original", "Tamaño Comprimido", "Compresión", "Estado"});
    m_resultsTable->setStyleSheet("QTableWidget { background-color: #2c3e50; border: 2px solid #34495e; border-radius: 5px; gridline-color: #34495e; } QHeaderView::section { background-color: #34495e; color: white; padding: 5px; border: none; }");
    m_resultsTable->horizontalHeader()->setStretchLastSection(true);
    m_resultsTable->setMinimumHeight(150);
    
    resultsLayout->addWidget(m_resultsTable);
    
    mainLayout->addWidget(resultsGroup);
}

void MainWindow::createControlButtons(QVBoxLayout *mainLayout)
{
    QHBoxLayout *controlLayout = new QHBoxLayout;
    
    m_compressButton = new QPushButton("🚀 Comprimir Archivos");
    m_compressButton->setStyleSheet("QPushButton { background-color: #27ae60; color: white; border: none; padding: 15px; border-radius: 8px; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: #2ecc71; } QPushButton:pressed { background-color: #229954; }");
    m_compressButton->setMinimumHeight(50);
    
    m_stopButton = new QPushButton("⏹️ Detener");
    m_stopButton->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border: none; padding: 15px; border-radius: 8px; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: #c0392b; }");
    m_stopButton->setMinimumHeight(50);
    m_stopButton->setEnabled(false);
    
    m_clearResultsButton = new QPushButton("🗑️ Limpiar Resultados");
    m_clearResultsButton->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 15px; border-radius: 8px; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: #7f8c8d; }");
    m_clearResultsButton->setMinimumHeight(50);
    
    controlLayout->addWidget(m_compressButton);
    controlLayout->addWidget(m_stopButton);
    controlLayout->addWidget(m_clearResultsButton);
    
    mainLayout->addLayout(controlLayout);
}

void MainWindow::createStatusBar()
{
    statusBar()->setStyleSheet("QStatusBar { background-color: #2c3e50; color: white; }");
    statusBar()->showMessage("Listo");
}

void MainWindow::setupConnections()
{
    // File selection
    connect(m_addFilesButton, &QPushButton::clicked, this, &MainWindow::addFiles);
    connect(m_clearFilesButton, &QPushButton::clicked, this, &MainWindow::clearFiles);
    connect(m_selectOutputButton, &QPushButton::clicked, this, &MainWindow::selectOutputDirectory);
    
    // Compression controls
    connect(m_compressButton, &QPushButton::clicked, this, &MainWindow::startCompression);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::stopCompression);
    connect(m_clearResultsButton, &QPushButton::clicked, this, &MainWindow::clearResults);
    
    // Sliders
    connect(m_compressionLevelSlider, &QSlider::valueChanged, [this](int value) {
        m_compressionLevelLabel->setText(QString::number(value));
    });
    
    connect(m_imageQualitySlider, &QSlider::valueChanged, [this](int value) {
        m_imageQualityLabel->setText(QString::number(value) + "%");
    });
    
    // Drag and drop
    setAcceptDrops(true);
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
        for (const QString &file : files) {
            if (!m_selectedFiles.contains(file)) {
                m_selectedFiles.append(file);
                QListWidgetItem *item = new QListWidgetItem(QFileInfo(file).fileName());
                item->setData(Qt::UserRole, file);
                m_fileListWidget->addItem(item);
            }
        }
        updateStatus();
    }
}

void MainWindow::clearFiles()
{
    m_selectedFiles.clear();
    m_fileListWidget->clear();
    updateStatus();
}

void MainWindow::selectOutputDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "Seleccionar carpeta de salida",
        m_outputDirectory.isEmpty() ? QDir::homePath() : m_outputDirectory
    );
    
    if (!dir.isEmpty()) {
        m_outputDirectory = dir;
        m_outputPathLabel->setText("📁 Salida: " + dir);
        updateStatus();
    }
}

void MainWindow::startCompression()
{
    if (m_selectedFiles.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "Por favor selecciona al menos un archivo para comprimir.");
        return;
    }
    
    if (m_outputDirectory.isEmpty()) {
        m_outputDirectory = "output";
        QDir().mkpath(m_outputDirectory);
        m_outputPathLabel->setText("📁 Salida: " + m_outputDirectory);
    }
    
    m_isCompressing = true;
    m_compressButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_progressBar->setValue(0);
    m_progressLabel->setText("Iniciando compresión...");
    
    // Clear previous results
    m_resultsTable->setRowCount(0);
    
    // Start compression in background
    QFuture<void> future = QtConcurrent::run([this]() {
        compressFiles();
    });
    
    m_compressionWatcher = new QFutureWatcher<void>(this);
    connect(m_compressionWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::onCompressionFinished);
    m_compressionWatcher->setFuture(future);
}

void MainWindow::stopCompression()
{
    if (m_compressionWatcher && m_compressionWatcher->isRunning()) {
        m_compressionWatcher->cancel();
        m_compressionWatcher->waitForFinished();
    }
    
    m_isCompressing = false;
    m_compressButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_progressLabel->setText("Compresión detenida");
}

void MainWindow::clearResults()
{
    m_resultsTable->setRowCount(0);
    updateStatus();
}

void MainWindow::compressFiles()
{
    int totalFiles = m_selectedFiles.size();
    
    for (int i = 0; i < totalFiles && !m_compressionWatcher->isCanceled(); ++i) {
        const QString &filePath = m_selectedFiles[i];
        QFileInfo fileInfo(filePath);
        
        // Update progress
        int progress = ((i + 1) * 100) / totalFiles;
        QMetaObject::invokeMethod(this, [this, progress, fileName = fileInfo.fileName()]() {
            m_progressBar->setValue(progress);
            m_progressLabel->setText(QString("Comprimiendo %1 (%2%)").arg(fileName).arg(progress));
        }, Qt::QueuedConnection);
        
        // Compress file
        QString outputPath = m_outputDirectory + "/" + fileInfo.baseName() + "_compressed" + fileInfo.suffix();
        
        // Use the pure C++ compressor
        CompressionResult result = PureCppCompressor::compressFile(filePath.toStdString(), outputPath.toStdString());
        
        // Add result to table
        QMetaObject::invokeMethod(this, [this, result, fileInfo]() {
            addResultToTable(result, fileInfo.fileName());
        }, Qt::QueuedConnection);
        
        // Small delay to show progress
        QThread::msleep(100);
    }
}

void MainWindow::addResultToTable(const CompressionResult &result, const QString &fileName)
{
    int row = m_resultsTable->rowCount();
    m_resultsTable->insertRow(row);
    
    m_resultsTable->setItem(row, 0, new QTableWidgetItem(fileName));
    m_resultsTable->setItem(row, 1, new QTableWidgetItem(formatFileSize(result.originalSize)));
    m_resultsTable->setItem(row, 2, new QTableWidgetItem(formatFileSize(result.compressedSize)));
    m_resultsTable->setItem(row, 3, new QTableWidgetItem(QString::number(result.compressionRatio, 'f', 2) + "%"));
    
    QTableWidgetItem *statusItem = new QTableWidgetItem(result.success ? "✅ Exitoso" : "❌ Error");
    statusItem->setForeground(result.success ? QColor("#27ae60") : QColor("#e74c3c"));
    m_resultsTable->setItem(row, 4, statusItem);
}

QString MainWindow::formatFileSize(size_t bytes)
{
    const QStringList units = {"B", "KB", "MB", "GB"};
    double size = bytes;
    int unitIndex = 0;
    
    while (size >= 1024.0 && unitIndex < units.size() - 1) {
        size /= 1024.0;
        unitIndex++;
    }
    
    return QString::number(size, 'f', 2) + " " + units[unitIndex];
}

void MainWindow::onCompressionFinished()
{
    m_isCompressing = false;
    m_compressButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_progressBar->setValue(100);
    m_progressLabel->setText("Compresión completada");
    
    if (m_compressionWatcher->isCanceled()) {
        statusBar()->showMessage("Compresión cancelada");
    } else {
        statusBar()->showMessage("Compresión completada exitosamente");
    }
}

void MainWindow::updateStatus()
{
    QString status = QString("Archivos seleccionados: %1").arg(m_selectedFiles.size());
    statusBar()->showMessage(status);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    m_outputDirectory = settings.value("outputDirectory", "output").toString();
    m_compressionTypeCombo->setCurrentText(settings.value("compressionType", "ZIP").toString());
    m_compressionLevelSlider->setValue(settings.value("compressionLevel", 6).toInt());
    m_imageQualitySlider->setValue(settings.value("imageQuality", 85).toInt());
    m_preserveStructureCheck->setChecked(settings.value("preserveStructure", false).toBool());
    m_overwriteCheck->setChecked(settings.value("overwrite", false).toBool());
    
    if (!m_outputDirectory.isEmpty()) {
        m_outputPathLabel->setText("📁 Salida: " + m_outputDirectory);
    }
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("outputDirectory", m_outputDirectory);
    settings.setValue("compressionType", m_compressionTypeCombo->currentText());
    settings.setValue("compressionLevel", m_compressionLevelSlider->value());
    settings.setValue("imageQuality", m_imageQualitySlider->value());
    settings.setValue("preserveStructure", m_preserveStructureCheck->isChecked());
    settings.setValue("overwrite", m_overwriteCheck->isChecked());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    
    if (mimeData->hasUrls()) {
        for (const QUrl &url : mimeData->urls()) {
            QString filePath = url.toLocalFile();
            if (!filePath.isEmpty() && !m_selectedFiles.contains(filePath)) {
                m_selectedFiles.append(filePath);
                QListWidgetItem *item = new QListWidgetItem(QFileInfo(filePath).fileName());
                item->setData(Qt::UserRole, filePath);
                m_fileListWidget->addItem(item);
            }
        }
        updateStatus();
    }
} 