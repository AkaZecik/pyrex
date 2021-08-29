#include "ast.h"

namespace pyrex {
    AST::Parser::Tokenizer::Tokenizer(const std::string &regex) : regex{regex}, curr_pos{0} {}

    std::vector<AST::Parser::Token> AST::Parser::Tokenizer::get_all_tokens() {
        // TODO
    }

    AST::Parser::Token AST::Parser::Tokenizer::get_token() {
        // TODO
    }

    AST::Parser::Token AST::Parser::Tokenizer::parse_escape() {
        // TODO
    }

    AST::Parser::Token AST::Parser::Tokenizer::parse_hex() {
        // TODO
    }
}