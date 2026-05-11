cmake_minimum_required(VERSION 3.15)
project(ChatServer)

set(CMAKE_CXX_STANDARD 20) # for couroutines

find_package(nlohmann_json REQUIRED) # JSON library
add_executable(ChatServer main.cpp)

target_link_libraries(ChatServer PRIVATE
        Boost::boost
        Boost::system
        nlohmann_json::nlohmann_json
)