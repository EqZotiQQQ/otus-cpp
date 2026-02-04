cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(bayan VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.1.0
)

FetchContent_MakeAvailable(fmt)

add_executable(bayan src/main.cpp)
add_library(bayan_lib src/bayan.cpp)
target_link_libraries(bayan PRIVATE bayan_lib)

message(STATUS "bayan will use C++ standard: ${CMAKE_CXX_STANDARD}")

target_include_directories(bayan PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(bayan_lib PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS program_options)

target_link_libraries(bayan_lib
    PUBLIC
        Boost::program_options
)
target_compile_options(bayan PRIVATE
    -Wall -Wextra -pedantic -Werror
)

target_link_libraries(bayan_lib
    PUBLIC
        fmt::fmt
)

# if(WITH_BOOST_TEST)
#     enable_testing()

#     find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)

#     add_executable(test_bayan tests/test_bayan.cpp)

#     target_include_directories(
#         test_bayan
#         PRIVATE
#             "${CMAKE_CURRENT_SOURCE_DIR}/include"
#     )

#     target_link_libraries(test_bayan PRIVATE Boost::unit_test_framework bayan_lib)

#     target_compile_options(test_bayan PRIVATE
#         -Wall -Wextra -pedantic -Werror
#     )

#     add_test(
#         NAME test
#         COMMAND $<TARGET_FILE:test_bayan>
#     )
# endif()

install(TARGETS bayan RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
