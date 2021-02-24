cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(cgal_in_circle VERSION 1.0 LANGUAGES CXX)

find_package(CGAL REQUIRED)

# if compiler is gcc then add the math rounding flag
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math")
endif()

add_library(utils STATIC utility.cpp utility.hpp)

include_directories(${CGAL_INCLUDE_DIRS})
link_libraries(utils ${CGAL_LIBRARY} ${GMP_LIBRARIES})

add_executable(in_circle in_circle.cpp)
add_executable(in_sphere in_sphere.cpp)