cmake_minimum_required(VERSION 3.2)
project(worm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    array.c
    array.h
    array_test.c
    array_test.h
    interpreter.c
    interpreter.h
    memory.c
    memory.h
    memory_test.c
    memory_test.h
    minunit.c
    minunit.h)

add_executable(worm ${SOURCE_FILES})