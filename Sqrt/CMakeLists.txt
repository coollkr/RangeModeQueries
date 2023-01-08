cmake_minimum_required(VERSION 3.10.2)
project(sqrtN)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD 14)

add_executable(sqrtN main.cpp ExactMode_Sqrt.cpp ExactMode_Sqrt.h)