//
// Created by bercik on 13.08.2021.
//

#ifndef TOKENIZER_CPP
#define TOKENIZER_CPP

#include <string>
#include <iostream>
#include <utility>
#include <vector>

enum class TokenType {
    LPAREN,
    RPAREN,
    STAR,
    UNION,
    QMARK,
    CHAR,
    END,
};

struct Token {
    TokenType type;
    char value{};

    explicit Token(TokenType type) : type(type) {}

    Token(TokenType type, char value) : type(type), value(value) {}
};

struct Tokenizer {
    std::string regex;
    long long curr_pos = 0;

    explicit Tokenizer(std::string regex) : regex(std::move(regex)) {}

    std::vector<Token> get_all_tokens() {
        std::vector<Token> tokens;

        while (true) {
            Token token = get_token();
            tokens.push_back(token);

            if (token.type == TokenType::END) {
                return tokens;
            }
        }
    }

    Token get_token() {
        if (curr_pos == regex.size()) {
            return Token(TokenType::END);
        }

        char c = regex[curr_pos];
        curr_pos += 1;

        switch (c) {
            case '(':
                return Token(TokenType::LPAREN);
            case ')':
                return Token(TokenType::RPAREN);
            case '*':
                return Token(TokenType::STAR);
            case '|':
                return Token(TokenType::UNION);
            case '?':
                return Token(TokenType::QMARK);
            case '\\':
                return parse_escape();
            default:
                return {TokenType::CHAR, c};
        }
    }

    Token parse_escape() {
        if (curr_pos == regex.size()) {
            throw std::runtime_error("Premature end of regex");
        }

        char c = regex[curr_pos];
        curr_pos += 1;

        switch (c) {
            case 'n':
                return {TokenType::CHAR, '\n'};
            case 'r':
                return {TokenType::CHAR, '\r'};
            case 'f':
                return {TokenType::CHAR, '\f'};
            case 't':
                return {TokenType::CHAR, '\t'};
            case '\\':
                return {TokenType::CHAR, '\\'};
            case '(':
                return {TokenType::CHAR, '('};
            case ')':
                return {TokenType::CHAR, ')'};
            case '*':
                return {TokenType::CHAR, '*'};
            case '|':
                return {TokenType::CHAR, '|'};
            case '?':
                return {TokenType::CHAR, '?'};
            case 'x':
                return {TokenType::CHAR, parse_hex()};
            default:
                throw std::runtime_error("Invalid escape");
        }
    }

    char parse_hex() {
        if (curr_pos + 2 > regex.size()) {
            throw std::runtime_error("Premature end of regex");
        }

        int value = std::stoi(regex.substr(curr_pos, 2), nullptr, 16);
        curr_pos += 2;
        return static_cast<char>(value);
    }
};

#endif // TOKENIZER_CPP