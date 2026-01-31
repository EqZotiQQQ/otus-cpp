cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(bulk VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# 
# include(FetchContent)
# 
# FetchContent_Declare(
#     fmt
#     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#     GIT_TAG 12.1.0
# )
# 
# set(FMT_BUILD_SHARED OFF)
# set(FMT_BUILD_TESTS OFF)
# set(FMT_BUILD_DOCS OFF)
# 
# FetchContent_MakeAvailable(fmt)
# 
# set(SPDLOG_FMT_EXTERNAL ON)
# set(SPDLOG_BUILD_SHARED OFF)
# set(SPDLOG_BUILD_EXAMPLE OFF)
# set(SPDLOG_BUILD_TESTS OFF)
# set(SPDLOG_BUILD_BENCH OFF)
# 
# FetchContent_Declare(
#     spdlog
#     GIT_REPOSITORY https://github.com/gabime/spdlog.git
#     GIT_TAG v1.17.0
# )
# 
# FetchContent_MakeAvailable(spdlog)
# 
add_executable(bulk src/main.cpp)
add_library(bulk_lib src/bulk.cpp)
target_link_libraries(bulk PRIVATE bulk_lib)
# 
# target_link_libraries(bulk_lib
#     PUBLIC
#     spdlog::spdlog
# )

message(STATUS "bulk will use C++ standard: ${STD}")

target_include_directories(bulk PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(bulk_lib PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(bulk PRIVATE
    -Wall -Wextra -pedantic -Werror
)
target_compile_options(bulk PRIVATE
    -Wall -Wextra -pedantic -Werror
)

# if(WITH_BOOST_TEST)
#     enable_testing()

#     find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)

#     add_executable(test_bulk tests/test_bulk.cpp)

#     target_include_directories(
#         test_bulk
#         PRIVATE
#             "${CMAKE_CURRENT_SOURCE_DIR}/include"
#     )

#     target_link_libraries(test_bulk PRIVATE Boost::unit_test_framework bulk_lib)

#     target_compile_options(test_bulk PRIVATE
#         -Wall -Wextra -pedantic -Werror
#     )

#     add_test(
#         NAME test
#         COMMAND $<TARGET_FILE:test_bulk>
#     )
# endif()

install(TARGETS bulk RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
