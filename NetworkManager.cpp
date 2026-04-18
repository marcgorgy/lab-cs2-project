#include "NetworkManager.h"

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
    QByteArray data = socket->readAll();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if (err.error != QJsonParseError::NoError)
        return;

    if (!doc.isObject())
        return;

    processJson(doc.object());
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