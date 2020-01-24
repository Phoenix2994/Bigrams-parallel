cmake_minimum_required(VERSION 3.15)
project(bigrams_parallel)

set(CMAKE_CXX_STANDARD 11 )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")

set(SOURCE_FILES main.cpp grams_computing.h grams_computing.cpp)
add_executable(bigrams_parallel ${SOURCE_FILES})