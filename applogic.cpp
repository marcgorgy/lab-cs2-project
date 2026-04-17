#include "AppLogic.h"
#include <QTimer> // ADDING THIS FOR FAKE DELAYS -- TESTING PHASE ONLY, COMMENT OUT LATER

AppLogic::AppLogic(QObject *parent) : QObject(parent), m_isLoggedIn(false), m_port(0)
{
}

// ─── 1. VALIDATION & ROUTING (GUI -> Network) ─────────────

void AppLogic::onGuiLoginRequested(const QString &username, const QString &host, int port) {
    // Validation
    if (username.trimmed().isEmpty()) {
        emit showLoginError("Username cannot be empty.");
        return;
    }
    if (host.trimmed().isEmpty() || port <= 0 || port > 65535) {
        emit showLoginError("Invalid host or port.");
        return;
    }

    // State Management: Remember the credentials
    m_myUsername = username.trimmed();
    m_host = host.trimmed();
    m_port = port;

    // Tell UI to show the spinning progress bar
    emit setLoginConnectingState(true);

    // Passes clean data to our networking teammate, Ann
    emit requestNetworkConnect(m_myUsername, m_host, m_port);

    /* //  TEMPORARY HACK TO TEST WITHOUT ANN -- FOR NOUR from MOHAMED
    // THis will wait 1 second, then pretend Ann's code successfully connected
    QTimer::singleShot(1000, this, &AppLogic::onNetworkConnected);
    */
}

void AppLogic::onGuiMessageSendRequested(const QString &text) {
    // Validation: don't send empty strings
    if (text.trimmed().isEmpty()) return;

    // Tell UI to display our own message immediately
    emit appendChatMessage(m_myUsername, text.trimmed(), true);

    // Pass text to Ann, where she will wrap it in JSON using QJson objects
    emit requestNetworkSendChat(text.trimmed());

    /* // TEMPORARY HACK TO TEST WITHOUT ANN -- FOR NOUR from MOHAMED
    // Wait half a second, then pretend the server replied
    QTimer::singleShot(500, this, [this, text]() {
        onNetworkIncomingMessage("MockServer", "You just said: " + text);
    });
    */
}

void AppLogic::onGuiLogoutRequested() {
    emit requestNetworkDisconnect();
}

// ─── 2. STATE MANAGEMENT & ROUTING (Network -> GUI) ─────────────

void AppLogic::onNetworkConnected() {
    m_isLoggedIn = true;

    // Stop the loading spinner
    emit setLoginConnectingState(false);

    // Switch the screen to View 2
    emit navigateToChatScreen(m_myUsername, m_host, m_port);
    emit updateNetworkStatus("Connected", false);
}

void AppLogic::onNetworkConnectionFailed(const QString &errorMsg) {
    m_isLoggedIn = false;
    emit setLoginConnectingState(false);
    emit showLoginError(errorMsg);
}

void AppLogic::onNetworkDisconnected() {
    m_isLoggedIn = false;
    m_onlineUsers.clear();
    m_myUsername.clear();

    emit chatScreenClearUsers();
    emit navigateToLoginScreen();
}

// ─── 3. MESSAGE FORMATTING & DATA HANDLING ─────────────

void AppLogic::onNetworkIncomingMessage(const QString &sender, const QString &text) {
    // Ignore if it's our own message bouncing back
    if (sender == m_myUsername) return;

    emit appendChatMessage(sender, text, false);
}

void AppLogic::onNetworkUserJoined(const QString &username) {
    if (!m_onlineUsers.contains(username)) {
        m_onlineUsers.append(username);
        emit chatScreenAddUser(username);
        emit appendSystemMessage(username + " joined the chat.");
    }
}

void AppLogic::onNetworkUserLeft(const QString &username) {
    m_onlineUsers.removeAll(username);
    emit chatScreenRemoveUser(username);
    emit appendSystemMessage(username + " left the chat.");
}

void AppLogic::onNetworkUserListReceived(const QStringList &users) { // Q string list storing and returning current online user
    m_onlineUsers = users;
    emit chatScreenClearUsers();
    for (const QString &user : m_onlineUsers) {
        emit chatScreenAddUser(user);
    }
}

// - Mohamed Fadul Mohamed
