cmake_minimum_required(VERSION 3.19)
project(pyrex)

set(CMAKE_CXX_STANDARD 14)

add_executable(pyrex main.cpp)
add_executable(parser tokenizer.cpp parser.cpp)
add_executable(kalkulator kalkulator.cpp)