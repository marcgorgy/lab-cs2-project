cmake_minimum_required(VERSION 3.16)
project(QtChatApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)

if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
endif()

set(SOURCES
    main.cpp
    MainWindow.cpp
    LoginScreen.cpp
    ChatRoomScreen.cpp
    SettingsScreen.cpp
    AppLogic.cpp
    NetworkManager.cpp
)

set(HEADERS
    MainWindow.h
    LoginScreen.h
    ChatRoomScreen.h
    SettingsScreen.h
    AppLogic.h
    NetworkManager.h
)

add_executable(QtChatApp ${SOURCES} ${HEADERS})

if(Qt6_FOUND)
    target_link_libraries(QtChatApp PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)
else()
    target_link_libraries(QtChatApp PRIVATE Qt5::Core Qt5::Widgets Qt5::Network)
endif()
