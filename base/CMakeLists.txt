cmake_minimum_required(VERSION 3.26)

project(base)

set (CMAKE_CXX_STANDARD 17)
set (SOURCES defines.h base.c)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC glm ${CMAKE_SOURCE_DIR}/)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)