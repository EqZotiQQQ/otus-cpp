cmake_minimum_required(VERSION 3.28.3)
project(otus_hws)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

# include(FetchContent)
# FetchContent_Declare(
#     range-v3
#     GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
#     GIT_TAG        master
# )
# FetchContent_MakeAvailable(range-v3)

# global level packages:
find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)

include(CTest)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)
# add_subdirectory(otus_hws/hw1)
# add_subdirectory(otus_hws/hw2)
# add_subdirectory(otus_hws/hw3)
# add_subdirectory(otus_hws/hw4)
add_subdirectory(otus_hws/hw5)

# add_subdirectory(experiments)
# add_subdirectory(problems)

