cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(cpp_basics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Catch2 REQUIRED)
find_package(Boost REQUIRED)

option(SAN "Enable Sanitizers" false)
option(ENABLE_COVERAGE "Use lcov code coverage analysis" false)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

if(ENABLE_COVERAGE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "set build type debug" FORCE)
    include(CodeCoverage.cmake)
    APPEND_COVERAGE_COMPILER_FLAGS()
endif()

if (SAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -fsanitize=leak")
endif()

add_library(ra INTERFACE)
target_include_directories(ra INTERFACE include)

add_executable(test_sv_set ./app/test_sv_set.cpp)
target_link_libraries(test_sv_set ra Catch2::Catch2)


add_executable(test_intrusive_list ./app/test_intrusive_list.cpp)
target_link_libraries(test_intrusive_list ra Catch2::Catch2 ${Boost_LIBRARIES})
target_include_directories(test_intrusive_list PUBLIC ${Boost_INCLUDE_DIRS})

if(ENABLE_COVERAGE)
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME coverage
        EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/run_tests"
        EXECUTABLE_ARGS "${CMAKE_CURRENT_BINARY_DIR}"
        LCOV_ARGS --rc loc_branch_coverage=1
        GENHTML_ARGS --branch-coverage -o "${CMAKE_CURRENT_BINARY_DIR}/coverage"
        DEPENDENCIES test_sv_set
    )
endif()
