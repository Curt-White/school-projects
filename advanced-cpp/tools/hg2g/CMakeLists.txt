cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(hg2g VERSION 1.0 LANGUAGES CXX)

# create a library with contents of lib
add_library(hlib STATIC ./lib/answer.cpp ./lib/question.cpp)
target_include_directories(hlib PUBLIC include)

# create executable and link libs
add_executable(answer ./src/answer.cpp)
target_link_libraries(answer hlib)
