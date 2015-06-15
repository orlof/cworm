cmake_minimum_required(VERSION 3.2)
project(worm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    array.c
    array.h
    interpreter.c
    interpreter.h
    memory.c
    memory.h
    memory_test.c
    minunit.h)

add_executable(worm ${SOURCE_FILES} interpreter.h interpreter.c)