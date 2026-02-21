cmake_minimum_required(VERSION 3.28.3)

project(chat VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost REQUIRED COMPONENTS system thread program_options)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
include(dependencies)

add_library(chat_proto)

target_sources(chat_proto PRIVATE proto/schema.proto)

target_link_libraries(chat_proto PUBLIC protobuf::libprotobuf)

target_include_directories(chat_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Client

add_executable(client src/client_main.cpp src/client.cpp)

target_link_libraries(client PRIVATE Boost::system Boost::thread Boost::program_options
                                     spdlog::spdlog chat_proto)

target_include_directories(client PRIVATE include)

# Server

add_executable(server src/server_main.cpp src/chat_room.cpp src/server.cpp src/user_manager.cpp
                      src/command_parser.cpp)

target_link_libraries(server PRIVATE Boost::system Boost::thread Boost::program_options
                                     spdlog::spdlog chat_proto)

target_include_directories(server PRIVATE include)
