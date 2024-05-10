cmake_minimum_required(VERSION 3.10)

project(MySocketProject)

add_executable(SocketServer server.c socketutil.c)
add_executable(SocketClient client.c socketutil.c)

target_include_directories(SocketServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(SocketClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})