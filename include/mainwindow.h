#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QProgressBar>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include "compressor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addFiles();
    void clearFiles();
    void selectOutputDirectory();
    void startCompression();
    void onCompressionFinished(const QList<CompressionResult> &results);
    void onErrorOccurred(const QString &error);
    void onProgressUpdated(const QString &message, int percentage);

private:
    void setupUI();
    void createFileSelectionSection();
    void createCompressionOptionsSection();
    void createProgressSection();
    void createResultsSection();
    void createControlButtons();
    void displayResults(const QList<CompressionResult> &results);
    void updateProgressBar(const QString &message, int percentage);
    void enableControls(bool enable);

    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;

    // File selection
    QListWidget *m_fileListWidget;
    QPushButton *m_addFilesButton;
    QPushButton *m_clearFilesButton;
    QLabel *m_outputPathLabel;
    QPushButton *m_selectOutputButton;

    // Compression options
    QButtonGroup *m_compressionTypeGroup;
    QRadioButton *m_zipRadioButton;
    QRadioButton *m_gzipRadioButton;

    // Progress
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;

    // Results
    QTextEdit *m_resultsTextEdit;

    // Control buttons
    QPushButton *m_compressButton;
    QPushButton *m_exitButton;

    // Data
    QStringList m_selectedFiles;
    QString m_outputDirectory;

    // Compressor
    Compressor *m_compressor;
    QThread *m_compressorThread;
    QFutureWatcher<QList<CompressionResult>> *m_compressionWatcher;
};

#endif // MAINWINDOW_H
