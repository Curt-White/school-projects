cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(cpp_basics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Catch2 REQUIRED)
find_package(CGAL REQUIRED)
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math")
endif()

add_library(ra INTERFACE)
target_include_directories(ra INTERFACE include)

add_executable(test_interval ./app/test_interval.cpp)
target_link_libraries(test_interval ra Catch2::Catch2)

add_executable(test_kernel ./app/test_kernel.cpp)
include_directories(${CGAL_INCLUDE_DIRS})
target_link_libraries(test_kernel ra Catch2::Catch2 ${CGAL_LIBRARY} ${GMP_LIBRARIES})

add_executable(delaunay_triangulation ./app/delaunay_triangulation.cpp)
include_directories(app ${CGAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(delaunay_triangulation ra ${CGAL_LIBRARY} ${GMP_LIBRARIES} ${Boost_LIBRARY_DIRS})

if(ENABLE_COVERAGE)
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME coverage
        EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/run_tests"
        EXECUTABLE_ARGS "${CMAKE_CURRENT_BINARY_DIR}"
        LCOV_ARGS --rc loc_branch_coverage=1
        GENHTML_ARGS --branch-coverage -o "${CMAKE_CURRENT_BINARY_DIR}/coverage"
        DEPENDENCIES test_interval test_kernel
    )
endif()


