
cmake_minimum_required(VERSION 3.10.2)
project(1plus)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD 14)





set(SDSL_INCLUDE_DIR /users/grad/liu1/local/include)
set(SDSL_LIBRARY /users/grad/liu1/local/lib)
include_directories(${SDSL_INCLUDE_DIR})
link_directories(${SDSL_LIBRARY})
link_libraries(/users/grad/liu1/sdsl-lite)

add_executable(1plus main.cpp ApproxMode_Sample_Compressed.cpp ApproxMode_Sample_Compressed.h)

target_link_libraries(1plus PUBLIC sdsl)
SET_TARGET_PROPERTIES(1plus PROPERTIES linker_language cxx)
target_include_directories(1plus PUBLIC ${SDSL_INCLUDE_DIR})