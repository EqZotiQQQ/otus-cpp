cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(ip_filter VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB ASSET_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ASSET_FILES} "${CMAKE_CURRENT_BINARY_DIR}/assets/"
    COMMENT "Copying assets to build directory"
)

add_executable(ip_filter src/main.cpp)
add_library(ip_filter_lib src/ip_filter.cpp)
target_link_libraries(ip_filter PRIVATE ip_filter_lib)

message(STATUS "ip_filter will use C++ standard: ${STD}")

target_include_directories(ip_filter PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(ip_filter_lib PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(ip_filter PRIVATE
    -Wall -Wextra -pedantic -Werror
)
target_compile_options(ip_filter PRIVATE
    -Wall -Wextra -pedantic -Werror
)

if(WITH_BOOST_TEST)
    enable_testing()
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)

    add_executable(test_filter tests/test_filter.cpp)

    target_include_directories(
        test_filter
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(test_filter
        ${Boost_LIBRARIES}
        ip_filter_lib
    )

    target_compile_options(test_filter PRIVATE
        -Wall -Wextra -pedantic -Werror
    )

    add_test(test_filter test_filter)
endif()

install(TARGETS ip_filter RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)