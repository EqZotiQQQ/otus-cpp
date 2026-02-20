cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(chat VERSION ${PROJECT_VERSION})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost REQUIRED COMPONENTS system thread program_options)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.1.0
)

set(FMT_BUILD_SHARED OFF)
set(FMT_BUILD_TESTS OFF)
set(FMT_BUILD_DOCS OFF)

FetchContent_MakeAvailable(fmt)

set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_BUILD_SHARED OFF)
set(SPDLOG_BUILD_EXAMPLE OFF)
set(SPDLOG_BUILD_TESTS OFF)
set(SPDLOG_BUILD_BENCH OFF)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(spdlog)

add_executable(client src/client_main.cpp src/client.cpp src/command_parser.cpp)
target_link_libraries(client PUBLIC Boost::system Boost::thread Boost::program_options spdlog::spdlog)
target_include_directories(client PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


add_executable(server src/server_main.cpp src/chat_room.cpp src/server.cpp src/user_manager.cpp)
target_link_libraries(server PUBLIC Boost::system Boost::thread Boost::program_options spdlog::spdlog)
target_include_directories(server PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


message(STATUS "chat will use C++ standard: ${STD}")

install(TARGETS server client
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)


set(CPACK_PACKAGE_NAME "otus_hws")
set(CPACK_PACKAGE_DESCRIPTION "Outer joiner")
set(CPACK_PACKAGE_CONTACT "mf_52@mail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "EqZotiQQQ")
set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27)")

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

include(CPack)
