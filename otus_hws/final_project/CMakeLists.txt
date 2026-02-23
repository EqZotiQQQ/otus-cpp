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

set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/schema.proto)
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/build/generated)

file(MAKE_DIRECTORY ${GENERATED_DIR})

add_custom_command(
    OUTPUT ${GENERATED_DIR}/schema.pb.cc ${GENERATED_DIR}/schema.pb.h
    COMMAND protobuf::protoc ARGS --cpp_out=${GENERATED_DIR}
            --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto ${PROTO_FILE}
    DEPENDS ${PROTO_FILE})

add_library(chat_proto ${GENERATED_DIR}/schema.pb.cc)

target_link_libraries(chat_proto PUBLIC protobuf::libprotobuf)

target_include_directories(chat_proto PUBLIC ${GENERATED_DIR})

# Client

add_executable(client src/client_main.cpp src/client.cpp)

target_link_libraries(client PRIVATE Boost::system Boost::thread Boost::program_options
                                     spdlog::spdlog chat_proto)

target_include_directories(client PRIVATE include)

# Server

add_executable(server src/server_main.cpp src/chat_room.cpp src/server.cpp src/user_manager.cpp
                      src/command_parser.cpp src/session.cpp)

target_link_libraries(server PRIVATE Boost::system Boost::thread Boost::program_options
                                     spdlog::spdlog chat_proto)

target_include_directories(server PRIVATE include)

# if(WITH_BOOST_TEST) enable_testing() find_package(Boost 1.70 REQUIRED COMPONENTS
# unit_test_framework) add_executable(test_integrational tests/test_integrational.cpp)
# target_include_directories( test_chat PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" )
# target_link_libraries(test_chat PRIVATE Boost::unit_test_framework bulk_lib)
# target_compile_options(test_chat PRIVATE -Wall -Wextra -pedantic -Werror ) add_test( NAME test
# COMMAND $<TARGET_FILE:test_chat> ) endif()
