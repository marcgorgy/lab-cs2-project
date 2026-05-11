#include "applogic.h"

AppLogic::AppLogic(QObject *parent) : QObject(parent), m_isLoggedIn(false), m_port(0)
{
}

// ─── 1. VALIDATION & ROUTING (GUI → Network) ─────────────────────────────────

void AppLogic::onGuiLoginRequested(const QString &username, const QString &host, int port)
{
    if (username.trimmed().isEmpty()) {
        emit showLoginError("Username cannot be empty.");
        return;
    }
    if (host.trimmed().isEmpty() || port <= 0 || port > 65535) {
        emit showLoginError("Invalid host or port.");
        return;
    }

    m_myUsername = username.trimmed();
    m_host       = host.trimmed();
    m_port       = port;

    emit setLoginConnectingState(true);
    emit requestNetworkConnect(m_myUsername, m_host, m_port);
}

void AppLogic::onGuiMessageSendRequested(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) return;

    // ── Group command: /join <groupname> ────────────────────────────────────
    if (trimmed.startsWith("/join ")) {
        const QString group = trimmed.mid(6).trimmed(); // everything after "/join "
        if (group.isEmpty()) {
            emit appendSystemMessage("Usage: /join <group-name>");
            return;
        }
        m_currentGroup = group;
        emit requestNetworkGroupJoin(group);
        emit chatScreenSetGroup(group);
        emit appendSystemMessage(QString("You joined group: #%1").arg(group));
        return;
    }

    // ── Leave group command: /leave ──────────────────────────────────────────
    if (trimmed == "/leave") {
        if (m_currentGroup.isEmpty()) {
            emit appendSystemMessage("You are not in any group.");
            return;
        }
        emit appendSystemMessage(QString("Left group: #%1  (back to global)").arg(m_currentGroup));
        m_currentGroup.clear();
        emit chatScreenSetGroup("");
        return;
    }

    // ── Regular message ──────────────────────────────────────────────────────
    if (!m_currentGroup.isEmpty()) {
        // Show locally with group tag
        emit appendChatMessage(m_myUsername,
                               QString("[#%1] %2").arg(m_currentGroup, trimmed),
                               true);
        emit requestNetworkGroupMessage(m_currentGroup, trimmed);
    } else {
        emit appendChatMessage(m_myUsername, trimmed, true);
        emit requestNetworkSendChat(trimmed);
    }
}

void AppLogic::onGuiLogoutRequested()
{
    emit requestNetworkDisconnect();
}

// ─── 2. STATE MANAGEMENT & ROUTING (Network → GUI) ───────────────────────────

void AppLogic::onNetworkConnected()
{
    m_isLoggedIn = true;
    emit setLoginConnectingState(false);
    emit navigateToChatScreen(m_myUsername, m_host, m_port);
    emit updateNetworkStatus("Connected", false);
    emit setChatInputEnabled(true);
}

void AppLogic::onNetworkConnectionFailed(const QString &errorMsg)
{
    m_isLoggedIn = false;
    emit setLoginConnectingState(false);
    emit showLoginError(errorMsg);
}

void AppLogic::onNetworkDisconnected()
{
    m_isLoggedIn = false;
    m_onlineUsers.clear();
    m_myUsername.clear();
    m_currentGroup.clear();

    emit chatScreenClearUsers();
    emit navigateToLoginScreen();
    emit setChatInputEnabled(false);
}

// ─── 3. MESSAGE FORMATTING & DATA HANDLING ───────────────────────────────────

void AppLogic::onNetworkIncomingMessage(const QString &sender, const QString &text)
{
    if (sender == m_myUsername) return; // ignore own echo
    emit appendChatMessage(sender, text, false);
}

void AppLogic::onNetworkUserJoined(const QString &username)
{
    if (!m_onlineUsers.contains(username)) {
        m_onlineUsers.append(username);
        emit chatScreenAddUser(username);
        emit appendSystemMessage(username + " joined the chat.");
    }
}

void AppLogic::onNetworkUserLeft(const QString &username)
{
    m_onlineUsers.removeAll(username);
    emit chatScreenRemoveUser(username);
    emit appendSystemMessage(username + " left the chat.");
}

void AppLogic::onNetworkUserListReceived(const QStringList &users)
{
    m_onlineUsers = users;
    emit chatScreenClearUsers();
    for (const QString &user : m_onlineUsers)
        emit chatScreenAddUser(user);
}

// ─── 4. GROUP HANDLING ───────────────────────────────────────────────────────

void AppLogic::onNetworkGroupMessage(const QString &group,
                                     const QString &sender,
                                     const QString &text)
{
    // Ignore our own messages (already shown locally in onGuiMessageSendRequested)
    if (sender == m_myUsername) return;

    // Only render if we're actually in this group; drop messages for other groups
    if (group != m_currentGroup) return;

    emit appendChatMessage(sender, QString("[#%1] %2").arg(group, text), false);
}

void AppLogic::onNetworkGroupJoined(const QString &group, const QString &username)
{
    // System notice when someone else joins the same group we're in
    if (group == m_currentGroup && username != m_myUsername)
        emit appendSystemMessage(username + " joined #" + group);
}

// Mohamed Fadul Mohamed