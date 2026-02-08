cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(outer_join VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

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

add_library(simple_inmemory_sql src/in_memory_sql.cpp)
target_link_libraries(simple_inmemory_sql PUBLIC Boost::system Boost::thread Boost::program_options spdlog::spdlog)

add_executable(outer_join_server src/in_memory_server.cpp)

target_link_libraries(outer_join_server PUBLIC simple_inmemory_sql)

message(STATUS "async will use C++ standard: ${STD}")

target_include_directories(simple_inmemory_sql PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(outer_join_server PUBLIC
    -Wall -Wextra -pedantic -Werror
)

install(TARGETS outer_join_server outer_join_server
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(WITH_BOOST_TEST)
    enable_testing()

    find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)

    add_executable(test_simple_inmemory_sql tests/test_in_memory_server.cpp)

    target_include_directories(
        test_simple_inmemory_sql
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(test_simple_inmemory_sql PRIVATE Boost::unit_test_framework simple_inmemory_sql)

    target_compile_options(test_simple_inmemory_sql PRIVATE
        -Wall -Wextra -pedantic -Werror
    )

    add_test(
        NAME test
        COMMAND $<TARGET_FILE:test_simple_inmemory_sql>
    )
endif()

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
