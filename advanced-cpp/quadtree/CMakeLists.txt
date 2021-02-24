cmake_minimum_required(VERSION 3.1) 
project(cpp_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Catch2 REQUIRED)
find_package(Boost REQUIRED)

add_library(sim ./src/shapes.cpp ./src/window.cpp)
target_include_directories(sim PUBLIC include PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(sim ${Boost_LIBRARY_DIRS})

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(main main.cpp)
target_link_libraries(main sim ${CURSES_LIBRARIES} Boost::boost)

install(TARGETS main DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(PROGRAMS demo DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

add_executable(test ./test/test.cpp)
target_link_libraries(test sim ${CURSES_LIBRARIES} Catch2::Catch2 Boost::boost)
