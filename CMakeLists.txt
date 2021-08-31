cmake_minimum_required(VERSION 3.19)
project(pyrex)

set(CMAKE_CXX_STANDARD 17)

add_executable(pyrex tokenizer.cpp parser.cpp ast.cpp nfa.cpp dfa.cpp ast.h ast_node.cpp ast_ast.cpp ast_parser.cpp ast_tokenizer.cpp new_regex.cpp new_regex.h)
add_executable(main_parser main_parser.cpp)
add_executable(main_nfa main_nfa.cpp)
add_executable(main_dfa main_dfa.cpp)
add_executable(regex regex.cpp regex.h)

add_executable(ast_main ast_main.cpp ast.h ast_ast.cpp ast_node.cpp ast_parser.cpp ast_tokenizer.cpp)
