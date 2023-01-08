
cmake_minimum_required(VERSION 3.10.2)
project(PST)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD 14)
set(SDSL_INCLUDE_DIR /users/grad/liu1/local/include)
set(SDSL_LIBRARY /users/grad/liu1/local/lib)
find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)

include_directories (${GTK2_INCLUDE_DIRS})
include_directories(${SDSL_INCLUDE_DIR})
link_directories(${SDSL_LIBRARY})
link_libraries(/users/grad/liu1/sdsl-lite)
link_directories (${GTK2_LIBRARY_DIRS})

add_executable(PST main.cpp adds/ptree.c adds/ptree.h ApproxMode_PST.cpp ApproxMode_PST.h)

add_definitions (${GTK2_CFLAGS_OTHER})
target_link_libraries(PST PUBLIC sdsl ${GTK2_LIBRARIES})
SET_TARGET_PROPERTIES(PST PROPERTIES linker_language cxx)




target_include_directories(PST PUBLIC ${SDSL_INCLUDE_DIR})



