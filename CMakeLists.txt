cmake_minimum_required(VERSION 3.6.4)

# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(MilkTeaPot)

add_subdirectory(MilkTea)
add_subdirectory(MilkPowder)
add_subdirectory(Milk)
