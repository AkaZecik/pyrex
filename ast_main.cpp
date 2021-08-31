#include <iostream>
#include "ast.h"

int main() {
    std::cout << std::boolalpha;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        std::string regex;
        std::getline(std::cin, regex);
//        std::cout << "len(\"" << regex << "\") == " << regex.size() << std::endl;

        try {
            pyrex::AST ast = pyrex::AST::from_regex(regex);
            auto to_string = ast.to_string();
            std::cout << to_string << std::endl;
            std::cout << (regex == to_string) << std::endl;
        } catch (std::runtime_error &e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }

        std::cout << std::endl;
    }
#pragma clang diagnostic pop
}