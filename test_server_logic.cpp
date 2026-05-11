// test_server_logic.cpp
// Unit tests for the Server Business Logic
// Tests routing decisions and state management INDEPENDENTLY of Boost.Asio.
// No real sockets are created; the server logic is extracted into a plain
// testable class (ServerLogic) that the production chatserver_activation.cpp
// delegates to.
//
// Build:
//   g++ -std=c++20 test_server_logic.cpp \
//       -lgtest -lgmock -lgtest_main -pthread -o test_server_logic
//
// Or in CMakeLists.txt:
//   find_package(GTest REQUIRED)
//   add_executable(test_server_logic test_server_logic.cpp)
//   target_link_libraries(test_server_logic PRIVATE GTest::gtest GTest::gmock
//                         GTest::gtest_main nlohmann_json::nlohmann_json)
//   add_test(NAME ServerLogicTests COMMAND test_server_logic)

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nlohmann/json.hpp>
#include <string>
#include <set>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>

using json = nlohmann::json;
using ::testing::Contains;
using ::testing::Not;
using ::testing::SizeIs;

// ─────────────────────────────────────────────────────────────────────────────
// Testable Server Abstractions
//
// We lift the routing and state logic from chatserver_activation.cpp into
// a standalone ServerLogic class that has NO Boost.Asio dependency.
// The production file can construct one of these and delegate to it.
// ─────────────────────────────────────────────────────────────────────────────

// Represents one connected client (ID only – no real socket)
struct FakeSession {
    int         id;
    std::string username = "Anonymous";
};

using SessionPtr = std::shared_ptr<FakeSession>;

// Callback used by ServerLogic to "send" a JSON message to a specific session
using SendFn = std::function<void(SessionPtr, const json &)>;

// ─────────────────────────────────────────────────────────────────────────────
// ServerLogic  –  the pure business logic extracted from chatserver_activation.cpp
// ─────────────────────────────────────────────────────────────────────────────
class ServerLogic
{
public:
    // Inject a send callback so tests can capture outgoing messages
    explicit ServerLogic(SendFn sender) : m_send(std::move(sender)) {}

    // Called when a new TCP connection is accepted
    SessionPtr onClientConnected()
    {
        auto s = std::make_shared<FakeSession>();
        s->id  = m_nextId++;
        m_sessions.insert(s);
        return s;
    }

    // Called when a raw JSON string arrives from a client
    void onMessageReceived(SessionPtr session, const std::string &raw)
    {
        json j;
        try { j = json::parse(raw); }
        catch (...) { return; } // malformed JSON – silently drop

        std::string type = j.value("type", "");

        if (type == "login") {
            session->username = j.value("username", "Guest");
            broadcast({ {"type", "user_joined"}, {"username", session->username} });
        }
        else if (type == "chat") {
            std::string text = j.value("text", "");
            if (text.empty()) return; // don't relay empty messages
            broadcast({
                {"type",   "incoming_message"},
                {"sender", session->username},
                {"text",   text}
            });
        }
        // Unknown type – silently ignore (no crash)
    }

    // Called when a client disconnects (TCP close / error)
    void onClientDisconnected(SessionPtr session)
    {
        m_sessions.erase(session);
        broadcast({ {"type", "user_left"}, {"username", session->username} });
    }

    // Helpers for test assertions
    std::size_t sessionCount()  const { return m_sessions.size(); }
    bool        hasSession(SessionPtr s) const { return m_sessions.count(s) > 0; }

private:
    void broadcast(const json &msg)
    {
        for (auto &s : m_sessions)
            m_send(s, msg);
    }

    SendFn                       m_send;
    std::set<SessionPtr>         m_sessions;
    int                          m_nextId = 1;
};

// ─────────────────────────────────────────────────────────────────────────────
// MockSender  –  captures every message "sent" during a test
// ─────────────────────────────────────────────────────────────────────────────
struct SentMessage {
    int         recipientId;
    std::string type;
    json        payload;
};

class MockSender
{
public:
    std::vector<SentMessage> sent;

    SendFn asSendFn()
    {
        return [this](SessionPtr s, const json &msg) {
            sent.push_back({ s->id, msg.value("type",""), msg });
        };
    }

    // Convenience: all messages of a given type
    std::vector<SentMessage> ofType(const std::string &t) const
    {
        std::vector<SentMessage> out;
        std::copy_if(sent.begin(), sent.end(), std::back_inserter(out),
                     [&](const SentMessage &m){ return m.type == t; });
        return out;
    }

    void clear() { sent.clear(); }
};

// ─────────────────────────────────────────────────────────────────────────────
// Test Fixture
// ─────────────────────────────────────────────────────────────────────────────
class ServerLogicTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        sender = std::make_unique<MockSender>();
        logic  = std::make_unique<ServerLogic>(sender->asSendFn());
    }

    std::unique_ptr<MockSender>  sender;
    std::unique_ptr<ServerLogic> logic;
};

// ─────────────────────────────────────────────────────────────────────────────
// TEST 1 – New connection is tracked in the session set
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, NewConnectionIsTracked)
{
    EXPECT_EQ(logic->sessionCount(), 0u);
    auto s = logic->onClientConnected();
    EXPECT_EQ(logic->sessionCount(), 1u);
    EXPECT_TRUE(logic->hasSession(s));
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 2 – Login message sets username and broadcasts user_joined
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, LoginSetsUsernameAndBroadcastsUserJoined)
{
    auto alice = logic->onClientConnected();
    auto bob   = logic->onClientConnected();
    sender->clear(); // discard any connection-phase messages

    json loginMsg = { {"type", "login"}, {"username", "alice"} };
    logic->onMessageReceived(alice, loginMsg.dump());

    EXPECT_EQ(alice->username, "alice");

    // Both alice and bob should receive the user_joined broadcast
    auto joined = sender->ofType("user_joined");
    ASSERT_EQ(joined.size(), 2u) << "user_joined must be broadcast to all sessions";

    // Every recipient must see the correct username
    for (auto &m : joined)
        EXPECT_EQ(m.payload.value("username",""), "alice");
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 3 – Chat message is broadcast to ALL connected clients
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, ChatIsBroadcastToAllClients)
{
    auto alice = logic->onClientConnected();
    auto bob   = logic->onClientConnected();
    auto carol = logic->onClientConnected();

    logic->onMessageReceived(alice, json{{"type","login"},{"username","alice"}}.dump());
    logic->onMessageReceived(bob,   json{{"type","login"},{"username","bob"}}.dump());
    logic->onMessageReceived(carol, json{{"type","login"},{"username","carol"}}.dump());
    sender->clear();

    logic->onMessageReceived(alice,
        json{{"type","chat"},{"text","Hello everyone!"}}.dump());

    auto msgs = sender->ofType("incoming_message");
    // All 3 sessions receive the message (server reflects to sender too)
    ASSERT_EQ(msgs.size(), 3u);
    for (auto &m : msgs) {
        EXPECT_EQ(m.payload.value("sender",""), "alice");
        EXPECT_EQ(m.payload.value("text",""),   "Hello everyone!");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 4 – Empty chat text is silently dropped (not broadcast)
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, EmptyChatTextIsDropped)
{
    auto alice = logic->onClientConnected();
    logic->onMessageReceived(alice, json{{"type","login"},{"username","alice"}}.dump());
    sender->clear();

    logic->onMessageReceived(alice, json{{"type","chat"},{"text",""}}.dump());

    EXPECT_TRUE(sender->ofType("incoming_message").empty())
        << "Empty chat payload must not be relayed to any client";
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 5 – Malformed JSON is silently ignored (no crash, no broadcast)
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, MalformedJsonIsIgnored)
{
    auto alice = logic->onClientConnected();
    sender->clear();

    EXPECT_NO_THROW(logic->onMessageReceived(alice, "this is not json {{{"));
    EXPECT_TRUE(sender->sent.empty());
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 6 – Unknown message type is silently ignored
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, UnknownMessageTypeIsIgnored)
{
    auto alice = logic->onClientConnected();
    logic->onMessageReceived(alice, json{{"type","login"},{"username","alice"}}.dump());
    sender->clear();

    logic->onMessageReceived(alice,
        json{{"type","ping"},{"data","something"}}.dump());

    EXPECT_TRUE(sender->sent.empty())
        << "Unrecognised message types must not trigger any broadcast";
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 7 – Disconnect removes session and broadcasts user_left
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, DisconnectRemovesSessionAndBroadcastsUserLeft)
{
    auto alice = logic->onClientConnected();
    auto bob   = logic->onClientConnected();

    logic->onMessageReceived(alice, json{{"type","login"},{"username","alice"}}.dump());
    logic->onMessageReceived(bob,   json{{"type","login"},{"username","bob"}}.dump());
    sender->clear();

    logic->onClientDisconnected(alice);

    // alice is gone from the session set
    EXPECT_EQ(logic->sessionCount(), 1u);
    EXPECT_FALSE(logic->hasSession(alice));

    // bob still connected; must receive user_left
    auto leftMsgs = sender->ofType("user_left");
    ASSERT_FALSE(leftMsgs.empty());
    EXPECT_EQ(leftMsgs.front().payload.value("username",""), "alice");
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 8 – Last client disconnecting leaves the session set empty
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, LastClientDisconnectLeavesEmptySet)
{
    auto alice = logic->onClientConnected();
    logic->onMessageReceived(alice, json{{"type","login"},{"username","alice"}}.dump());
    logic->onClientDisconnected(alice);

    EXPECT_EQ(logic->sessionCount(), 0u);
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 9 – Multiple clients each receive independent unique session IDs
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, EachClientGetsUniqueSessionId)
{
    auto s1 = logic->onClientConnected();
    auto s2 = logic->onClientConnected();
    auto s3 = logic->onClientConnected();

    EXPECT_NE(s1->id, s2->id);
    EXPECT_NE(s2->id, s3->id);
    EXPECT_NE(s1->id, s3->id);
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 10 – Chat sender field matches the session's logged-in username
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, ChatSenderMatchesLoginUsername)
{
    auto alice = logic->onClientConnected();
    logic->onMessageReceived(alice, json{{"type","login"},{"username","alice"}}.dump());
    sender->clear();

    logic->onMessageReceived(alice, json{{"type","chat"},{"text","test"}}.dump());

    auto msgs = sender->ofType("incoming_message");
    ASSERT_FALSE(msgs.empty());
    // The server stamps the sender as the session's stored username,
    // NOT whatever the client puts in the "sender" field of the message.
    EXPECT_EQ(msgs.front().payload.value("sender",""), "alice");
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 11 – Chat before login uses default "Anonymous" username
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, ChatBeforeLoginUsesAnonymousUsername)
{
    auto ghost = logic->onClientConnected();
    // No login message – send chat directly
    logic->onMessageReceived(ghost, json{{"type","chat"},{"text","hello"}}.dump());

    auto msgs = sender->ofType("incoming_message");
    ASSERT_FALSE(msgs.empty());
    EXPECT_EQ(msgs.front().payload.value("sender",""), "Anonymous");
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 12 – Disconnected client does not receive subsequent broadcasts
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(ServerLogicTest, DisconnectedClientReceivesNoBroadcasts)
{
    auto alice = logic->onClientConnected();
    auto bob   = logic->onClientConnected();

    logic->onMessageReceived(alice, json{{"type","login"},{"username","alice"}}.dump());
    logic->onMessageReceived(bob,   json{{"type","login"},{"username","bob"}}.dump());

    // Alice leaves
    logic->onClientDisconnected(alice);
    sender->clear();

    // Bob sends a message
    logic->onMessageReceived(bob, json{{"type","chat"},{"text","is anyone there?"}}.dump());

    auto msgs = sender->ofType("incoming_message");
    // Only bob (1 recipient) should receive it
    EXPECT_EQ(msgs.size(), 1u);
    EXPECT_EQ(msgs.front().recipientId, bob->id);
}
