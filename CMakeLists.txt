cmake_minimum_required(VERSION 3.16)
project(CombinedChatApp LANGUAGES CXX)

# Use C++20 to satisfy the server's requirement for coroutines 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt-specific automatic tools [cite: 1]
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# --- Find All Dependencies ---

# Find Qt (Client dependencies) [cite: 1]
find_package(Qt6 COMPONENTS Core Widgets Network Test QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
endif()

# Find Boost and JSON (Server dependencies) 
find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)

# --- Source Files ---

# Combined sources from both projects 
# IMPORTANT: You must rename one of the 'main.cpp' files to avoid a conflict.
# Here, the server entry point is assumed to be renamed to 'server_main.cpp'.
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

# --- Create Single Executable ---

add_executable(CombinedChatApp ${SOURCES} ${HEADERS})

# Link all libraries to the single executable
if(Qt6_FOUND)
    target_link_libraries(CombinedChatApp PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)
else()
    target_link_libraries(CombinedChatApp PRIVATE Qt5::Core Qt5::Widgets Qt5::Network)
endif()

target_link_libraries(CombinedChatApp PRIVATE
    Boost::boost
    Boost::system
    nlohmann_json::nlohmann_json
)
# ─── CMakeLists_tests.txt ────────────────────────────────────────────────────
# Append this block to the bottom of your existing CMakeLists.txt,
# OR include it as a subdirectory (add_subdirectory(tests)).
#
# Prerequisites already declared in your main CMakeLists.txt:
#   find_package(Qt6/Qt5 ...)
#   find_package(nlohmann_json REQUIRED)
#
# GoogleTest is fetched automatically if not installed system-wide.
# ─────────────────────────────────────────────────────────────────────────────
 
enable_testing()
 
# ── Fetch GoogleTest if not already available ─────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
set(INSTALL_GTEST OFF)
set(BUILD_GMOCK ON)
FetchContent_MakeAvailable(googletest)
 
# ── Test target 1: Client Business Logic (AppLogic) ──────────────────────────
add_executable(test_applogic
    test_applogic.cpp
    applogic.cpp        # the real AppLogic implementation under test
)
 
target_link_libraries(test_applogic PRIVATE
    GTest::gtest
    GTest::gmock
    
)
 
# Link the correct Qt version (mirrors the main executable)
if(Qt6_FOUND)
    target_link_libraries(test_applogic PRIVATE Qt6::Core Qt::Test)
else()
    target_link_libraries(test_applogic PRIVATE Qt5::Core Qt::Test)
endif()
 
# Qt MOC must run on the test file (it contains QObject subclasses)
set_target_properties(test_applogic PROPERTIES AUTOMOC ON)
 
add_test(NAME AppLogicTests COMMAND test_applogic)
 
# ── Test target 2: Server Business Logic (pure C++, NO Boost.Asio) ───────────
add_executable(test_server_logic
    test_server_logic.cpp
    # NOTE: we do NOT compile chatserver_activation.cpp here because
    # test_server_logic.cpp defines its own inline ServerLogic class.
    # This keeps the tests free of any Boost.Asio dependency.
)
 
target_link_libraries(test_server_logic PRIVATE
    GTest::gtest
    GTest::gmock
    GTest::gtest_main          # server tests use the default main()
    nlohmann_json::nlohmann_json
)
 
add_test(NAME ServerLogicTests COMMAND test_server_logic)