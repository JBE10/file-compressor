#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QTableWidget>
#include <QFutureWatcher>
#include <QStringList>
#include <QString>
#include <QSettings>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QVBoxLayout>
#include <QStatusBar>

// Include the compressor header
#include "gui_compressor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void addFiles();
    void clearFiles();
    void selectOutputDirectory();
    void startCompression();
    void stopCompression();
    void clearResults();
    void onCompressionFinished();

private:
    void setupUI();
    void setupConnections();
    void createHeader(QVBoxLayout *mainLayout);
    void createFileSelectionSection(QVBoxLayout *mainLayout);
    void createCompressionOptionsSection(QVBoxLayout *mainLayout);
    void createProgressSection(QVBoxLayout *mainLayout);
    void createResultsSection(QVBoxLayout *mainLayout);
    void createControlButtons(QVBoxLayout *mainLayout);
    void createStatusBar();

    void compressFiles();
    void addResultToTable(const CompressionResult &result, const QString &fileName);
    QString formatFileSize(size_t bytes);
    void updateStatus();
    void loadSettings();
    void saveSettings();

    // UI Components
    QListWidget *m_fileListWidget;
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;
    QLabel *m_outputPathLabel;
    QPushButton *m_addFilesButton;
    QPushButton *m_clearFilesButton;
    QPushButton *m_selectOutputButton;
    QPushButton *m_compressButton;
    QPushButton *m_stopButton;
    QPushButton *m_clearResultsButton;
    QComboBox *m_compressionTypeCombo;
    QSlider *m_compressionLevelSlider;
    QSlider *m_imageQualitySlider;
    QLabel *m_compressionLevelLabel;
    QLabel *m_imageQualityLabel;
    QCheckBox *m_preserveStructureCheck;
    QCheckBox *m_overwriteCheck;
    QTableWidget *m_resultsTable;

    // Data
    QStringList m_selectedFiles;
    QString m_outputDirectory;
    QFutureWatcher<void> *m_compressionWatcher;
    bool m_isCompressing;
};

#endif // GUI_MAINWINDOW_H
