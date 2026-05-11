cmake_minimum_required(VERSION 3.16)
project(CombinedChatApp LANGUAGES CXX)

# C++20 is required by the server coroutines (Boost.Asio co_spawn)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt automatic tooling
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ── Dependencies ──────────────────────────────────────────────────────────────

# Qt (client UI + networking)
find_package(Qt6 COMPONENTS Core Widgets Network Test QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets Network Test REQUIRED)
endif()

# Boost.Asio (server coroutine runtime)
find_package(Boost REQUIRED COMPONENTS system)

# nlohmann/json (server JSON parsing)
find_package(nlohmann_json REQUIRED)

# ── Main executable ───────────────────────────────────────────────────────────

set(SOURCES
    main.cpp
    MainWindow.cpp
    LoginScreen.cpp
    ChatRoomScreen.cpp
    SettingsScreen.cpp
    applogic.cpp
    NetworkManager.cpp
    chatserver_activation.cpp
)

set(HEADERS
    MainWindow.h
    LoginScreen.h
    ChatRoomScreen.h
    SettingsScreen.h
    applogic.h
    NetworkManager.h
)

add_executable(CombinedChatApp ${SOURCES} ${HEADERS})

if(Qt6_FOUND)
    target_link_libraries(CombinedChatApp PRIVATE
        Qt6::Core Qt6::Widgets Qt6::Network)
else()
    target_link_libraries(CombinedChatApp PRIVATE
        Qt5::Core Qt5::Widgets Qt5::Network)
endif()

target_link_libraries(CombinedChatApp PRIVATE
    Boost::boost
    Boost::system
    nlohmann_json::nlohmann_json
)

# ── Tests ─────────────────────────────────────────────────────────────────────

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
set(INSTALL_GTEST OFF)
set(BUILD_GMOCK ON)
FetchContent_MakeAvailable(googletest)

# Test target 1: Client business logic (AppLogic)
add_executable(test_applogic
    test_applogic.cpp
    applogic.cpp
)

set_target_properties(test_applogic PROPERTIES AUTOMOC ON)

target_link_libraries(test_applogic PRIVATE
    GTest::gtest
    GTest::gmock
)

if(Qt6_FOUND)
    target_link_libraries(test_applogic PRIVATE Qt6::Core Qt6::Test)
else()
    target_link_libraries(test_applogic PRIVATE Qt5::Core Qt5::Test)
endif()

add_test(NAME AppLogicTests COMMAND test_applogic)

# Test target 2: Server business logic (pure C++, no Boost.Asio)
# test_server_logic.cpp defines its own inline ServerLogic class,
# so chatserver_activation.cpp is intentionally NOT compiled here.
add_executable(test_server_logic
    test_server_logic.cpp
)

target_link_libraries(test_server_logic PRIVATE
    GTest::gtest
    GTest::gmock
    GTest::gtest_main
    nlohmann_json::nlohmann_json
)

add_test(NAME ServerLogicTests COMMAND test_server_logic)