//
// Created by bercik on 07.08.2021.
//

/* Repetition qualifiers (*, +, ?, {m,n}, etc) cannot be directly nested. */

#include <iostream>
#include <stack>
#include <vector>

struct node {
    int hello;
};

struct op : node {
    std::string kind;
    node *children[];
};

struct leaf {

};

class Parser {
    std::string const &regex;

    explicit Parser(std::string &regex) : regex(regex) {}


};


enum class TokenKind {
    CHAR,
    ESCAPE_CHAR,
    CHARSET_OPEN,
    CHARSET_CLOSE,
    PARENS_OPEN,
    PARENS_CLOSE,
    NCG,
    CG_NAME,
};

struct TokenValue {};

struct Token {
    TokenKind kind;
    std::string value; // co z errorami, tj. informacja, ktory znak jest "zly"?
};


struct RegexTokenizer {
    unsigned long long counter = 0;
    std::string const &regex;

    explicit RegexTokenizer(std::string const &regex) : regex(regex) {}

    Token getNextToken() {
        counter += 1;
        return {};
    }
};


enum class NodeKind {
    CHAR,
    CHARSET,
    PARENS,
    CONCAT,
    UNION,
    INTERSECT,
    MINUS,
    XOR,
    STAR,
    PLUS,
    REPEAT,
    ALTERNATE,
    COMPLEMENT,
    OPTIONAL,
    CASE_INSENSITIVE,
};


struct Node {

};



int tokenize(std::string const &regex) {
    bool in_char_set = false;
    std::vector<token> output;
    std::stack<int> stack;

    for (int i = 0; i < regex.size();) {
        if (regex[i] == '[') {
            if (in_char_set) {
                output.emplace_back(TokenKind::op, '[');
            } else {
                in_char_set = true;
            }
        } else if (regex[i] == ']') {
            if (in_char_set) {
                in_char_set = false;
            } else {

            }
        } else if (regex[i] == '\\') {

        } else if (regex[i] == '(') {

        } else if (regex[i] == ' ') {

        } else if (regex[i] == ' ') {

        } else if (regex[i] == ' ') {

        } else if (regex[i] == ' ') {

        } else if (regex[i] == ' ') {

        } else if (regex[i] == ' ') {

        } else if (regex[i] == ' ') {

        } else if (regex[i] == ' ') {

        } else if (regex[i] == ' ') {

        } else if (regex[i] == ' ') {

        }

    }
}


int parse(std::string const &regex) {
    bool in_char_set = false;
    std::stack<int> hello;

    for (char c : regex) {
        if (c == '+') {

        }
    }
    return 0;
}


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
    std::string a;
    std::cin >> a;
    std::cout << "Hello " << a << std::endl;
    print();
    parse(a);
    return 0;
}
