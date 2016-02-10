cmake_minimum_required(VERSION 3.2)
project(worm)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES
    array.c
    array.h
    array_test.c
    array_test.h
    dict.c
    dict.h
    float.c
    float.h
    integer.c
    integer.h
    interpreter.c
    interpreter.h
    memory.c
    memory.h
    memory_test.c
    memory_test.h
    minunit.c
    minunit.h
    stdutil.c
    stdutil.h
    stdutil_test.c
    stdutil_test.h
    string.c
    string.h)

add_executable(worm ${SOURCE_FILES} minunit.c)
