



cmake_minimum_required(VERSION 3.10.2)
project(1BitPlus)

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

add_executable(1BitPlus main.cpp ApproxMode_Sample_Plain.cpp ApproxMode_Sample_Plain.h)

target_link_libraries(1BitPlus PUBLIC sdsl)
SET_TARGET_PROPERTIES(1BitPlus PROPERTIES linker_language cxx)
target_include_directories(1BitPlus PUBLIC ${SDSL_INCLUDE_DIR})