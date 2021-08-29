#include "ast.h"

namespace pyrex {
    AST::Parser::Token::Token(AST::Parser::TokenType type) : type{type} {}

    AST::Parser::Token::Token(AST::Parser::TokenType type, char value) : type{type}, value{value} {}

    AST::Parser::Parser(const std::string &regex) : regex{regex} {}

    AST::Parser::~Parser() {
        // TODO
    }

    AST AST::Parser::parse() {
        // TODO
    }

    bool AST::Parser::can_insert_concat(Token before, Token after) {
        // TODO
    }

    bool AST::Parser::before_concat(Token token) {
        // TODO
    }

    bool AST::Parser::after_concat(Token token) {
        // TODO
    }

    void AST::Parser::parse_range() {
        // TODO
    }

    void AST::Parser::push_node() {
        // TODO
    }

    void AST::Parser::interpret_operator(Operator *op) {
        // TODO
    }

    void AST::Parser::drop_operators_precedence(int precedence) {
        // TODO
    }

    void AST::Parser::drop_operators_until_group() {
        // TODO
    }
}