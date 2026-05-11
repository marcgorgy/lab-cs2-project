#include "NetworkManager.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent),
      socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::readyRead,
            this,   &NetworkManager::onReadyRead);

    connect(socket, &QTcpSocket::errorOccurred,
            this,   &NetworkManager::onSocketError);

    // onConnected sends the login packet THEN emits connected(),
    // so AppLogic never navigates before the server knows our username.
    connect(socket, &QTcpSocket::connected,
            this,   &NetworkManager::onConnected);

    connect(socket, &QTcpSocket::disconnected,
            this,   &NetworkManager::disconnected);
}

// ─────────────────────────────────────────────────────────────────────────────

void NetworkManager::connectToServer(const QString &username,
                                     const QString &host,
                                     int port)
{
    m_username = username;

    // Abort any leftover connection before starting a new one.
    if (socket->state() != QAbstractSocket::UnconnectedState)
        socket->abort();

    socket->connectToHost(host, port);
}

// ─────────────────────────────────────────────────────────────────────────────

void NetworkManager::sendChatMessage(const QString &text)
{
    QJsonObject obj;
    obj["type"]   = "chat";
    obj["sender"] = m_username;
    obj["text"]   = text;           // server reads "text", not "payload"
    sendJson(obj);
}

// ─────────────────────────────────────────────────────────────────────────────

void NetworkManager::sendGroupJoin(const QString &group)
{
    QJsonObject obj;
    obj["type"]     = "group_join";
    obj["username"] = m_username;
    obj["group"]    = group;
    sendJson(obj);
}

void NetworkManager::sendGroupMessage(const QString &group, const QString &text)
{
    QJsonObject obj;
    obj["type"]   = "group_message";
    obj["sender"] = m_username;
    obj["group"]  = group;
    obj["text"]   = text;
    sendJson(obj);
}

// ─────────────────────────────────────────────────────────────────────────────

void NetworkManager::disconnectFromServer()
{
    m_buffer.clear();
    socket->disconnectFromHost();
}

// ─────────────────────────────────────────────────────────────────────────────

void NetworkManager::sendJson(const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    socket->write(data + "\n");
}

// ─────────────────────────────────────────────────────────────────────────────

void NetworkManager::onConnected()
{
    QJsonObject loginMsg;
    loginMsg["type"]     = "login";
    loginMsg["username"] = m_username;
    sendJson(loginMsg);

    emit connected();
}

// ─────────────────────────────────────────────────────────────────────────────

void NetworkManager::onReadyRead()
{
    m_buffer += socket->readAll();

    int newlinePos;
    while ((newlinePos = m_buffer.indexOf('\n')) != -1) {
        QByteArray line = m_buffer.left(newlinePos).trimmed();
        m_buffer.remove(0, newlinePos + 1);

        if (line.isEmpty()) continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) continue;

        processJson(doc.object());
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void NetworkManager::processJson(const QJsonObject &obj)
{
    const QString type = obj["type"].toString();

    if (type == "incoming_message") {
        emit incomingMessage(obj["sender"].toString(),
                             obj["text"].toString());
    }
    else if (type == "user_joined") {
        emit userJoined(obj["username"].toString());
    }
    else if (type == "user_left") {
        emit userLeft(obj["username"].toString());
    }
    else if (type == "user_list") {
        QStringList users;
        for (auto v : obj["users"].toArray())
            users << v.toString();
        emit userListReceived(users);
    }
    // ── Group events ─────────────────────────────────────────────────────────
    else if (type == "group_message") {
        emit groupMessage(obj["group"].toString(),
                          obj["sender"].toString(),
                          obj["text"].toString());
    }
    else if (type == "group_joined") {
        emit groupJoined(obj["group"].toString(),
                         obj["username"].toString());
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void NetworkManager::onSocketError()
{
    emit connectionFailed(socket->errorString());
}