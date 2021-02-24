cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(timer VERSION 1.0 LANGUAGES CXX)

set(Boost_USE_MULTITHREADED false)
find_package(Boost REQUIRED timer)

add_executable(timer timer.cpp fibonacci.cpp fibonacci.hpp)

# link the boost contents to the executable target
target_include_directories(timer PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(timer ${Boost_LIBRARIES})

# could also use imported targets as follows
# target_link_libraries(timer Boost::timer)