cmake_minimum_required(VERSION 3.10.2)
project(Heplus)

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

add_executable(Heplus main.cpp ApproxMode_Succ_Compressed.cpp ApproxMode_Succ_Compressed.h)

target_link_libraries(Heplus PUBLIC sdsl)
SET_TARGET_PROPERTIES(Heplus PROPERTIES linker_language cxx)
target_include_directories(Heplus PUBLIC ${SDSL_INCLUDE_DIR})