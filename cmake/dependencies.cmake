include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.1.0)

set(FMT_BUILD_SHARED
    OFF
    CACHE BOOL "" FORCE)
set(FMT_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(FMT_BUILD_DOCS
    OFF
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0)

set(SPDLOG_FMT_EXTERNAL
    ON
    CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED
    OFF
    CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE
    OFF
    CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH
    OFF
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG v25.3)

set(protobuf_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(protobuf_BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "" FORCE)
set(protobuf_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(protobuf)
