#include <iostream>
#include <string>
#include <set>
#include <memory> // memory management
#include <boost/asio.hpp>
#include <nlohmann/json.hpp> // Standard JSON library for C++

using boost::asio::ip::tcp;
using json = nlohmann::json;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

// Strucuter that will track every single online user at a given moment
struct ChatSession : public std::enable_shared_from_this<ChatSession> {
    tcp::socket socket;
    std::string username = "Anonymous";

    ChatSession(tcp::socket s) : socket(std::move(s)) {}
};

std::set<std::shared_ptr<ChatSession>> online_users;

// Sending JSON to a specific client
void send_to_client(std::shared_ptr<ChatSession> session, const json& j) {
    std::string data = j.dump() + "\n"; // Adding newline as a message delimiter
    boost::asio::write(session->socket, boost::asio::buffer(data));
}

// Broadcast JSON to all online clients
void broadcast(const json& j) {
    for (auto& session : online_users) {
        send_to_client(session, j);
    }
}

awaitable<void> handle_client(tcp::socket socket) {
    auto session = std::make_shared<ChatSession>(std::move(socket));
    online_users.insert(session);

    std::cout << "[Server] New connection established.\n";

    try {
        char data[4096];
        while (true) {
//          // step 1, read raw data
            auto [ec, bytes_read] = co_await session->socket.async_read_some(
                boost::asio::buffer(data), as_tuple(use_awaitable));

            if (ec) break; // Graceful exit on disconnect or error

            // step 2, parse the JSON message from the client
            std::string raw_msg(data, bytes_read);
            auto j = json::parse(raw_msg);

            // Step 3, routing
            std::string type = j.value("type", "");

            if (type == "login") {
                session->username = j.value("username", "Guest");
                std::cout << "[Login] " << session->username << " has joined.\n";

                // Tell others someone joined
                broadcast({{"type", "user_joined"}, {"username", session->username}});
            }
            else if (type == "chat") {
                std::cout << "[Chat] " << session->username << ": " << j["text"] << "\n";

                // Route message to everyone (including sender)
                broadcast({
                    {"type", "incoming_message"},
                    {"sender", session->username},
                    {"text", j["text"]}
                });
            }
        }
    } catch (const std::exception& e) {
            std::cerr << "[Error] Session error: " << e.what() << "\n";
    }
    // step 4, graceful disconnection
    online_users.erase(session);
    std::cout << "[Server] " << session->username << " disconnected.\n";
    broadcast({{"type", "user_left"}, {"username", session->username}});
}

// listener coroutine
awaitable<void> listener() {
    auto executor = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), 12345});

    std::cout << "Boost.Asio Server running on port 54321...\n";

    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));
        if (!ec) {
            co_spawn(executor, handle_client(std::move(socket)), detached);
        }
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        co_spawn(io_context, listener(), detached);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
    }
    return 0;
}
// Created by Mohamed Fadul on 5/11/26.
//
