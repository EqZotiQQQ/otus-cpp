cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
project(chat VERSION 0.0.${PATCH_VERSION})

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

add_library(client_lib src/client.cpp)
target_link_libraries(client_lib PUBLIC Boost::system Boost::thread Boost::program_options
                                        spdlog::spdlog chat_proto)
target_include_directories(client_lib PUBLIC include)

add_executable(client src/client_main.cpp)
target_link_libraries(client PUBLIC client_lib)

# Server

add_library(server_lib src/chat_room.cpp src/server.cpp src/user_manager.cpp src/command_parser.cpp
                       src/session_impl.cpp src/network_session.cpp)
target_link_libraries(server_lib PUBLIC Boost::system Boost::thread Boost::program_options
                                        spdlog::spdlog chat_proto)
target_include_directories(server_lib PUBLIC include)

add_executable(server src/server_main.cpp)
target_link_libraries(server PUBLIC server_lib)

if(WITH_BOOST_TEST)
    enable_testing()
    find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)
    add_executable(test_server tests/test_server.cpp)
    target_include_directories(test_server PRIVATE include)
    target_link_libraries(test_server PRIVATE Boost::unit_test_framework server_lib)
    add_test(NAME test COMMAND $<TARGET_FILE:test_server>)
endif()


include(CPack)

install(TARGETS server client RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_NAME "otus-hws")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Linux")
    
    
include(CPack)
