#include <QApplication>
#include "MainWindow.h"
#include "AppLogic.h"

// IMPORTANT for ANN: Replace "NetworkManager.h" with whatever you
// actually name your network class file
// #include "NetworkManager.h" -- assuming this is what you will call it

#include "LoginScreen.h"
#include "ChatRoomScreen.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("QtChat");
    app.setOrganizationName("Lab");

    // 1. Create all the pieces
    MainWindow w;
    AppLogic appLogic;

    // IMPORTANT for ANN -> uncomment the line below once you push your code
    // NetworkManager networkManager;

    // Get pointers to the screens inside MainWindow
    // (Ensure you added these getter methods to MainWindow.h!)
    LoginScreen* loginScreen = w.getLoginScreen();
    ChatRoomScreen* chatScreen = w.getChatRoomScreen();

    // ─── 2. NEW: Wire YOUR Brain to control MainWindow navigation ────
    QObject::connect(&appLogic, &AppLogic::navigateToChatScreen, &w, &MainWindow::showChatRoom);
    QObject::connect(&appLogic, &AppLogic::navigateToLoginScreen, &w, &MainWindow::showLogin);


    // ─── 3. Wire GUI Inputs (Signals) -> Brain (Slots) ───────────────
    if (loginScreen) {
        QObject::connect(loginScreen, &LoginScreen::loginRequested, &appLogic, &AppLogic::onGuiLoginRequested);

        // Brain Outputs -> Login Screen Feedback
        QObject::connect(&appLogic, &AppLogic::showLoginError, loginScreen, &LoginScreen::setErrorMessage);
        QObject::connect(&appLogic, &AppLogic::setLoginConnectingState, loginScreen, &LoginScreen::setConnecting);
    }

    if (chatScreen) {
        QObject::connect(chatScreen, &ChatRoomScreen::messageSendRequested, &appLogic, &AppLogic::onGuiMessageSendRequested);
        QObject::connect(chatScreen, &ChatRoomScreen::logoutRequested, &appLogic, &AppLogic::onGuiLogoutRequested);

        // Brain Outputs -> Chat Screen Feedback
        QObject::connect(&appLogic, &AppLogic::appendChatMessage, chatScreen, &ChatRoomScreen::appendMessage);
        QObject::connect(&appLogic, &AppLogic::appendSystemMessage, chatScreen, &ChatRoomScreen::appendSystemMessage);
        QObject::connect(&appLogic, &AppLogic::updateNetworkStatus, chatScreen, &ChatRoomScreen::setNetworkStatus);
        QObject::connect(&appLogic, &AppLogic::chatScreenAddUser, chatScreen, &ChatRoomScreen::addUser);
        QObject::connect(&appLogic, &AppLogic::chatScreenRemoveUser, chatScreen, &ChatRoomScreen::removeUser);
        QObject::connect(&appLogic, &AppLogic::chatScreenClearUsers, chatScreen, &ChatRoomScreen::clearUsers);
    }

    // ─── 4. Wire Brain <---> Network (Currently for Ann) ──────────────
    /* // Wire Brain Outputs -> Network (Slots)
    QObject::connect(&appLogic, &AppLogic::requestNetworkConnect, &networkManager, &NetworkManager::connectToServer);
    QObject::connect(&appLogic, &AppLogic::requestNetworkSendChat, &networkManager, &NetworkManager::sendChatMessage);
    QObject::connect(&appLogic, &AppLogic::requestNetworkDisconnect, &networkManager, &NetworkManager::disconnectFromServer);

    // Wire Network (Signals) -> Brain (Slots)
    QObject::connect(&networkManager, &NetworkManager::connected, &appLogic, &AppLogic::onNetworkConnected);
    QObject::connect(&networkManager, &NetworkManager::connectionFailed, &appLogic, &AppLogic::onNetworkConnectionFailed);
    QObject::connect(&networkManager, &NetworkManager::disconnected, &appLogic, &AppLogic::onNetworkDisconnected);
    QObject::connect(&networkManager, &NetworkManager::incomingMessage, &appLogic, &AppLogic::onNetworkIncomingMessage);
    QObject::connect(&networkManager, &NetworkManager::userJoined, &appLogic, &AppLogic::onNetworkUserJoined);
    QObject::connect(&networkManager, &NetworkManager::userLeft, &appLogic, &AppLogic::onNetworkUserLeft);
    QObject::connect(&networkManager, &NetworkManager::userListReceived, &appLogic, &AppLogic::onNetworkUserListReceived);
    */

    w.show();
    return app.exec();
}
