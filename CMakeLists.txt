cmake_minimum_required(VERSION 3.26)
set(CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "CMake generator" FORCE)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(x)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif (WIN32)

add_subdirectory(vendor)
add_subdirectory(src)