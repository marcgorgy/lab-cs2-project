#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);

public slots:
    void connectToServer(const QString &username, const QString &host, int port);
    void sendChatMessage(const QString &text);
    void disconnectFromServer();

    // Group support
    void sendGroupJoin(const QString &group);
    void sendGroupMessage(const QString &group, const QString &text);

signals:
    void connected();
    void connectionFailed(const QString &error);
    void disconnected();

    // Parsed from server JSON
    void incomingMessage(const QString &sender, const QString &text);
    void userJoined(const QString &username);
    void userLeft(const QString &username);
    void userListReceived(const QStringList &users);

    // Group events
    void groupMessage(const QString &group, const QString &sender, const QString &text);
    void groupJoined(const QString &group, const QString &username);

private slots:
    void onConnected();
    void onReadyRead();
    void onSocketError();

private:
    void sendJson(const QJsonObject &obj);
    void processJson(const QJsonObject &obj);

    QByteArray  m_buffer;
    QTcpSocket *socket;
    QString     m_username;
};