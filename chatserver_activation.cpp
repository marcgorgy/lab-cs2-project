#include <iostream>
#include <string>
#include <set>
#include <map>
#include <memory>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;
using json = nlohmann::json;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

// ─── Session ─────────────────────────────────────────────────────────────────

struct ChatSession : public std::enable_shared_from_this<ChatSession> {
    tcp::socket socket;
    std::string username = "Anonymous";
    std::string group;              // empty = global channel

    explicit ChatSession(tcp::socket s) : socket(std::move(s)) {}
};

using SessionPtr = std::shared_ptr<ChatSession>;

// ─── Global state ─────────────────────────────────────────────────────────────

std::set<SessionPtr>                      online_users;
std::map<std::string, std::set<SessionPtr>> groups; // group name → members

// ─── Helpers ──────────────────────────────────────────────────────────────────

void send_to_client(const SessionPtr &session, const json &j)
{
    std::string data = j.dump() + "\n";
    boost::asio::write(session->socket, boost::asio::buffer(data));
}

// Broadcast to every connected client
void broadcast(const json &j)
{
    for (auto &s : online_users)
        send_to_client(s, j);
}

// Send to every member of a named group only
void broadcast_group(const std::string &group_name, const json &j)
{
    auto it = groups.find(group_name);
    if (it == groups.end()) return;
    for (auto &s : it->second)
        send_to_client(s, j);
}

// ─── Per-client coroutine ─────────────────────────────────────────────────────

awaitable<void> handle_client(tcp::socket socket)
{
    auto session = std::make_shared<ChatSession>(std::move(socket));
    online_users.insert(session);

    std::cout << "[Server] New connection established.\n";

    try {
        char data[4096];
        while (true) {
            auto [ec, bytes_read] = co_await session->socket.async_read_some(
                boost::asio::buffer(data), as_tuple(use_awaitable));

            if (ec) break;

            std::string raw_msg(data, bytes_read);
            json j;
            try { j = json::parse(raw_msg); }
            catch (...) { continue; } // malformed JSON — skip

            std::string type = j.value("type", "");

            // ── login ────────────────────────────────────────────────────────
            if (type == "login") {
                session->username = j.value("username", "Guest");
                std::cout << "[Login] " << session->username << " joined.\n";
                broadcast({ {"type", "user_joined"}, {"username", session->username} });
            }

            // ── chat (global) ────────────────────────────────────────────────
            else if (type == "chat") {
                std::string text = j.value("text", "");
                if (text.empty()) continue;
                std::cout << "[Chat] " << session->username << ": " << text << "\n";
                broadcast({
                    {"type",   "incoming_message"},
                    {"sender", session->username},
                    {"text",   text}
                });
            }

            // ── group_join ───────────────────────────────────────────────────
            else if (type == "group_join") {
                std::string group_name = j.value("group", "");
                if (group_name.empty()) continue;

                // Remove from old group if any
                if (!session->group.empty())
                    groups[session->group].erase(session);

                session->group = group_name;
                groups[group_name].insert(session);

                std::cout << "[Group] " << session->username
                          << " joined #" << group_name << "\n";

                // Notify all members of this group (including the joiner)
                broadcast_group(group_name, {
                    {"type",     "group_joined"},
                    {"group",    group_name},
                    {"username", session->username}
                });
            }

            // ── group_message ────────────────────────────────────────────────
            else if (type == "group_message") {
                std::string group_name = j.value("group", "");
                std::string text       = j.value("text",  "");
                if (group_name.empty() || text.empty()) continue;

                std::cout << "[#" << group_name << "] "
                          << session->username << ": " << text << "\n";

                broadcast_group(group_name, {
                    {"type",   "group_message"},
                    {"group",  group_name},
                    {"sender", session->username},
                    {"text",   text}
                });
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "[Error] Session error: " << e.what() << "\n";
    }

    // ── Graceful disconnection ───────────────────────────────────────────────
    if (!session->group.empty())
        groups[session->group].erase(session);

    online_users.erase(session);
    std::cout << "[Server] " << session->username << " disconnected.\n";
    broadcast({ {"type", "user_left"}, {"username", session->username} });
}

// ─── Listener coroutine ───────────────────────────────────────────────────────

awaitable<void> listener()
{
    auto executor = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), 12345});
    std::cout << "Boost.Asio Chat Server running on port 12345...\n";

    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));
        if (!ec)
            co_spawn(executor, handle_client(std::move(socket)), detached);
    }
}

// ─── Entry point (called from main.cpp) ──────────────────────────────────────

void runChatServer()
{
    try {
        boost::asio::io_context io_context;
        co_spawn(io_context, listener(), detached);
        io_context.run();
    } catch (const std::exception &e) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
    }
}
// Created by Mohamed Fadul on 5/11/26.