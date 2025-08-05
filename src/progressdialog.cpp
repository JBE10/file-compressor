#include "progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Progreso de Compresión");
    setFixedSize(400, 150);
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    layout->addWidget(m_progressBar);

    // Message label
    m_messageLabel = new QLabel("Iniciando compresión...", this);
    layout->addWidget(m_messageLabel);

    // Cancel button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_cancelButton = new QPushButton("Cancelar", this);
    connect(m_cancelButton, &QPushButton::clicked, this, &ProgressDialog::cancelled);
    buttonLayout->addWidget(m_cancelButton);

    layout->addLayout(buttonLayout);
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::updateProgress(const QString &message, int percentage)
{
    m_progressBar->setValue(percentage);
    m_messageLabel->setText(message);
}

void ProgressDialog::setCancelEnabled(bool enabled)
{
    m_cancelButton->setEnabled(enabled);
}
