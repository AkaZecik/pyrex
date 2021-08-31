#include "ast.h"

namespace pyrex {
    AST::Parser::Tokenizer::Tokenizer(const std::string &regex) : regex{regex}, curr_pos{0} {}

    std::vector<AST::Parser::Token> AST::Parser::Tokenizer::get_all_tokens() {
        std::vector<Token> tokens;

        while (true) {
            Token token = get_token();
            tokens.push_back(token);

            if (token.type == TokenType::END) {
                return tokens;
            }
        }
    }

    AST::Parser::Token AST::Parser::Tokenizer::get_token() {
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
            case '{':
                return Token(TokenType::LCURLY);
            case '}':
                return Token(TokenType::RCURLY);
            case '*':
                return Token(TokenType::STAR);
            case '+':
                return Token(TokenType::PLUS);
            case '|':
                return Token(TokenType::UNION);
            case '?':
                return Token(TokenType::QMARK);
            case '.':
                return Token(TokenType::DOT);
            case '\\':
                return parse_escape();
            default:
                if ('0' <= c && c <= '9') {
                    return {TokenType::DIGIT, c};
                } else {
                    return {TokenType::CHAR, c};
                }
        }
    }

    AST::Parser::Token AST::Parser::Tokenizer::parse_escape() {
        if (curr_pos == regex.size()) {
            throw std::runtime_error("Premature accepting of regex");
        }

        char c = regex[curr_pos];
        curr_pos += 1;

        switch (c) {
            case '#':
                return Token{TokenType::NOTHING};
            case 'e':
                return Token{TokenType::EMPTY};
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
            case '+':
                return {TokenType::CHAR, '+'};
            case '|':
                return {TokenType::CHAR, '|'};
            case '?':
                return {TokenType::CHAR, '?'};
            case '{':
                return {TokenType::CHAR, '{'};
            case '}':
                return {TokenType::CHAR, '}'};
            case '.':
                return {TokenType::CHAR, '.'};
            case 'd':
                return Token{TokenType::SMALL_D};
            case 's':
                return Token{TokenType::SMALL_S};
            case 'w':
                return Token{TokenType::SMALL_W};
            case 'x':
                return {TokenType::CHAR, parse_hex()};
            default:
                throw std::runtime_error("Invalid escape");
        }
    }

    char AST::Parser::Tokenizer::parse_hex() {
        if (curr_pos + 2 > regex.size()) {
            throw std::runtime_error("Premature accepting of regex");
        }

        int value = std::stoi(regex.substr(curr_pos, 2), nullptr, 16);
        curr_pos += 2;
        return static_cast<char>(value);
    }
}