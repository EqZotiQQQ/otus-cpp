cmake_minimum_required(VERSION 3.28.3)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)

if (DEFINED ENV{GITHUB_RUN_NUMBER})
    set(PROJECT_VERSION_PATCH $ENV{GITHUB_RUN_NUMBER})
elseif (DEFINED PATCH_VERSION)
    set(PROJECT_VERSION_PATCH ${PATCH_VERSION})
else()
    set(PROJECT_VERSION_PATCH 1)
endif()

project(otus_hws VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CPACK_PACKAGE_NAME "async")
set(CPACK_PACKAGE_VENDOR "otus")
set(CPACK_PACKAGE_CONTACT "Miko <mf_52@mail.ru>")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

set(CPACK_PACKAGE_FILE_NAME "async-${PROJECT_VERSION}-Linux")
set(CPACK_DEBIAN_FILE_NAME "async-${PROJECT_VERSION}-Linux")

set(CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Miko <mf_52@mail.ru>")
set(CPACK_DEBIAN_PACKAGE_NAME "async")

set(CPACK_GENERATOR DEB)
include(CPack)
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

