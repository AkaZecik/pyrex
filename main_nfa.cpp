//
// Created by bercik on 15.08.2021.
//

#include <string>
#include <iostream>
#include <algorithm>
#include "parser.cpp"
#include "nfa.cpp"

int main() {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        Parser parser(input);
        Node *tree = parser.parse();
        NFA nfa = std::move(NFA::from_ast(tree));
        std::cout << "matching:" << std::endl;
        while (true) {
            std::string text;
            std::getline(std::cin, text);

            if (text == "next") {
                break;
            }
            std::cout << text.size() << std::endl;
            std::cout << std::boolalpha << nfa.traverse(text, 0, 0, nullptr) << std::endl;
        }
        std::cout << "---" << std::endl;
    }
#pragma clang diagnostic pop
}