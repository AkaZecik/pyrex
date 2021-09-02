cmake_minimum_required(VERSION 3.19)
project(pyrex)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        pyrex
        ast.cpp
        dfa.cpp
        ast.h
        ast_node.cpp
        ast_ast.cpp
        ast_parser.cpp
        ast_tokenizer.cpp
        new_regex.cpp
        new_regex.h
        new_regex_nfa.cpp
        new_regex_new_regex.cpp
        new_regex_nfa_match.cpp
)

add_executable(ast_main ast_main.cpp ast.h ast_ast.cpp ast_node.cpp ast_parser.cpp ast_tokenizer.cpp)
