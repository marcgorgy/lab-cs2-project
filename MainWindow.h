#pragma once

#include <QMainWindow>
#include <QStackedWidget>

class LoginScreen;
class ChatRoomScreen;
class SettingsScreen;

/**
 * MainWindow: top-level window that owns a QStackedWidget for
 * screen navigation. Contains NO business logic.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

    LoginScreen* getLoginScreen() const { return m_loginScreen; }
    ChatRoomScreen* getChatRoomScreen() const { return m_chatScreen; }
    SettingsScreen* getSettingsScreen() const { return m_settingsScreen; }

public slots:
    void showLogin();
    void showChatRoom(const QString &username, const QString &host, int port);
    void showSettings(const QString &username, const QString &host, int port);

private:
    QStackedWidget  *m_stack;
    LoginScreen     *m_loginScreen;
    ChatRoomScreen  *m_chatScreen;
    SettingsScreen  *m_settingsScreen;
};
