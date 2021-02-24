cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(cpp_basics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Catch2 REQUIRED)
option(SAN "Enable Sanitizers" false)

if(SAN STREQUAL true)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

add_library(ra STATIC ./lib/random.cpp)
target_include_directories(ra PUBLIC include)

add_executable(test_random ./app/test_random.cpp)
add_executable(test_rational ./app/test_rational.cpp)

target_link_libraries(test_random ra Catch2::Catch2)
target_link_libraries(test_rational ra Catch2::Catch2)
