#include "AppLogic.h"

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
}

void AppLogic::onGuiMessageSendRequested(const QString &text) {
    // Validation: don't send empty strings
    if (text.trimmed().isEmpty()) return;

    // Tell UI to display our own message immediately (optional, or wait for server bounce)
    emit appendChatMessage(m_myUsername, text.trimmed(), true);

    // Pass text to Ann, where she will wrap it in JSON using QJson objects
    emit requestNetworkSendChat(text.trimmed());
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

// - Mohamed Fadul Mohamed
