cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(editor VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(editor src/main.cpp)
add_library(editor_lib src/editor.cpp)
target_link_libraries(editor PRIVATE editor_lib)

message(STATUS "editor will use C++ standard: ${STD}")

target_include_directories(editor PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(editor_lib PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(editor PRIVATE
    -Wall -Wextra -pedantic -Werror
)
target_compile_options(editor PRIVATE
    -Wall -Wextra -pedantic -Werror
)

if(WITH_BOOST_TEST)
    enable_testing()

    find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)

    add_executable(test_editor tests/test_editor.cpp)

    target_include_directories(
        test_editor
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(test_editor PRIVATE Boost::unit_test_framework editor_lib)

    target_compile_options(test_editor PRIVATE
        -Wall -Wextra -pedantic -Werror
    )

    add_test(
        NAME test
        COMMAND $<TARGET_FILE:test_editor>
    )
endif()

install(TARGETS editor RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
