#pragma once
#ifndef APPLOGIC_H
#define APPLOGIC_H

#include <QObject>
#include <QString>
#include <QStringList>

class AppLogic : public QObject
{
    Q_OBJECT

public:
    explicit AppLogic(QObject *parent = nullptr);

    // ─── INPUTS (Slots) ──────────────────────────────────────────────────────
public slots:
    // From GUI: Login Screen
    void onGuiLoginRequested(const QString &username, const QString &host, int port);

    // From GUI: Chat Screen
    void onGuiMessageSendRequested(const QString &text);
    void onGuiLogoutRequested();

    // From Network: Connection status
    void onNetworkConnected();
    void onNetworkConnectionFailed(const QString &errorMsg);
    void onNetworkDisconnected();

    // From Network: Incoming data
    void onNetworkIncomingMessage(const QString &sender, const QString &text);
    void onNetworkUserJoined(const QString &username);
    void onNetworkUserLeft(const QString &username);
    void onNetworkUserListReceived(const QStringList &users);

    // From Network: Group events
    void onNetworkGroupMessage(const QString &group, const QString &sender, const QString &text);
    void onNetworkGroupJoined(const QString &group, const QString &username);

    // ─── OUTPUTS (Signals) ───────────────────────────────────────────────────
signals:
    // To GUI: Login Screen feedback
    void showLoginError(const QString &msg);
    void setLoginConnectingState(bool connecting);

    // To GUI: Navigation
    void navigateToChatScreen(const QString &username, const QString &host, int port);
    void navigateToLoginScreen();

    // To GUI: Chat Screen feedback
    void appendChatMessage(const QString &sender, const QString &text, bool isOwn);
    void appendSystemMessage(const QString &text);
    void updateNetworkStatus(const QString &status, bool isError);
    void chatScreenAddUser(const QString &username);
    void chatScreenRemoveUser(const QString &username);
    void chatScreenClearUsers();
    void setChatInputEnabled(bool enabled);

    // To GUI: Group feedback
    void chatScreenSetGroup(const QString &groupName);

    // To Network Layer
    void requestNetworkConnect(const QString &username, const QString &host, int port);
    void requestNetworkSendChat(const QString &text);
    void requestNetworkDisconnect();
    void requestNetworkGroupJoin(const QString &group);
    void requestNetworkGroupMessage(const QString &group, const QString &text);

    // ─── STATE (Private) ─────────────────────────────────────────────────────
private:
    QString     m_myUsername;
    QString     m_host;
    int         m_port      = 0;
    bool        m_isLoggedIn = false;
    QString     m_currentGroup; // empty = global channel
    QStringList m_onlineUsers;
};

#endif // APPLOGIC_H

// Mohamed Fadul Mohamed