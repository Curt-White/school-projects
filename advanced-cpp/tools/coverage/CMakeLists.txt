cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(coverage VERSION 1.0 LANGUAGES CXX)

option(ENABLE_COVERAGE "Use lcov code coverage analysis" false)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

if(ENABLE_COVERAGE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "set build type debug" FORCE)
    include(CodeCoverage.cmake)
    APPEND_COVERAGE_COMPILER_FLAGS()
endif()

add_executable(random random.cpp)

if(ENABLE_COVERAGE)
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME coverage
        EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/run_tests"
        EXECUTABLE_ARGS "${CMAKE_CURRENT_BINARY_DIR}"
        LCOV_ARGS --rc loc_branch_coverage=1
        GENHTML_ARGS --branch-coverage -o "${CMAKE_CURRENT_BINARY_DIR}/coverage"
        DEPENDENCIES random
    )
endif()
