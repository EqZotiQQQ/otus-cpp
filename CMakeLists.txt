cmake_minimum_required(VERSION 3.28.3)
project(otus_hws)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CPACK_PACKAGE_NAME "async")
set(CPACK_PACKAGE_VENDOR "otus")
set(CPACK_PACKAGE_CONTACT "Miko <mf_52@mail.ru>")

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

# Debian требует maintainer
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <you@example.com>")

include(CTest)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)
# add_subdirectory(otus_hws/hw1)
# add_subdirectory(otus_hws/hw2)
# add_subdirectory(otus_hws/hw3)
# add_subdirectory(otus_hws/hw4)
# add_subdirectory(otus_hws/hw5)
# add_subdirectory(otus_hws/hw6)
# add_subdirectory(otus_hws/hw7)
# add_subdirectory(otus_hws/hw8)
add_subdirectory(otus_hws/hw9)

# add_subdirectory(experiments)
# add_subdirectory(problems)

