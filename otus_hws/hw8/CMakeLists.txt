cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(duplicate_searcher VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(duplicate_searcher src/main.cpp)
add_library(duplicate_searcher_lib src/duplicate_searcher.cpp)
target_link_libraries(duplicate_searcher PRIVATE duplicate_searcher_lib)

message(STATUS "duplicate_searcher will use C++ standard: ${STD}")

target_include_directories(duplicate_searcher PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(duplicate_searcher_lib PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

find_package(Boost REQUIRED COMPONENTS program_options)

target_link_libraries(duplicate_searcher_lib
    PUBLIC
        Boost::program_options
)
target_compile_options(duplicate_searcher PRIVATE
    -Wall -Wextra -pedantic -Werror
)
target_compile_options(duplicate_searcher PRIVATE
    -Wall -Wextra -pedantic -Werror
)

# if(WITH_BOOST_TEST)
#     enable_testing()

#     find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)

#     add_executable(test_duplicate_searcher tests/test_duplicate_searcher.cpp)

#     target_include_directories(
#         test_duplicate_searcher
#         PRIVATE
#             "${CMAKE_CURRENT_SOURCE_DIR}/include"
#     )

#     target_link_libraries(test_duplicate_searcher PRIVATE Boost::unit_test_framework duplicate_searcher_lib)

#     target_compile_options(test_duplicate_searcher PRIVATE
#         -Wall -Wextra -pedantic -Werror
#     )

#     add_test(
#         NAME test
#         COMMAND $<TARGET_FILE:test_duplicate_searcher>
#     )
# endif()

install(TARGETS duplicate_searcher RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
