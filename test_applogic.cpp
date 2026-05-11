// test_applogic.cpp
// Unit tests for AppLogic (Client Business Logic)
// Uses GoogleTest + GoogleMock to mock the network layer entirely.
// Run fully offline – no real socket is ever opened.
//
// Build (example, adjust paths to match your CMake setup):
//   g++ -std=c++17 test_applogic.cpp applogic.cpp \
//       $(pkg-config --cflags --libs Qt5Core) \
//       -lgtest -lgmock -lgtest_main -pthread -o test_applogic
//
// Or add to CMakeLists.txt:
//   find_package(GTest REQUIRED)
//   add_executable(test_applogic test_applogic.cpp applogic.cpp)
//   target_link_libraries(test_applogic PRIVATE GTest::gtest GTest::gmock
//                         GTest::gtest_main Qt5::Core)
//   add_test(NAME AppLogicTests COMMAND test_applogic)
 
#include <gtest/gtest.h>
#include <gmock/gmock.h>
 
#include <QCoreApplication>
#include <QSignalSpy>
#include <QString>
#include <QStringList>
 
#include "applogic.h"
 
// ─────────────────────────────────────────────────────────────────────────────
// Minimal QApplication bootstrap (required for Qt signal/slot machinery)
// ─────────────────────────────────────────────────────────────────────────────
int    g_argc = 0;
char **g_argv = nullptr;
 
// ─────────────────────────────────────────────────────────────────────────────
// MockNetworkSink
// Represents the NetworkManager's interface as seen by AppLogic.
// We connect AppLogic's outgoing signals to these mock methods so we can
// VERIFY that AppLogic tells the network layer the right things.
// ─────────────────────────────────────────────────────────────────────────────
class MockNetworkSink : public QObject
{
    Q_OBJECT
public:
    // GoogleMock methods – called via Qt connections
    MOCK_METHOD(void, connectToServer,
                (const QString &username, const QString &host, int port));
    MOCK_METHOD(void, sendChatMessage, (const QString &text));
    MOCK_METHOD(void, disconnectFromServer, ());
 
public slots:
    // These slots bridge AppLogic signals → GoogleMock expectations
    void onConnectRequested(const QString &u, const QString &h, int p)
        { connectToServer(u, h, p); }
    void onSendRequested(const QString &t)
        { sendChatMessage(t); }
    void onDisconnectRequested()
        { disconnectFromServer(); }
};
 
// ─────────────────────────────────────────────────────────────────────────────
// Test Fixture
// ─────────────────────────────────────────────────────────────────────────────
class AppLogicTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        logic = new AppLogic();
        net   = new MockNetworkSink();
 
        // Wire AppLogic → MockNetworkSink
        QObject::connect(logic, &AppLogic::requestNetworkConnect,
                         net,   &MockNetworkSink::onConnectRequested);
        QObject::connect(logic, &AppLogic::requestNetworkSendChat,
                         net,   &MockNetworkSink::onSendRequested);
        QObject::connect(logic, &AppLogic::requestNetworkDisconnect,
                         net,   &MockNetworkSink::onDisconnectRequested);
    }
 
    void TearDown() override
    {
        delete logic;
        delete net;
    }
 
    AppLogic        *logic = nullptr;
    MockNetworkSink *net   = nullptr;
};
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 1 – Empty username is rejected before reaching the network
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, LoginRejectsEmptyUsername)
{
    // The network layer must NEVER be called when validation fails
    EXPECT_CALL(*net, connectToServer(::testing::_, ::testing::_, ::testing::_))
        .Times(0);
 
    QSignalSpy errorSpy(logic, &AppLogic::showLoginError);
    QSignalSpy connectingSpy(logic, &AppLogic::setLoginConnectingState);
 
    logic->onGuiLoginRequested("", "127.0.0.1", 12345);
 
    // showLoginError must fire exactly once
    ASSERT_EQ(errorSpy.count(), 1);
    EXPECT_FALSE(errorSpy.first().first().toString().isEmpty());
 
    // setLoginConnectingState(true) must NOT have been emitted
    // (the spinner should never appear for an invalid request)
    for (auto &args : connectingSpy)
        EXPECT_FALSE(args.first().toBool())
            << "Connecting spinner should never be shown for invalid input";
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 2 – Invalid port is rejected before reaching the network
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, LoginRejectsInvalidPort)
{
    EXPECT_CALL(*net, connectToServer(::testing::_, ::testing::_, ::testing::_))
        .Times(0);
 
    QSignalSpy errorSpy(logic, &AppLogic::showLoginError);
 
    // Port 0 is invalid (valid range: 1–65535)
    logic->onGuiLoginRequested("alice", "127.0.0.1", 0);
    EXPECT_EQ(errorSpy.count(), 1);
 
    // Port > 65535 is invalid
    logic->onGuiLoginRequested("alice", "127.0.0.1", 99999);
    EXPECT_EQ(errorSpy.count(), 2);
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 3 – Valid login forwards credentials to the network layer exactly once
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, ValidLoginForwardsToNetwork)
{
    EXPECT_CALL(*net, connectToServer(QString("alice"), QString("127.0.0.1"), 12345))
        .Times(1);
 
    QSignalSpy spinningSpy(logic, &AppLogic::setLoginConnectingState);
    QSignalSpy errorSpy(logic,    &AppLogic::showLoginError);
 
    logic->onGuiLoginRequested("alice", "127.0.0.1", 12345);
 
    // No validation error
    EXPECT_EQ(errorSpy.count(), 0);
 
    // Spinner must have been activated (setLoginConnectingState(true))
    ASSERT_GE(spinningSpy.count(), 1);
    EXPECT_TRUE(spinningSpy.first().first().toBool());
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 4 – Successful network connection transitions UI to chat screen
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, NetworkConnectedNavigatesToChatScreen)
{
    // First perform a valid login so AppLogic stores the credentials
    EXPECT_CALL(*net, connectToServer(::testing::_, ::testing::_, ::testing::_))
        .Times(1);
    logic->onGuiLoginRequested("alice", "127.0.0.1", 12345);
 
    QSignalSpy navSpy(logic,         &AppLogic::navigateToChatScreen);
    QSignalSpy spinningSpy(logic,    &AppLogic::setLoginConnectingState);
    QSignalSpy inputSpy(logic,       &AppLogic::setChatInputEnabled);
    QSignalSpy statusSpy(logic,      &AppLogic::updateNetworkStatus);
 
    // Simulate the network confirming a successful connection
    logic->onNetworkConnected();
 
    // Must navigate to chat screen
    ASSERT_EQ(navSpy.count(), 1);
    // The username passed must match what was logged in with
    EXPECT_EQ(navSpy.first().at(0).toString(), QString("alice"));
 
    // Spinner must be turned off
    bool spinnerOff = false;
    for (auto &args : spinningSpy)
        if (!args.first().toBool()) spinnerOff = true;
    EXPECT_TRUE(spinnerOff) << "Connecting spinner must be hidden after connection";
 
    // Chat input must be unlocked
    ASSERT_GE(inputSpy.count(), 1);
    EXPECT_TRUE(inputSpy.last().first().toBool());
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 5 – Connection failure shows error and does NOT navigate
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, NetworkConnectionFailedShowsErrorAndStaysOnLogin)
{
    EXPECT_CALL(*net, connectToServer(::testing::_, ::testing::_, ::testing::_))
        .Times(1);
    logic->onGuiLoginRequested("alice", "127.0.0.1", 12345);
 
    QSignalSpy navSpy(logic,   &AppLogic::navigateToChatScreen);
    QSignalSpy errorSpy(logic, &AppLogic::showLoginError);
 
    logic->onNetworkConnectionFailed("Connection refused");
 
    EXPECT_EQ(navSpy.count(), 0)  << "Must not navigate on failure";
    ASSERT_EQ(errorSpy.count(), 1);
    EXPECT_FALSE(errorSpy.first().first().toString().isEmpty());
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 6 – Empty message is silently dropped (not forwarded to network)
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, EmptyMessageIsDropped)
{
    EXPECT_CALL(*net, sendChatMessage(::testing::_)).Times(0);
 
    QSignalSpy chatSpy(logic, &AppLogic::appendChatMessage);
 
    logic->onGuiMessageSendRequested("");
    logic->onGuiMessageSendRequested("   "); // whitespace-only
 
    EXPECT_EQ(chatSpy.count(), 0);
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 7 – Valid message is forwarded to network AND shown locally as "own"
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, ValidMessageForwardedAndShownLocally)
{
    // Login first so m_myUsername is populated
    EXPECT_CALL(*net, connectToServer(::testing::_, ::testing::_, ::testing::_))
        .Times(1);
    logic->onGuiLoginRequested("alice", "127.0.0.1", 12345);
    logic->onNetworkConnected();
 
    EXPECT_CALL(*net, sendChatMessage(QString("Hello world"))).Times(1);
 
    QSignalSpy chatSpy(logic, &AppLogic::appendChatMessage);
 
    logic->onGuiMessageSendRequested("Hello world");
 
    ASSERT_EQ(chatSpy.count(), 1);
    auto args = chatSpy.first();
    EXPECT_EQ(args.at(0).toString(), QString("alice")); // sender = own username
    EXPECT_EQ(args.at(1).toString(), QString("Hello world"));
    EXPECT_TRUE(args.at(2).toBool());                   // isOwn = true
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 8 – Incoming message from OTHER user is displayed
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, IncomingMessageFromOtherUserIsDisplayed)
{
    EXPECT_CALL(*net, connectToServer(::testing::_, ::testing::_, ::testing::_))
        .Times(1);
    logic->onGuiLoginRequested("alice", "127.0.0.1", 12345);
    logic->onNetworkConnected();
 
    QSignalSpy chatSpy(logic, &AppLogic::appendChatMessage);
 
    logic->onNetworkIncomingMessage("bob", "Hey Alice!");
 
    ASSERT_EQ(chatSpy.count(), 1);
    auto args = chatSpy.first();
    EXPECT_EQ(args.at(0).toString(), QString("bob"));
    EXPECT_EQ(args.at(1).toString(), QString("Hey Alice!"));
    EXPECT_FALSE(args.at(2).toBool()); // isOwn = false
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 9 – Own message echoed back by server is IGNORED (no duplicate display)
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, OwnMessageEchoFromServerIsIgnored)
{
    EXPECT_CALL(*net, connectToServer(::testing::_, ::testing::_, ::testing::_))
        .Times(1);
    logic->onGuiLoginRequested("alice", "127.0.0.1", 12345);
    logic->onNetworkConnected();
 
    QSignalSpy chatSpy(logic, &AppLogic::appendChatMessage);
 
    // Server echoes the message back with sender == our own username
    logic->onNetworkIncomingMessage("alice", "Hello world");
 
    EXPECT_EQ(chatSpy.count(), 0) << "Echo of own message must be silently ignored";
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 10 – User join adds to list and emits system message
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, UserJoinedAddsUserAndShowsSystemMessage)
{
    QSignalSpy addSpy(logic,    &AppLogic::chatScreenAddUser);
    QSignalSpy sysSpy(logic,    &AppLogic::appendSystemMessage);
 
    logic->onNetworkUserJoined("bob");
 
    ASSERT_EQ(addSpy.count(), 1);
    EXPECT_EQ(addSpy.first().first().toString(), QString("bob"));
 
    ASSERT_EQ(sysSpy.count(), 1);
    EXPECT_TRUE(sysSpy.first().first().toString().contains("bob"));
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 11 – Duplicate user_joined events don't add the same user twice
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, DuplicateUserJoinedIsIgnored)
{
    QSignalSpy addSpy(logic, &AppLogic::chatScreenAddUser);
 
    logic->onNetworkUserJoined("bob");
    logic->onNetworkUserJoined("bob"); // duplicate
 
    EXPECT_EQ(addSpy.count(), 1) << "Duplicate join must not add user twice";
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 12 – User leave removes from list and emits system message
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, UserLeftRemovesUserAndShowsSystemMessage)
{
    logic->onNetworkUserJoined("bob");
 
    QSignalSpy removeSpy(logic, &AppLogic::chatScreenRemoveUser);
    QSignalSpy sysSpy(logic,    &AppLogic::appendSystemMessage);
 
    logic->onNetworkUserLeft("bob");
 
    ASSERT_EQ(removeSpy.count(), 1);
    EXPECT_EQ(removeSpy.first().first().toString(), QString("bob"));
    ASSERT_EQ(sysSpy.count(), 1);
    EXPECT_TRUE(sysSpy.first().first().toString().contains("bob"));
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 13 – onNetworkUserListReceived replaces the entire user list
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, UserListReceivedRefreshesOnlineList)
{
    // Pre-populate with a stale user
    logic->onNetworkUserJoined("staleUser");
 
    QSignalSpy clearSpy(logic, &AppLogic::chatScreenClearUsers);
    QSignalSpy addSpy(logic,   &AppLogic::chatScreenAddUser);
 
    QStringList freshList = {"alice", "bob", "carol"};
    logic->onNetworkUserListReceived(freshList);
 
    // Must clear first, then add each user
    ASSERT_GE(clearSpy.count(), 1);
    EXPECT_EQ(addSpy.count(), 3);
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 14 – Logout forwards disconnect to network and navigates to login
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, LogoutDisconnectsAndNavigatesToLogin)
{
    EXPECT_CALL(*net, connectToServer(::testing::_, ::testing::_, ::testing::_))
        .Times(1);
    logic->onGuiLoginRequested("alice", "127.0.0.1", 12345);
    logic->onNetworkConnected();
 
    EXPECT_CALL(*net, disconnectFromServer()).Times(1);
 
    logic->onGuiLogoutRequested();
    // NetworkManager will eventually call onNetworkDisconnected;
    // simulate that here to verify navigation
    QSignalSpy navSpy(logic,   &AppLogic::navigateToLoginScreen);
    QSignalSpy inputSpy(logic, &AppLogic::setChatInputEnabled);
    logic->onNetworkDisconnected();
 
    ASSERT_EQ(navSpy.count(), 1);
    // Chat input must be locked again
    bool inputLocked = false;
    for (auto &args : inputSpy)
        if (!args.first().toBool()) inputLocked = true;
    EXPECT_TRUE(inputLocked) << "Chat input must be disabled after disconnect";
}
 
// ─────────────────────────────────────────────────────────────────────────────
// TEST 15 – Whitespace-only username is rejected (trimming guard)
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(AppLogicTest, WhitespaceOnlyUsernameIsRejected)
{
    EXPECT_CALL(*net, connectToServer(::testing::_, ::testing::_, ::testing::_))
        .Times(0);
 
    QSignalSpy errorSpy(logic, &AppLogic::showLoginError);
    logic->onGuiLoginRequested("   ", "127.0.0.1", 12345);
 
    ASSERT_EQ(errorSpy.count(), 1);
}
 
// ─────────────────────────────────────────────────────────────────────────────
// main – initialise Qt before running tests
// ─────────────────────────────────────────────────────────────────────────────
int main(int argc, char **argv)
{
    // QCoreApplication is required for Qt signal/slot to work in unit tests
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
 
#include "test_applogic.moc"   // Required by Qt MOC for QObject in the test file
 