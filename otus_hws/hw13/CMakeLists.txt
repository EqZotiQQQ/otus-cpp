cmake_minimum_required(VERSION 3.10.0)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(ml_interface VERSION ${PROJECT_VERSION})

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

FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0   # можешь заменить на нужную версию или commit
)

# Чтобы не тянуть лишнее
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(eigen)

add_executable(ml_interface src/main.cpp)

target_link_libraries(ml_interface PUBLIC Boost::system Boost::thread Boost::program_options spdlog::spdlog Eigen3::Eigen)

message(STATUS "ml_interface will use C++ standard: ${STD}")

target_include_directories(ml_interface PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(ml_interface PUBLIC
    -Wall -Wextra -pedantic -Werror
)

install(TARGETS ml_interface
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
