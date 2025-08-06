#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();

    void updateProgress(const QString &message, int percentage);
    void setCancelEnabled(bool enabled);

signals:
    void cancelled();

private:
    QProgressBar *m_progressBar;
    QLabel *m_messageLabel;
    QPushButton *m_cancelButton;
};

#endif // PROGRESSDIALOG_H
