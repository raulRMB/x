cmake_minimum_required(VERSION 3.26)

project(util)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE
    file.cpp file.h
    primitives.h
    color.h
    Util.cpp
    Util.h
)

add_library(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE base Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE base Vulkan::Vulkan)