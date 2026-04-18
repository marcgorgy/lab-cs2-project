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

signals:
    void connected();
    void connectionFailed(const QString &error);
    void disconnected();

    // parsed from server JSON
    void incomingMessage(const QString &sender, const QString &text);
    void userJoined(const QString &username);
    void userLeft(const QString &username);
    void userListReceived(const QStringList &users);

private slots:
    void onReadyRead();
    void onSocketError();

private:
    void sendJson(const QJsonObject &obj);
    void processJson(const QJsonObject &obj);

    QTcpSocket *socket;
    QString m_username;
};