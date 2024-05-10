cmake_minimum_required(VERSION 3.10)

project(MySocketProject)

add_executable(SocketServer server.cpp socketutil.cpp)
add_executable(SocketClient client.cpp socketutil.cpp)

target_include_directories(SocketServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(SocketClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})