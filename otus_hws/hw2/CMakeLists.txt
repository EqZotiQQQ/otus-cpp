cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(ip_filter VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_executable(ip_filter src/ip_filter.cpp)

set_target_properties(ip_filter PROPERTIES
CXX_STANDARD 23
CXX_STANDARD_REQUIRED ON
)

get_target_property(STD ip_filter CXX_STANDARD)
message(STATUS "ip_filter will use C++ standard: ${STD}")

target_include_directories(ip_filter PRIVATE 
    ${GENERATED_DIR} 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(ip_filter PRIVATE
    -Wall -Wextra -pedantic -Werror
)
target_compile_options(ip_filter PRIVATE
    -Wall -Wextra -pedantic -Werror
)

if(WITH_BOOST_TEST)
    # find_package(Boost COMPONENTS unit_test_framework REQUIRED)

    # add_executable(test_version tests/test_version.cpp)

    # target_include_directories(
    #     test_version
    #     PRIVATE
    #         "${CMAKE_CURRENT_SOURCE_DIR}/include"  # Абсолютный путь
    #         ${GENERATED_DIR}
    # )

    # set_target_properties(test_version PROPERTIES
    #     CXX_STANDARD 23
    #     CXX_STANDARD_REQUIRED ON
    # )

    # target_link_libraries(test_version
    #     ${Boost_LIBRARIES}
    #     ip_filter
    # )

    # target_compile_options(test_version PRIVATE
    #     -Wall -Wextra -pedantic -Werror
    # )
endif()

install(TARGETS ip_filter RUNTIME DESTINATION bin)

if(WITH_BOOST_TEST)
    enable_testing()
    add_test(test_version test_version)
endif()