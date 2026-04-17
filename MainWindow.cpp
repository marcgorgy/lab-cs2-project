#include "MainWindow.h"
#include "LoginScreen.h"
#include "ChatRoomScreen.h"
#include "SettingsScreen.h"

#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stack(new QStackedWidget(this))
    , m_loginScreen(new LoginScreen(this))
    , m_chatScreen(new ChatRoomScreen(this))
    , m_settingsScreen(new SettingsScreen(this))
{
    setWindowTitle("QtChat");
    resize(900, 650);
    setCentralWidget(m_stack);

    m_stack->addWidget(m_loginScreen);   // index 0
    m_stack->addWidget(m_chatScreen);    // index 1
    m_stack->addWidget(m_settingsScreen);// index 2

    // Navigation signals
   /* connect(m_loginScreen, &LoginScreen::loginRequested,
            this, &MainWindow::showChatRoom); */ // Deleted by Mohamed, this defies the business layer

    connect(m_chatScreen, &ChatRoomScreen::settingsRequested,
            this, &MainWindow::showSettings);

    connect(m_chatScreen, &ChatRoomScreen::logoutRequested,
            this, &MainWindow::showLogin);

    connect(m_settingsScreen, &SettingsScreen::settingsSaved,
            this, &MainWindow::showChatRoom);

    connect(m_settingsScreen, &SettingsScreen::cancelled,
            this, [this]() {
                m_stack->setCurrentWidget(m_chatScreen);
            });

    m_stack->setCurrentWidget(m_loginScreen);
}

void MainWindow::showLogin()
{
    m_loginScreen->reset();
    m_stack->setCurrentWidget(m_loginScreen);
}

void MainWindow::showChatRoom(const QString &username, const QString &host, int port)
{
    m_chatScreen->configure(username, host, port);
    m_stack->setCurrentWidget(m_chatScreen);
}

void MainWindow::showSettings(const QString &username, const QString &host, int port)
{
    m_settingsScreen->populate(username, host, port);
    m_stack->setCurrentWidget(m_settingsScreen);
}
