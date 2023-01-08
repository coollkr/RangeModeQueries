


cmake_minimum_required(VERSION 3.10.2)
project(RrrExactMode2)

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

add_executable(RrrExactMode2 main.cpp ExactMode_Subsr2_Compressed.cpp ExactMode_Subsr2_Compressed.h)


target_link_libraries(RrrExactMode2 PUBLIC sdsl)
SET_TARGET_PROPERTIES(RrrExactMode2 PROPERTIES linker_language cxx)
target_include_directories(RrrExactMode2 PUBLIC ${SDSL_INCLUDE_DIR})