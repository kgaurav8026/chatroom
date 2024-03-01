// automates the build process of the project
cmake_minimum_required(VERSION 3.10)
project(SocketClient C)

# Add the executable target
add_executable(SocketClient client.c socketutil.c)

# Set the C standard to C11
set(CMAKE_C_STANDARD 11)

# Include the current directory for header files
target_include_directories(SocketClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
