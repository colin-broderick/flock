cmake_minimum_required(VERSION 3.22)
project(flock)
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} source/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -g -Wextra -Wpedantic)
target_link_libraries(${PROJECT_NAME} sfml-system sfml-graphics sfml-window)
