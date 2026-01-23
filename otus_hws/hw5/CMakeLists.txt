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

# find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# find_package(Boost REQUIRED COMPONENTS type_index)
# target_link_libraries(editor_lib PUBLIC Boost::type_index)

# find_package(Flatbuffers REQUIRED) # header only

find_package(spdlog REQUIRED)
target_link_libraries(editor_lib PUBLIC spdlog::spdlog)

target_link_libraries(editor_lib PUBLIC sfml-graphics sfml-window sfml-system)


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

MESSAGE ("Generating FlatBuffers schema to ${CMAKE_BINARY_DIR}/include/shapes_generated.h")
MESSAGE ("Generating FlatBuffers schema to ${CMAKE_CURRENT_SOURCE_DIR}/schemas/shapes.fbs")

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/include/shapes_generated.h
    COMMAND flatc --cpp --gen-object-api
            -o ${CMAKE_BINARY_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/schemas/shapes.fbs
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/schemas/shapes.fbs
    COMMENT "Generating FlatBuffers schema"
)

add_custom_target(generate_flatbuffers ALL
    DEPENDS ${CMAKE_BINARY_DIR}/include/shapes_generated.h
)

target_include_directories(editor  PRIVATE ${CMAKE_BINARY_DIR}/include) # othervice public to remove dup
target_include_directories(editor_lib  PRIVATE ${CMAKE_BINARY_DIR}/include)

add_dependencies(editor_lib generate_flatbuffers)

target_include_directories(
    editor_lib
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    /usr/include            # flatbuffers headers
)

# if(WITH_BOOST_TEST)
#     enable_testing()

#     find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)

#     add_executable(test_editor tests/test_editor.cpp)

#     target_include_directories(
#         test_editor
#         PRIVATE
#             "${CMAKE_CURRENT_SOURCE_DIR}/include"
#     )

#     target_link_libraries(test_editor PRIVATE Boost::unit_test_framework editor_lib)

#     target_compile_options(test_editor PRIVATE
#         -Wall -Wextra -pedantic -Werror
#     )

#     add_test(
#         NAME test
#         COMMAND $<TARGET_FILE:test_editor>
#     )
# endif()

install(TARGETS editor RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
