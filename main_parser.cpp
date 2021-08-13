//
// Created by bercik on 14.08.2021.
//

#include <string>
#include <iostream>
#include "ast.cpp"
#include "parser.cpp"

int main() {
    while (true) {
        std::string regex;
        std::getline(std::cin, regex);
        std::cout << regex.size() << std::endl;
        Parser parser(regex);

        try {
            Node *ast = parser.parse();
            std::cout << ast->to_string() << std::endl;
        } catch (std::runtime_error &e) {
            std::cout << e.what() << std::endl;
        }

        std::cout << std::endl;
    }
}