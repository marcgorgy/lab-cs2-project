#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QLabel;
class QProgressBar;
QT_END_NAMESPACE

/**
 * LoginScreen – collects username, host, port.
 * Emits loginRequested(); all validation feedback stays in this class.
 * NO business/network logic here.
 */
class LoginScreen : public QWidget
{
    Q_OBJECT

public:
    explicit LoginScreen(QWidget *parent = nullptr);

    void reset();

    // Called by MainWindow (or NetworkManager) to show network feedback
    void setConnecting(bool connecting);
    void setErrorMessage(const QString &msg);

signals:
    void loginRequested(const QString &username, const QString &host, int port);

private slots:
    void onConnectClicked();

private:
    void buildUi();
    void applyStyles();
    bool validateInputs();   // returns false + shows inline error if invalid

    QLineEdit   *m_usernameEdit;
    QLineEdit   *m_hostEdit;
    QLineEdit   *m_portEdit;
    QPushButton *m_connectBtn;
    QLabel      *m_errorLabel;
    QProgressBar *m_progressBar;
};
