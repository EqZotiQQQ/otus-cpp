cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(ip_printer VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(ip_printer src/main.cpp)
add_library(ip_printer_lib src/ip_printer.cpp)
target_link_libraries(ip_printer PRIVATE ip_printer_lib)

message(STATUS "ip_printer will use C++ standard: ${STD}")

target_include_directories(ip_printer PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(ip_printer_lib PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_options(ip_printer PRIVATE
    -Wall -Wextra -pedantic -Werror
)
target_compile_options(ip_printer PRIVATE
    -Wall -Wextra -pedantic -Werror
)

# if(WITH_BOOST_TEST)
#     enable_testing()

#     find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)

#     add_executable(test_ip_printer tests/test_ip_printer.cpp)

#     target_include_directories(
#         test_ip_printer
#         PRIVATE
#             "${CMAKE_CURRENT_SOURCE_DIR}/include"
#     )

#     target_link_libraries(test_ip_printer PRIVATE Boost::unit_test_framework ip_printer_lib)

#     target_compile_options(test_ip_printer PRIVATE
#         -Wall -Wextra -pedantic -Werror
#     )

#     add_test(
#         NAME test
#         COMMAND $<TARGET_FILE:test_ip_printer>
#     )
# endif()

install(TARGETS ip_printer RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
