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
            this, &NetworkManager::onReadyRead);

    connect(socket, &QTcpSocket::errorOccurred,
            this, &NetworkManager::onSocketError);

    connect(socket, &QTcpSocket::connected,
            this, &NetworkManager::connected);

    connect(socket, &QTcpSocket::disconnected,
            this, &NetworkManager::disconnected);
}

// ─────────────────────────────────────────────

void NetworkManager::connectToServer(const QString &username,
                                     const QString &host,
                                     int port)
{
    m_username = username;
    socket->connectToHost(host, port);
}

// ─────────────────────────────────────────────

void NetworkManager::sendChatMessage(const QString &text)
{
    QJsonObject obj;
    obj["type"] = "chat";
    obj["sender"] = m_username;
    obj["payload"] = text;

    sendJson(obj);
}

// ─────────────────────────────────────────────

void NetworkManager::disconnectFromServer()
{
    m_buffer.clear();
    socket->disconnectFromHost();
}

// ─────────────────────────────────────────────

void NetworkManager::sendJson(const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    socket->write(data + "\n");
}

// ─────────────────────────────────────────────



void NetworkManager::onReadyRead()
{
    // Append whatever arrived to our persistent buffer.
    // readAll() may contain 0, 1, or many complete JSON lines.
    m_buffer += socket->readAll();

    // Process every complete line (terminated by '\n') that is in the buffer.
    // Anything left over (a partial line) stays in m_buffer for the next signal.
    int newlinePos;
    while ((newlinePos = m_buffer.indexOf('\n')) != -1) {
        QByteArray line = m_buffer.left(newlinePos).trimmed();
        m_buffer.remove(0, newlinePos + 1); // consume this line + the '\n'

        if (line.isEmpty()) continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) continue;

        processJson(doc.object());
    }
}

// ─────────────────────────────────────────────

void NetworkManager::processJson(const QJsonObject &obj)
{
    QString type = obj["type"].toString();

    if (type == "chat") {
        emit incomingMessage(
            obj["sender"].toString(),
            obj["payload"].toString()
        );
    }
    else if (type == "user_joined") {
        emit userJoined(obj["sender"].toString());
    }
    else if (type == "user_left") {
        emit userLeft(obj["sender"].toString());
    }
    else if (type == "user_list") {
        QStringList users;
        for (auto v : obj["users"].toArray())
            users << v.toString();

        emit userListReceived(users);
    }
}

// ─────────────────────────────────────────────

void NetworkManager::onSocketError()
{
    emit connectionFailed(socket->errorString());
}
