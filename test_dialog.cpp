#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

class TestDialog : public QMainWindow
{
    Q_OBJECT

public:
    TestDialog(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        QWidget *centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        QPushButton *testFileButton = new QPushButton("Test File Dialog");
        QPushButton *testDirButton = new QPushButton("Test Directory Dialog");

        layout->addWidget(testFileButton);
        layout->addWidget(testDirButton);

        connect(testFileButton, &QPushButton::clicked, [this]() {
            qDebug() << "File button clicked";
            QStringList files = QFileDialog::getOpenFileNames(
                this,
                "Test File Dialog",
                QDir::homePath(),
                "All files (*.*)"
            );
            qDebug() << "Selected files:" << files;
            if (!files.isEmpty()) {
                QMessageBox::information(this, "Success", "Files selected: " + files.join(", "));
            } else {
                QMessageBox::information(this, "Info", "No files selected");
            }
        });

        connect(testDirButton, &QPushButton::clicked, [this]() {
            qDebug() << "Directory button clicked";
            QString dir = QFileDialog::getExistingDirectory(
                this,
                "Test Directory Dialog",
                QDir::homePath()
            );
            qDebug() << "Selected directory:" << dir;
            if (!dir.isEmpty()) {
                QMessageBox::information(this, "Success", "Directory selected: " + dir);
            } else {
                QMessageBox::information(this, "Info", "No directory selected");
            }
        });

        setWindowTitle("Test Dialogs");
        setMinimumSize(300, 200);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestDialog window;
    window.show();

    return app.exec();
}

#include "test_dialog.moc"
