#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    TestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        QWidget *centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        QLabel *title = new QLabel("Prueba de Botones");
        title->setStyleSheet("font-size: 18px; font-weight: bold;");
        layout->addWidget(title);

        QPushButton *testButton1 = new QPushButton("Agregar Archivos");
        QPushButton *testButton2 = new QPushButton("Seleccionar Directorio");
        QPushButton *testButton3 = new QPushButton("Detener");

        layout->addWidget(testButton1);
        layout->addWidget(testButton2);
        layout->addWidget(testButton3);

        connect(testButton1, &QPushButton::clicked, [this]() {
            QMessageBox::information(this, "Test", "Botón 'Agregar Archivos' funcionando!");
        });

        connect(testButton2, &QPushButton::clicked, [this]() {
            QMessageBox::information(this, "Test", "Botón 'Seleccionar Directorio' funcionando!");
        });

        connect(testButton3, &QPushButton::clicked, [this]() {
            QMessageBox::information(this, "Test", "Botón 'Detener' funcionando!");
        });

        setWindowTitle("Test GUI");
        setMinimumSize(300, 200);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestWindow window;
    window.show();

    return app.exec();
}

#include "test_gui.moc"
