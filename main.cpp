#include <QApplication>
#include "MainWindow.h"
#include "AppLogic.h"
#include "NetworkManager.h"

#include "LoginScreen.h"
#include "ChatRoomScreen.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("QtChat");
    app.setOrganizationName("Lab");

    // ─────────────────────────────────────────
    // Core components
    // ─────────────────────────────────────────
    MainWindow w;
    AppLogic appLogic;
    NetworkManager networkManager;

    // Screens
    LoginScreen* loginScreen = w.getLoginScreen();
    ChatRoomScreen* chatScreen = w.getChatRoomScreen();

    // ─────────────────────────────────────────
    // 1. Navigation (AppLogic → MainWindow)
    // ─────────────────────────────────────────
    QObject::connect(&appLogic, &AppLogic::navigateToChatScreen, &w, &MainWindow::showChatRoom);

    QObject::connect(&appLogic, &AppLogic::navigateToLoginScreen, &w, &MainWindow::showLogin);

    // ─────────────────────────────────────────
    // 2. GUI → AppLogic (Login Screen)
    // ─────────────────────────────────────────
    QObject::connect(loginScreen, &LoginScreen::loginRequested, &appLogic, &AppLogic::onGuiLoginRequested);

    QObject::connect(&appLogic, &AppLogic::showLoginError, loginScreen, &LoginScreen::setErrorMessage);

    QObject::connect(&appLogic, &AppLogic::setLoginConnectingState, loginScreen, &LoginScreen::setConnecting);

    // ─────────────────────────────────────────
    // 3. GUI → AppLogic (Chat Screen)
    // ─────────────────────────────────────────
    QObject::connect(chatScreen, &ChatRoomScreen::messageSendRequested, &appLogic, &AppLogic::onGuiMessageSendRequested);

    QObject::connect(chatScreen, &ChatRoomScreen::logoutRequested, &appLogic, &AppLogic::onGuiLogoutRequested);

    QObject::connect(&appLogic, &AppLogic::appendChatMessage, chatScreen, &ChatRoomScreen::appendMessage);

    QObject::connect(&appLogic, &AppLogic::appendSystemMessage, chatScreen, &ChatRoomScreen::appendSystemMessage);

    QObject::connect(&appLogic, &AppLogic::updateNetworkStatus, chatScreen, &ChatRoomScreen::setNetworkStatus);

    QObject::connect(&appLogic, &AppLogic::chatScreenAddUser, chatScreen, &ChatRoomScreen::addUser);

    QObject::connect(&appLogic, &AppLogic::chatScreenRemoveUser, chatScreen, &ChatRoomScreen::removeUser);

    QObject::connect(&appLogic, &AppLogic::chatScreenClearUsers, chatScreen, &ChatRoomScreen::clearUsers);

    // ─────────────────────────────────────────
    // 4. AppLogic → NetworkManager
    // (Outgoing requests)
    // ─────────────────────────────────────────
    QObject::connect(&appLogic, &AppLogic::requestNetworkConnect, &networkManager, &NetworkManager::connectToServer);

    QObject::connect(&appLogic, &AppLogic::requestNetworkSendChat, &networkManager, &NetworkManager::sendChatMessage);

    QObject::connect(&appLogic, &AppLogic::requestNetworkDisconnect, &networkManager, &NetworkManager::disconnectFromServer);

    // ─────────────────────────────────────────
    // 5. NetworkManager → AppLogic
    // (Incoming server events)
    // ─────────────────────────────────────────
    QObject::connect(&networkManager, &NetworkManager::connected, &appLogic, &AppLogic::onNetworkConnected);

    QObject::connect(&networkManager, &NetworkManager::connectionFailed, &appLogic, &AppLogic::onNetworkConnectionFailed);

    QObject::connect(&networkManager, &NetworkManager::disconnected, &appLogic, &AppLogic::onNetworkDisconnected);

    QObject::connect(&networkManager, &NetworkManager::incomingMessage, &appLogic, &AppLogic::onNetworkIncomingMessage);

    QObject::connect(&networkManager, &NetworkManager::userJoined, &appLogic, &AppLogic::onNetworkUserJoined);

    QObject::connect(&networkManager, &NetworkManager::userLeft, &appLogic, &AppLogic::onNetworkUserLeft);

    QObject::connect(&networkManager, &NetworkManager::userListReceived, &appLogic, &AppLogic::onNetworkUserListReceived);

    // ─────────────────────────────────────────
    // Start UI
    // ─────────────────────────────────────────
    w.show();

    return app.exec();
}