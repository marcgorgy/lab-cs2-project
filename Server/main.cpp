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



//
// Created by Mohamed Fadul on 5/11/26.
//
