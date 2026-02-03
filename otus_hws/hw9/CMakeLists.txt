cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(async VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set(CPACK_PACKAGE_NAME "async")
# set(CPACK_PROJECT_NAME "async")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Async bulk processing library and tool")

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

# if(WITH_BOOST_TEST)
#     enable_testing()
#     find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)
#     add_executable(test_async tests/test_async.cpp)
#     target_include_directories(
#         test_async
#         PRIVATE
#             "${CMAKE_CURRENT_SOURCE_DIR}/include"
#     )
#     target_link_libraries(test_async PRIVATE Boost::unit_test_framework async_lib)
#     target_compile_options(test_async PRIVATE
#         -Wall -Wextra -pedantic -Werror
#     )
#     add_test(
#         NAME test
#         COMMAND $<TARGET_FILE:test_async>
#     )
# endif()

install(TARGETS async async_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)


# set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
# 
# set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
# set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
# set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
# set(CPACK_PACKAGE_CONTACT example@example.com)
# 
# include(CPack)
