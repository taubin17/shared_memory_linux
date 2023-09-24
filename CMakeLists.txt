cmake_minimum_required(VERSION 3.10.2)
project(shared_memory_linux)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(cpp_essentials)

file(GLOB sources "*.cpp")

add_executable(shared_memory_test ${sources})