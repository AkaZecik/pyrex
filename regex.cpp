//
// Created by bercik on 10.08.2021.
//




#include <string>
#include "general_parser.cpp"

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


int print() {
    std::string a[] = {
        "abc", "ab|cd", "a(ba)*", "(a)+a*(a+)+", "[a-cx-z]abc", "((a))((b)|c)",
        };
    for (std::string const &b : a) {
        std::cout << b << std::endl;
    }
    return 0;
}


int main(int argc, char **argv) {
    std::string input;
    std::cin >> input;
    std::cout << "Hello " << input << std::endl;
    print();
    Regex regex(input);
    return 0;
}