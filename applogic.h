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

    // ─── INPUTS (Slots): The Brain for our program listens to these ─────────────────────────
public slots:
    // From GUI: Login Screen
    void onGuiLoginRequested(const QString &username, const QString &host, int port);

    // From GUI: Chat Screen
    void onGuiMessageSendRequested(const QString &text);
    void onGuiLogoutRequested();

    // From Network: Status updates
    void onNetworkConnected();
    void onNetworkConnectionFailed(const QString &errorMsg);
    void onNetworkDisconnected();

    // From Network: Incoming Data
    void onNetworkIncomingMessage(const QString &sender, const QString &text);
    void onNetworkUserJoined(const QString &username);
    void onNetworkUserLeft(const QString &username);
    void onNetworkUserListReceived(const QStringList &users);

    // ─── OUTPUTS (Signals): The Brain issues these orders, depnding on oour inputs ───────────────────
signals:
    // To GUI: Login Screen Feedback
    void showLoginError(const QString &msg);
    void setLoginConnectingState(bool connecting);

    // To GUI: Navigation
    void navigateToChatScreen(const QString &username, const QString &host, int port);
    void navigateToLoginScreen();

    // To GUI: Chat Screen Feedback
    void appendChatMessage(const QString &sender, const QString &text, bool isOwn);
    void appendSystemMessage(const QString &text);
    void updateNetworkStatus(const QString &status, bool isError);
    void chatScreenAddUser(const QString &username);
    void chatScreenRemoveUser(const QString &username);
    void chatScreenClearUsers();

    // To Network Layer
    void requestNetworkConnect(const QString &username, const QString &host, int port);
    void requestNetworkSendChat(const QString &text);
    void requestNetworkDisconnect();

    // ─── STATE (Memory): The Brain should remember this ───────────────────────────
private:
    QString m_myUsername;
    QString m_host;
    int m_port;
    bool m_isLoggedIn;
    QStringList m_onlineUsers;
};

#endif // APPLOGIC_H

// Mohamed Fadul Mohamed
