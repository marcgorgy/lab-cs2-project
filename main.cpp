#include <thread>
#include <chrono>
#include <QApplication>
#include "MainWindow.h"
#include "applogic.h"
#include "NetworkManager.h"
#include "LoginScreen.h"
#include "ChatRoomScreen.h"

extern void runChatServer();

int main(int argc, char *argv[])
{
    // Start the embedded chat server in a background thread.
    std::thread serverThread(runChatServer);
    serverThread.detach();

    // Give the server's acceptor time to bind before the UI tries to connect.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    QApplication app(argc, argv);
    app.setApplicationName("QtChat");
    app.setOrganizationName("Lab");

    // ── Core components ───────────────────────────────────────────────────────
    MainWindow     w;
    AppLogic       appLogic;
    NetworkManager networkManager;

    LoginScreen    *loginScreen = w.getLoginScreen();
    ChatRoomScreen *chatScreen  = w.getChatRoomScreen();

    // ── 1. Navigation (AppLogic → MainWindow) ─────────────────────────────────
    QObject::connect(&appLogic, &AppLogic::navigateToChatScreen,
                     &w,        &MainWindow::showChatRoom);
    QObject::connect(&appLogic, &AppLogic::navigateToLoginScreen,
                     &w,        &MainWindow::showLogin);

    // ── 2. Login Screen (GUI ↔ AppLogic) ──────────────────────────────────────
    QObject::connect(loginScreen, &LoginScreen::loginRequested,
                     &appLogic,   &AppLogic::onGuiLoginRequested);
    QObject::connect(&appLogic,   &AppLogic::showLoginError,
                     loginScreen, &LoginScreen::setErrorMessage);
    QObject::connect(&appLogic,   &AppLogic::setLoginConnectingState,
                     loginScreen, &LoginScreen::setConnecting);

    // ── 3. Chat Screen (GUI ↔ AppLogic) ──────────────────────────────────────
    QObject::connect(chatScreen, &ChatRoomScreen::messageSendRequested,
                     &appLogic,  &AppLogic::onGuiMessageSendRequested);
    QObject::connect(chatScreen, &ChatRoomScreen::logoutRequested,
                     &appLogic,  &AppLogic::onGuiLogoutRequested);

    QObject::connect(&appLogic,  &AppLogic::appendChatMessage,
                     chatScreen, &ChatRoomScreen::appendMessage);
    QObject::connect(&appLogic,  &AppLogic::appendSystemMessage,
                     chatScreen, &ChatRoomScreen::appendSystemMessage);
    QObject::connect(&appLogic,  &AppLogic::updateNetworkStatus,
                     chatScreen, &ChatRoomScreen::setNetworkStatus);
    QObject::connect(&appLogic,  &AppLogic::chatScreenAddUser,
                     chatScreen, &ChatRoomScreen::addUser);
    QObject::connect(&appLogic,  &AppLogic::chatScreenRemoveUser,
                     chatScreen, &ChatRoomScreen::removeUser);
    QObject::connect(&appLogic,  &AppLogic::chatScreenClearUsers,
                     chatScreen, &ChatRoomScreen::clearUsers);
    QObject::connect(&appLogic,  &AppLogic::setChatInputEnabled,
                     chatScreen, &ChatRoomScreen::setConnected);

    // Group badge in top bar
    QObject::connect(&appLogic,  &AppLogic::chatScreenSetGroup,
                     chatScreen, &ChatRoomScreen::setCurrentGroup);

    // ── 4. AppLogic → NetworkManager (outgoing requests) ──────────────────────
    QObject::connect(&appLogic,        &AppLogic::requestNetworkConnect,
                     &networkManager,  &NetworkManager::connectToServer);
    QObject::connect(&appLogic,        &AppLogic::requestNetworkSendChat,
                     &networkManager,  &NetworkManager::sendChatMessage);
    QObject::connect(&appLogic,        &AppLogic::requestNetworkDisconnect,
                     &networkManager,  &NetworkManager::disconnectFromServer);
    QObject::connect(&appLogic,        &AppLogic::requestNetworkGroupJoin,
                     &networkManager,  &NetworkManager::sendGroupJoin);
    QObject::connect(&appLogic,        &AppLogic::requestNetworkGroupMessage,
                     &networkManager,  &NetworkManager::sendGroupMessage);

    // ── 5. NetworkManager → AppLogic (incoming server events) ─────────────────
    QObject::connect(&networkManager, &NetworkManager::connected,
                     &appLogic,       &AppLogic::onNetworkConnected);
    QObject::connect(&networkManager, &NetworkManager::connectionFailed,
                     &appLogic,       &AppLogic::onNetworkConnectionFailed);
    QObject::connect(&networkManager, &NetworkManager::disconnected,
                     &appLogic,       &AppLogic::onNetworkDisconnected);
    QObject::connect(&networkManager, &NetworkManager::incomingMessage,
                     &appLogic,       &AppLogic::onNetworkIncomingMessage);
    QObject::connect(&networkManager, &NetworkManager::userJoined,
                     &appLogic,       &AppLogic::onNetworkUserJoined);
    QObject::connect(&networkManager, &NetworkManager::userLeft,
                     &appLogic,       &AppLogic::onNetworkUserLeft);
    QObject::connect(&networkManager, &NetworkManager::userListReceived,
                     &appLogic,       &AppLogic::onNetworkUserListReceived);
    QObject::connect(&networkManager, &NetworkManager::groupMessage,
                     &appLogic,       &AppLogic::onNetworkGroupMessage);
    QObject::connect(&networkManager, &NetworkManager::groupJoined,
                     &appLogic,       &AppLogic::onNetworkGroupJoined);

    // ── Start ─────────────────────────────────────────────────────────────────
    w.show();
    return app.exec();
}