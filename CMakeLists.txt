cmake_minimum_required(VERSION 3.19)
project(pyrex)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        pyrex
        main.cpp
        ast.h
        ast.cpp
        ast_node.cpp
        ast_parser.cpp
        ast_tokenizer.cpp
        regex.h
        regex.cpp
        regex_nfa.cpp
        regex_nfa_match.cpp
)

add_executable(ast_main ast_main.cpp ast.h ast.cpp ast_node.cpp ast_parser.cpp ast_tokenizer.cpp)
