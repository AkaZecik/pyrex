//
// Created by bercik on 10.08.2021.
//




#include <string>
#include "parser.cpp"

/************************************************
 *                    Parser                     *
 ************************************************/

struct Regex {
    Node *ast;

    explicit Regex(std::string const &regex) {
        Parser parser(regex);
        ast = parser.parse();
    }
};