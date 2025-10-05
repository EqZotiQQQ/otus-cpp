cmake_minimum_required(VERSION 4.0)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(alloc VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(alloc src/main.cpp)
add_library(ip_alloc_lib src/alloc.cpp)
target_link_libraries(alloc PRIVATE ip_alloc_lib)

message(STATUS "alloc will use C++ standard: ${STD}")

target_include_directories(alloc PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(ip_alloc_lib PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(alloc PRIVATE
    -Wall -Wextra -pedantic -Werror
)
target_compile_options(alloc PRIVATE
    -Wall -Wextra -pedantic -Werror
)

if(WITH_BOOST_TEST)
    enable_testing()

    find_package(Boost 1.70 REQUIRED CONFIG COMPONENTS unit_test_framework)

    add_executable(test_alloc tests/test_alloc.cpp)

    target_include_directories(
        test_alloc
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(test_alloc PRIVATE Boost::unit_test_framework ip_alloc_lib)

    target_compile_options(test_alloc PRIVATE
        -Wall -Wextra -pedantic -Werror
    )

    add_test(
        NAME test
        COMMAND $<TARGET_FILE:test_alloc>
    )
endif()

install(TARGETS alloc RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
