cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(cpp_basics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Catch2 REQUIRED)
option(SAN "Enable Sanitizers" false)

find_package(Boost REQUIRED)

if (SAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(EXTRA_COMPILE_FLAGS "-fconstexpr-loop-limit=10000")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang.*")
    set(EXTRA_COMPILE_FLAGS "-fconstexpr-steps=1000000000")
endif()

set_source_files_properties(app/test_mandelbrot.cpp PROPERTIES COMPILE_FLAGS ${EXTRA_COMPILE_FLAGS})

add_library(ra INTERFACE)
target_include_directories(ra INTERFACE include)

add_executable(test_cexpr_basic_string ./app/test_cexpr_basic_string.cpp)
add_executable(test_mandelbrot ./app/test_mandelbrot.cpp)

add_executable(test_cexpr_math ./app/test_cexpr_math.cpp)
target_include_directories(test_cexpr_math PUBLIC ${Boost_INCLUDE_DIR})

add_executable(test_biquad_filter ./app/test_biquad_filter.cpp)

target_link_libraries(test_cexpr_basic_string ra Catch2::Catch2)
target_link_libraries(test_mandelbrot ra Catch2::Catch2)
target_link_libraries(test_cexpr_math ra Catch2::Catch2 Boost::headers)
target_link_libraries(test_biquad_filter ra Catch2::Catch2)