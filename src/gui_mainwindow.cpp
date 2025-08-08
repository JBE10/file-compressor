#include "gui_mainwindow.h"
#include "gui_compressor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QTableWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QThread>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QHeaderView>
#include <QMimeData>
#include <QDebug> // Added for qDebug

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_fileListWidget(nullptr)
    , m_progressBar(nullptr)
    , m_progressLabel(nullptr)
    , m_outputPathLabel(nullptr)
    , m_addFilesButton(nullptr)
    , m_clearFilesButton(nullptr)
    , m_selectOutputButton(nullptr)
    , m_compressButton(nullptr)
    , m_stopButton(nullptr)
    , m_clearResultsButton(nullptr)
    , m_compressionTypeCombo(nullptr)
    , m_compressionLevelSlider(nullptr)
    , m_imageQualitySlider(nullptr)
    , m_compressionLevelLabel(nullptr)
    , m_imageQualityLabel(nullptr)
    , m_preserveStructureCheck(nullptr)
    , m_overwriteCheck(nullptr)
    , m_resultsTable(nullptr)
    , m_isCompressing(false)
{
    setupUI();
    setupConnections();
    loadSettings();

    // Set window properties
    setWindowTitle("Compresor de Archivos GUI");
    setMinimumSize(800, 600);

    // Center window on screen
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    // Enable drag and drop
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    createHeader(mainLayout);
    createFileSelectionSection(mainLayout);
    createCompressionOptionsSection(mainLayout);
    createProgressSection(mainLayout);
    createResultsSection(mainLayout);
    createControlButtons(mainLayout);
    createStatusBar();
}

void MainWindow::createHeader(QVBoxLayout *mainLayout)
{
    QLabel *titleLabel = new QLabel("Compresor de Archivos");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
}

void MainWindow::createFileSelectionSection(QVBoxLayout *mainLayout)
{
    QGroupBox *fileGroup = new QGroupBox("Selección de Archivos");
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);

    // File list
    m_fileListWidget = new QListWidget;
    m_fileListWidget->setMinimumHeight(150);
    fileLayout->addWidget(m_fileListWidget);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    m_addFilesButton = new QPushButton("Agregar Archivos");
    m_clearFilesButton = new QPushButton("Limpiar Lista");

    // Set button styles
    m_addFilesButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #1976D2; }");
    m_clearFilesButton->setStyleSheet("QPushButton { background-color: #FF5722; color: white; padding: 8px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #D84315; }");

    buttonLayout->addWidget(m_addFilesButton);
    buttonLayout->addWidget(m_clearFilesButton);
    fileLayout->addLayout(buttonLayout);

    // Output directory
    QHBoxLayout *outputLayout = new QHBoxLayout;
    m_outputPathLabel = new QLabel("Directorio de salida: No seleccionado");
    m_selectOutputButton = new QPushButton("Seleccionar Directorio");

    // Set output button style
    m_selectOutputButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #388E3C; }");

    outputLayout->addWidget(m_outputPathLabel);
    outputLayout->addWidget(m_selectOutputButton);
    fileLayout->addLayout(outputLayout);

    mainLayout->addWidget(fileGroup);
}

void MainWindow::createCompressionOptionsSection(QVBoxLayout *mainLayout)
{
    QGroupBox *optionsGroup = new QGroupBox("Opciones de Compresión");
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);

    // Compression type
    QHBoxLayout *typeLayout = new QHBoxLayout;
    QLabel *typeLabel = new QLabel("Tipo de compresión:");
    m_compressionTypeCombo = new QComboBox;
    m_compressionTypeCombo->addItems({"Automático", "ZIP", "GZIP", "Optimizado"});
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(m_compressionTypeCombo);
    optionsLayout->addLayout(typeLayout);

    // Compression level
    QHBoxLayout *levelLayout = new QHBoxLayout;
    m_compressionLevelLabel = new QLabel("Nivel de compresión: 6");
    m_compressionLevelSlider = new QSlider(Qt::Horizontal);
    m_compressionLevelSlider->setRange(1, 9);
    m_compressionLevelSlider->setValue(6);
    levelLayout->addWidget(m_compressionLevelLabel);
    levelLayout->addWidget(m_compressionLevelSlider);
    optionsLayout->addLayout(levelLayout);

    // Image quality
    QHBoxLayout *qualityLayout = new QHBoxLayout;
    m_imageQualityLabel = new QLabel("Calidad de imagen: 85%");
    m_imageQualitySlider = new QSlider(Qt::Horizontal);
    m_imageQualitySlider->setRange(10, 100);
    m_imageQualitySlider->setValue(85);
    qualityLayout->addWidget(m_imageQualityLabel);
    qualityLayout->addWidget(m_imageQualitySlider);
    optionsLayout->addLayout(qualityLayout);

    // Checkboxes
    QHBoxLayout *checkLayout = new QHBoxLayout;
    m_preserveStructureCheck = new QCheckBox("Preservar estructura de directorios");
    m_overwriteCheck = new QCheckBox("Sobrescribir archivos existentes");
    m_preserveStructureCheck->setChecked(true);
    m_overwriteCheck->setChecked(false);
    checkLayout->addWidget(m_preserveStructureCheck);
    checkLayout->addWidget(m_overwriteCheck);
    optionsLayout->addLayout(checkLayout);

    mainLayout->addWidget(optionsGroup);
}

void MainWindow::createProgressSection(QVBoxLayout *mainLayout)
{
    QGroupBox *progressGroup = new QGroupBox("Progreso");
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);

    m_progressBar = new QProgressBar;
    m_progressBar->setVisible(false);
    progressLayout->addWidget(m_progressBar);

    m_progressLabel = new QLabel("Listo para comprimir");
    progressLayout->addWidget(m_progressLabel);

    mainLayout->addWidget(progressGroup);
}

void MainWindow::createResultsSection(QVBoxLayout *mainLayout)
{
    QGroupBox *resultsGroup = new QGroupBox("Resultados");
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroup);

    m_resultsTable = new QTableWidget;
    m_resultsTable->setColumnCount(5);
    m_resultsTable->setHorizontalHeaderLabels({"Archivo", "Tamaño Original", "Tamaño Comprimido", "Ratio", "Estado"});
    m_resultsTable->horizontalHeader()->setStretchLastSection(true);
    m_resultsTable->setMinimumHeight(200);
    resultsLayout->addWidget(m_resultsTable);

    mainLayout->addWidget(resultsGroup);
}

void MainWindow::createControlButtons(QVBoxLayout *mainLayout)
{
    QHBoxLayout *controlLayout = new QHBoxLayout;

    m_compressButton = new QPushButton("Iniciar Compresión");
    m_stopButton = new QPushButton("Detener");
    m_clearResultsButton = new QPushButton("Limpiar Resultados");

    // Set button styles
    m_compressButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #388E3C; }");
    m_stopButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 10px; border-radius: 4px; } QPushButton:hover { background-color: #D32F2F; }");
    m_clearResultsButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 10px; border-radius: 4px; } QPushButton:hover { background-color: #F57C00; }");

    m_stopButton->setEnabled(false);

    controlLayout->addWidget(m_compressButton);
    controlLayout->addWidget(m_stopButton);
    controlLayout->addWidget(m_clearResultsButton);

    mainLayout->addLayout(controlLayout);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("Listo");
}

void MainWindow::setupConnections()
{
    // Connect all button signals
    connect(m_addFilesButton, &QPushButton::clicked, this, &MainWindow::addFiles);
    connect(m_clearFilesButton, &QPushButton::clicked, this, &MainWindow::clearFiles);
    connect(m_selectOutputButton, &QPushButton::clicked, this, &MainWindow::selectOutputDirectory);
    connect(m_compressButton, &QPushButton::clicked, this, &MainWindow::startCompression);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::stopCompression);
    connect(m_clearResultsButton, &QPushButton::clicked, this, &MainWindow::clearResults);

    // Connect slider signals
    connect(m_compressionLevelSlider, &QSlider::valueChanged, [this](int value) {
        m_compressionLevelLabel->setText(QString("Nivel de compresión: %1").arg(value));
    });

    connect(m_imageQualitySlider, &QSlider::valueChanged, [this](int value) {
        m_imageQualityLabel->setText(QString("Calidad de imagen: %1%").arg(value));
    });
}

void MainWindow::addFiles()
{
    qDebug() << "addFiles() called";

    // Use non-native dialog to ensure it works on macOS
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setWindowTitle("Seleccionar archivos para comprimir");
    dialog->setDirectory(QDir::homePath());
    dialog->setFileMode(QFileDialog::ExistingFiles);
    dialog->setNameFilter("Todos los archivos (*.*)");
    dialog->setModal(true);
    dialog->setOption(QFileDialog::DontUseNativeDialog, true); // Force Qt dialog

    // Make sure the dialog is visible
    dialog->resize(600, 400);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();

    if (dialog->exec() == QDialog::Accepted) {
        QStringList files = dialog->selectedFiles();
        qDebug() << "Files selected:" << files;

        if (!files.isEmpty()) {
            for (const QString &file : files) {
                if (!m_selectedFiles.contains(file)) {
                    m_selectedFiles.append(file);
                    m_fileListWidget->addItem(QFileInfo(file).fileName());
                }
            }
            updateStatus();
            QMessageBox::information(this, "Éxito", QString("Se agregaron %1 archivos").arg(files.size()));
        } else {
            QMessageBox::information(this, "Info", "No se seleccionaron archivos");
        }
    } else {
        qDebug() << "Dialog was cancelled";
    }

    dialog->deleteLater();
}

void MainWindow::clearFiles()
{
    QMessageBox::information(this, "Debug", "Botón 'Limpiar Lista' fue presionado");

    m_selectedFiles.clear();
    m_fileListWidget->clear();
    updateStatus();

    QMessageBox::information(this, "Éxito", "Lista de archivos limpiada");
}

void MainWindow::selectOutputDirectory()
{
    qDebug() << "selectOutputDirectory() called";

    // Use non-native dialog to ensure it works on macOS
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setWindowTitle("Seleccionar directorio de salida");
    dialog->setDirectory(QDir::homePath());
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
    dialog->setModal(true);
    dialog->setOption(QFileDialog::DontUseNativeDialog, true); // Force Qt dialog

    // Make sure the dialog is visible
    dialog->resize(600, 400);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();

    if (dialog->exec() == QDialog::Accepted) {
        QStringList dirs = dialog->selectedFiles();
        if (!dirs.isEmpty()) {
            QString dir = dirs.first();
            qDebug() << "Directory selected:" << dir;

            m_outputDirectory = dir;
            m_outputPathLabel->setText("Directorio de salida: " + dir);
            updateStatus();
            QMessageBox::information(this, "Éxito", "Directorio seleccionado: " + dir);
        } else {
            QMessageBox::information(this, "Info", "No se seleccionó ningún directorio");
        }
    } else {
        qDebug() << "Directory dialog was cancelled";
    }

    dialog->deleteLater();
}

void MainWindow::startCompression()
{
    QMessageBox::information(this, "Debug", "Botón 'Iniciar Compresión' fue presionado");

    if (m_selectedFiles.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor selecciona al menos un archivo.");
        return;
    }

    if (m_outputDirectory.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor selecciona un directorio de salida.");
        return;
    }

    m_isCompressing = true;
    m_compressButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, m_selectedFiles.size());
    m_progressBar->setValue(0);
    m_progressLabel->setText("Iniciando compresión...");

    // Clear previous results
    m_resultsTable->setRowCount(0);

    // Start compression synchronously
    compressFiles();
}

void MainWindow::stopCompression()
{
    QMessageBox::information(this, "Debug", "Botón 'Detener' fue presionado");

    m_isCompressing = false;
    m_compressButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_progressBar->setVisible(false);
    m_progressLabel->setText("Compresión detenida");
}

void MainWindow::clearResults()
{
    m_resultsTable->setRowCount(0);
    updateStatus();
}

void MainWindow::compressFiles()
{
    for (int i = 0; i < m_selectedFiles.size(); ++i) {
        if (!m_isCompressing) break;

        QString inputFile = m_selectedFiles[i];
        QFileInfo fileInfo(inputFile);
        QString outputFile = m_outputDirectory + "/" + fileInfo.baseName() + "_compressed";

        CompressionResult result = PureCppCompressor::compressFile(inputFile.toStdString(), outputFile.toStdString());

        addResultToTable(result, fileInfo.fileName());
        m_progressBar->setValue(i + 1);
        m_progressLabel->setText(QString("Comprimiendo %1...").arg(fileInfo.fileName()));

        // Process events to keep UI responsive
        QApplication::processEvents();
    }

    // Compression finished
    m_isCompressing = false;
    m_compressButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_progressBar->setVisible(false);
    m_progressLabel->setText("Compresión completada");
    updateStatus();
}

void MainWindow::addResultToTable(const CompressionResult &result, const QString &fileName)
{
    int row = m_resultsTable->rowCount();
    m_resultsTable->insertRow(row);

    m_resultsTable->setItem(row, 0, new QTableWidgetItem(fileName));
    m_resultsTable->setItem(row, 1, new QTableWidgetItem(formatFileSize(result.originalSize)));
    m_resultsTable->setItem(row, 2, new QTableWidgetItem(formatFileSize(result.compressedSize)));
    m_resultsTable->setItem(row, 3, new QTableWidgetItem(QString("%1%").arg(result.compressionRatio, 0, 'f', 1)));

    QTableWidgetItem *statusItem = new QTableWidgetItem(result.success ? "✓ Exitoso" : "✗ Error");
    statusItem->setForeground(result.success ? Qt::darkGreen : Qt::red);
    m_resultsTable->setItem(row, 4, statusItem);
}

QString MainWindow::formatFileSize(size_t bytes)
{
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double size = bytes;

    while (size >= 1024.0 && unit < 3) {
        size /= 1024.0;
        unit++;
    }

    return QString("%1 %2").arg(size, 0, 'f', 1).arg(units[unit]);
}

void MainWindow::updateStatus()
{
    QString status = QString("Archivos: %1 | Directorio: %2")
        .arg(m_selectedFiles.size())
        .arg(m_outputDirectory.isEmpty() ? "No seleccionado" : QFileInfo(m_outputDirectory).fileName());
    statusBar()->showMessage(status);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    m_outputDirectory = settings.value("outputDirectory", "").toString();
    if (!m_outputDirectory.isEmpty()) {
        m_outputPathLabel->setText("Directorio de salida: " + m_outputDirectory);
    }
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("outputDirectory", m_outputDirectory);
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
                m_fileListWidget->addItem(QFileInfo(filePath).fileName());
            }
        }
        updateStatus();
    }
}
