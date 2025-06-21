cmake_minimum_required(VERSION 3.14)
project(Assignments LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(third_party/googletest EXCLUDE_FROM_ALL)

add_subdirectory(mask)