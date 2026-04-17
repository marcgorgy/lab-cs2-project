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

// - Mohamed Fadul Mohamed
