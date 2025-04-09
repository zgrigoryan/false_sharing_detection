cmake_minimum_required(VERSION 3.16)
project(false_sharing_detection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output executable
add_executable(main main.cpp)
