//
// Created by bercik on 13.08.2021.
//

#include <string>
#include <iostream>
#include "tokenizer.cpp"



int main() {
    while (true) {
        std::string regex;
        std::cin >> regex;
        Tokenizer tokenizer(regex);

        try {
            auto tokens = tokenizer.get_all_tokens();

            for (Token token : tokens) {
                switch (token.type) {
                    case TokenType::LPAREN: std::cout << '('; break;
                    case TokenType::RPAREN: std::cout << ')'; break;
                    case TokenType::STAR: std::cout << '*'; break;
                    case TokenType::UNION: std::cout << '|'; break;
                    case TokenType::QMARK: std::cout << '?'; break;
                    case TokenType::CHAR: std::cout << '"' << token.value << '"'; break;
                    case TokenType::END: std::cout << "END"; break;
                }

                std::cout << std::endl;
            }
        } catch (std::runtime_error &e) {
            std::cout << e.what() << std::endl;
        }

        std::cout << std::endl;
    }
}