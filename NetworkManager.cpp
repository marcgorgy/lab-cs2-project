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
 
    // FIX (Bug #2): Wire connected → onConnected (private slot) instead of
    // directly re-emitting connected. onConnected sends the login JSON first,
    // THEN emits the public connected() signal so AppLogic navigates only
    // after the server has received credentials.
    connect(socket, &QTcpSocket::connected,
            this, &NetworkManager::onConnected);
 
    connect(socket, &QTcpSocket::disconnected,
            this, &NetworkManager::disconnected);
}
 
// ─────────────────────────────────────────────
 
void NetworkManager::connectToServer(const QString &username,
                                     const QString &host,
                                     int port)
{
    m_username = username;
 
    // FIX (Bug #3): If the socket is in any non-idle state from a previous
    // login attempt, abort it first. Calling connectToHost() on a socket that
    // is already Connected/Closing immediately triggers errorOccurred, which
    // fires connectionFailed → "Connection Failed" before the new handshake
    // even starts. abort() is synchronous and emits no signals.
    if (socket->state() != QAbstractSocket::UnconnectedState) {
        socket->abort();
    }
 
    // connectToHost() is non-blocking. The socket is NOT connected when this
    // returns. The login JSON is sent in onConnected() once the TCP handshake
    // completes (QTcpSocket::connected signal). Sending it here would write
    // to a not-yet-open socket and silently drop the data.
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
 
 
 
// FIX (Bug #2): This slot fires after the TCP handshake completes.
// Only now is the socket writable. We send the login packet so the server
// registers our username, then emit the public connected() signal so
// AppLogic transitions the UI to the chat screen.
void NetworkManager::onConnected()
{
    QJsonObject loginMsg;
    loginMsg["type"]     = "login";
    loginMsg["username"] = m_username;
    sendJson(loginMsg); // Socket is confirmed open — safe to write
 
    emit connected(); // Tell AppLogic: handshake done + server notified

} 
// ─────────────────────────────────────────────
void NetworkManager::onReadyRead(){ 
 
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
 
    // FIX: Server broadcasts type "incoming_message" (not "chat") and puts
    // the message body under "text" (not "payload"). Aligning both here.
    if (type == "incoming_message") {
        emit incomingMessage(
            obj["sender"].toString(),
            obj["text"].toString()      // was obj["payload"] — server sends "text"
        );
    }
    // FIX: Server broadcasts {"type":"user_joined","username":...}
    // The field is "username", not "sender".
    else if (type == "user_joined") {
        emit userJoined(obj["username"].toString()); // was obj["sender"]
    }
    else if (type == "user_left") {
        emit userLeft(obj["username"].toString());   // was obj["sender"]
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