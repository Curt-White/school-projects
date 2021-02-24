cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(buggy VERSION 1.0 LANGUAGES CXX)
include(Sanitizers.cmake)

set(CAN_SANITIZE "Clang" "GNU")
list(FIND CAN_SANITIZE "${CMAKE_CXX_COMPILER_ID}" _index)
# assignment only assumes flags valid for GCC and Clang
if(${_index} GREATER -1)
    set(OPT_FLAGS "")

    if(ENABLE_ASAN)
        set(OPT_FLAGS "-fsanitize=address")
    endif()

    if(ENABLE_UBSAN)
        set(OPT_FLAGS "${OPT_FLAGS} -fsanitize=undefined")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPT_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OPT_FLAGS}")
endif()

add_executable(buggy buggy.cpp)