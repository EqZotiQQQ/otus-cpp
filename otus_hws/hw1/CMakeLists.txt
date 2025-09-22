cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(helloworld VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})
configure_file(config/version.h.in ${GENERATED_DIR}/version.h)

add_executable(helloworld_cli src/main.cpp)

add_library(helloworld src/lib.cpp)

target_include_directories(helloworld PRIVATE 
    "${CMAKE_BINARY_DIR}" 
    ${GENERATED_DIR} 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(helloworld_cli PRIVATE helloworld)

target_include_directories(helloworld_cli PRIVATE 
    ${GENERATED_DIR} 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(helloworld_cli PRIVATE
    -Wall -Wextra -pedantic -Werror
)
target_compile_options(helloworld PRIVATE
    -Wall -Wextra -pedantic -Werror
)

if(WITH_BOOST_TEST)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)

    add_executable(test_version tests/test_version.cpp)

    target_include_directories(
        test_version
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include"  # Абсолютный путь
            ${GENERATED_DIR}
    )

    target_link_libraries(test_version
        ${Boost_LIBRARIES}
        helloworld
    )

    target_compile_options(test_version PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

install(TARGETS helloworld_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_BOOST_TEST)
    enable_testing()
    add_test(test_version test_version)
endif()