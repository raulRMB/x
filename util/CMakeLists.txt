cmake_minimum_required(VERSION 3.26)

project(util)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE
    file.cpp file.h
    primitives.h
    color.h
        LoopedList.h
)

add_library(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE base)
target_link_libraries(${PROJECT_NAME} PRIVATE base)