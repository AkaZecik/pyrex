cmake_minimum_required(VERSION 3.19)
project(pyrex)

set(CMAKE_CXX_STANDARD 14)

add_executable(general_pyrex general_tokenizer.cpp general_parser.cpp regex.cpp)
add_executable(kalkulator kalkulator.cpp)
add_executable(simplified simplified.cpp)
add_executable(pyrex tokenizer.cpp main_tokenizer.cpp)