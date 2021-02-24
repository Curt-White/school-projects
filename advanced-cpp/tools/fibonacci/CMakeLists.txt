cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(fibonacci VERSION 1.0 LANGUAGES CXX)

add_executable(fibonacci main.cpp fibonacci.cpp fibonacci.hpp)
