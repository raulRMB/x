cmake_minimum_required(VERSION 3.26)

project(base)

set (CMAKE_CXX_STANDARD 17)
set (SOURCES defines.h base.c)

add_library(${PROJECT_NAME} ${SOURCES})