#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QLineEdit;
class QPushButton;
class QLabel;
class QListWidget;
QT_END_NAMESPACE

/**
 * ChatRoomScreen – displays messages, a message input bar, and a user list.
 * Emits signals for actions; NO network/business logic lives here.
 */
class ChatRoomScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ChatRoomScreen(QWidget *parent = nullptr);

    /** Called by MainWindow when switching to this screen */
    void configure(const QString &username, const QString &host, int port);

    // ── Feedback API (called by NetworkManager / Controller) ──────────
    void appendMessage(const QString &sender, const QString &text, bool isOwn = false);
    void appendSystemMessage(const QString &text);
    void setNetworkStatus(const QString &status, bool isError = false);
    void setConnected(bool connected);
    void addUser(const QString &username);
    void removeUser(const QString &username);
    void clearUsers();

signals:
    void messageSendRequested(const QString &text);
    void settingsRequested(const QString &username, const QString &host, int port);
    void logoutRequested();

private slots:
    void onSendClicked();

private:
    void buildUi();
    void applyStyles();

    QString     m_username;
    QString     m_host;
    int         m_port = 0;

    QLabel      *m_titleLabel;
    QLabel      *m_statusLabel;
    QTextEdit   *m_chatDisplay;
    QLineEdit   *m_messageInput;
    QPushButton *m_sendBtn;
    QPushButton *m_settingsBtn;
    QPushButton *m_logoutBtn;
    QListWidget *m_userList;
};
