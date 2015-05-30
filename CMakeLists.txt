project(multi-methods)
cmake_minimum_required(VERSION 2.8)

set(HDR_LIST multimethods.h)
set(SRC_LIST main.cc)

add_executable(${PROJECT_NAME} ${HDR_LIST} ${SRC_LIST})
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall  -Weffc++ -pedantic")

