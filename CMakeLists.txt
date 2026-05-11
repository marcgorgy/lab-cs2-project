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
find_package(Qt6 COMPONENTS Core Widgets Network QUIET)
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
