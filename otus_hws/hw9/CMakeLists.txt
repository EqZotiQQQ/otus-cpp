cmake_minimum_required(VERSION 3.28.3)

project(async)

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(async src/main.cpp)
add_library(async_lib SHARED src/bulk.cpp src/manager.cpp)
target_link_libraries(async PRIVATE async_lib)

message(STATUS "async will use C++ standard: ${STD}")

target_include_directories(async PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(async_lib PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(async PRIVATE
    -Wall -Wextra -pedantic -Werror
)

install(TARGETS async async_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)