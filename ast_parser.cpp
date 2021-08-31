#include "ast.h"

/* TODO:
 *  - std::stoi might raise
 *  - create custom error classes
 */

namespace pyrex {
    AST::Parser::Token::Token(AST::Parser::TokenType type) : type{type} {}

    AST::Parser::Token::Token(AST::Parser::TokenType type, char value) : type{type}, value{value} {}

    AST::Parser::Parser(const std::string &regex)
        : regex{regex}, curr_pos{0}, concat_insertable{false} {
        Tokenizer tokenizer(regex);
        all_tokens = tokenizer.get_all_tokens();
    }

    AST AST::Parser::parse() {
        while (true) {
            if (can_insert_concat()) {
                interpret_operator(std::make_shared<ConcatNode>(nullptr, nullptr));
            }

            Token token = all_tokens[curr_pos];
            curr_pos += 1;

            if (token.type == TokenType::LPAREN) {
                parse_group();

                if (all_tokens[curr_pos].type == TokenType::RPAREN) {
                    throw std::runtime_error("Empty group");
                } else if (all_tokens[curr_pos].type == TokenType::END) {
                    throw std::runtime_error("Premature end of input");
                }
            } else if (token.type == TokenType::RPAREN) {
                drop_operators_until_group();

                if (stack.empty()) {
                    throw std::runtime_error("Too many closing parenthesis");
                }

                auto group = stack.back();
                stack.pop_back();
                push_node(std::move(group));
            } else if (token.type == TokenType::LCURLY) {
                parse_range();
            } else if (token.type == TokenType::RCURLY) {
                throw std::runtime_error("Unpaired closing curly brace");
            } else if (token.type == TokenType::STAR) {
                interpret_operator(std::make_shared<StarNode>(nullptr));
            } else if (token.type == TokenType::PLUS) {
                interpret_operator(std::make_shared<PlusNode>(nullptr));
            } else if (token.type == TokenType::QMARK) {
                interpret_operator(std::make_shared<QMarkNode>(nullptr));
            } else if (token.type == TokenType::UNION) {
                interpret_operator(std::make_shared<UnionNode>(nullptr, nullptr));
            } else if (token.type == TokenType::PERCENT) {
                interpret_operator(std::make_shared<PercentNode>(nullptr, nullptr));
            } else if (token.type == TokenType::CHAR || token.type == TokenType::DIGIT) {
                results.push_back(AST::for_char(token.value));
            } else if (token.type == TokenType::DOT) {
                results.push_back(AST::for_dot());
            } else if (token.type == TokenType::SMALL_D) {
                results.push_back(AST::for_small_d());
            } else if (token.type == TokenType::SMALL_S) {
                results.push_back(AST::for_small_s());
            } else if (token.type == TokenType::SMALL_W) {
                results.push_back(AST::for_small_w());
            } else if (token.type == TokenType::EMPTY) {
                results.push_back(AST::for_empty());
            } else if (token.type == TokenType::NOTHING) {
                results.push_back(AST::for_nothing());
            } else if (token.type == TokenType::END) {
                drop_operators_until_group();

                if (!stack.empty()) {
                    throw std::runtime_error("Missing closing parenthesis");
                }

                if (results.size() > 1) {
                    throw std::runtime_error("Not enough operators");
                }

                auto result = results.back();
                results.pop_back();
                return result;
            } else {
                throw std::runtime_error("Unknown token type");
            }

            if (token.type == TokenType::LPAREN ||
                token.type == TokenType::UNION ||
                token.type == TokenType::PERCENT) {
                concat_insertable = false;
            } else {
                concat_insertable = true;
            }
        }
    }

    bool AST::Parser::can_insert_concat() {
        return concat_insertable && after_concat(all_tokens[curr_pos]);
    }

    bool AST::Parser::after_concat(Token token) {
        return (
            token.type == TokenType::LPAREN ||
            token.type == TokenType::CHAR ||
            token.type == TokenType::DIGIT ||
            token.type == TokenType::DOT ||
            token.type == TokenType::EMPTY ||
            token.type == TokenType::NOTHING ||
            token.type == TokenType::SMALL_D ||
            token.type == TokenType::SMALL_S ||
            token.type == TokenType::SMALL_W
        );
    }

    void AST::Parser::parse_group() {
        if (all_tokens[curr_pos].type == TokenType::QMARK) {
            curr_pos += 1;

            if (all_tokens[curr_pos].type == TokenType::END) {
                throw std::runtime_error("Premature end of input");
            } else {
                if (all_tokens[curr_pos].type == TokenType::CHAR) {
                    if (all_tokens[curr_pos].value == ':') {
                        curr_pos += 1;
                        stack.push_back(std::make_shared<NonCGroupNode>(nullptr));
                        return;
                    } else if (all_tokens[curr_pos].value == '<') {
                        curr_pos += 1;
                        auto name = parse_identifier();

                        if (all_tokens[curr_pos].type == TokenType::END) {
                            throw std::runtime_error("Premature end of input");
                        } else if (all_tokens[curr_pos].type != TokenType::CHAR ||
                                   all_tokens[curr_pos].value != '>') {
                            throw std::runtime_error("Unterminated name of named group");
                        } else {
                            curr_pos += 1;
                            stack.push_back(std::make_shared<NamedCGroupNode>(nullptr, name));
                        }

                        return;
                    }
                }

                throw std::runtime_error("Unknown group extension");
            }
        } else {
            stack.push_back(std::make_shared<NumberedCGroupNode>(nullptr));
        }
    }

    std::string AST::Parser::parse_identifier() {
        std::string identifier;

        if (all_tokens[curr_pos].type == TokenType::CHAR &&
            identifier_start_char(all_tokens[curr_pos].value)) {
            identifier.push_back(all_tokens[curr_pos].value);
            curr_pos += 1;

            while (all_tokens[curr_pos].type == TokenType::CHAR &&
                   identifier_char(all_tokens[curr_pos].value)) {
                identifier.push_back(all_tokens[curr_pos].value);
                curr_pos += 1;
            }

            return identifier;
        } else {
            throw std::runtime_error("Bad character in named capturing group");
        }
    }

    bool AST::Parser::identifier_start_char(char chr) {
        return 'a' <= chr && chr <= 'z' || 'A' <= chr && chr <= 'Z' || chr == '_';
    }

    bool AST::Parser::identifier_char(char chr) {
        return identifier_start_char(chr) || '0' <= chr && chr <= '9';
    }

    void AST::Parser::parse_range() {
        std::string num1, num2;

        while (all_tokens[curr_pos].type == TokenType::DIGIT) {
            num1.push_back(all_tokens[curr_pos].value);
            curr_pos += 1;
        }

        if (all_tokens[curr_pos].type == TokenType::CHAR && all_tokens[curr_pos].value == ',') {
            curr_pos += 1;

            while (all_tokens[curr_pos].type == TokenType::DIGIT) {
                num2.push_back(all_tokens[curr_pos].value);
                curr_pos += 1;
            }

            if (all_tokens[curr_pos].type != TokenType::RCURLY) {
                throw std::runtime_error("Unclosed range operator");
            }

            curr_pos += 1;

            if (num1.empty() && num2.empty()) {
                throw std::runtime_error("Incorrect range operator");
            } else if (num1.empty()) {
                interpret_operator(std::make_shared<MaxNode>(nullptr, std::stoi(num2)));
            } else if (num2.empty()) {
                interpret_operator(std::make_shared<MinNode>(nullptr, std::stoi(num1)));
            } else {
                auto min = std::stoi(num1);
                auto max = std::stoi(num2);

                if (min > max) {
                    throw std::runtime_error("Max greater than min in range operator");
                }

                interpret_operator(std::make_shared<RangeNode>(nullptr, min, max));
            }
        } else {
            if (num1.empty()) {
                throw std::runtime_error("Incorrect range operator");
            } else if (all_tokens[curr_pos].type != TokenType::RCURLY) {
                throw std::runtime_error("Unclosed range operator");
            }

            curr_pos += 1;
            interpret_operator(std::make_shared<PowerNode>(nullptr, std::stoi(num1)));
        }
    }

    void AST::Parser::push_node(std::shared_ptr<InternalNode> &&internal_node) {
        if (internal_node->arity() > results.size()) {
            throw std::runtime_error("Too little operands");
        }

        if (internal_node->internal_node_type() == AST::InternalNode::Type::GROUP) {
            auto group = std::dynamic_pointer_cast<GroupNode>(internal_node);
            auto ast = results.back();
            results.pop_back();
            group->operand = ast.root;
            results.push_back(AST(group));
        } else {
            auto op = std::dynamic_pointer_cast<Operator>(internal_node);

            if (op->arity() == 1) {
                auto uop = std::dynamic_pointer_cast<UnaryOperator>(op);
                auto ast = results.back();
                results.pop_back();
                uop->operand = ast.root;
                results.push_back(AST(std::move(uop)));
            } else {
                auto bop = std::dynamic_pointer_cast<BinaryOperator>(op);
                auto right_ast = results.back();
                results.pop_back();
                auto left_ast = results.back();
                results.pop_back();
                bop->left_operand = left_ast.root;
                bop->right_operand = right_ast.root;
                results.push_back(AST(std::move(bop)));
            }
        }
    }

    void AST::Parser::interpret_operator(std::shared_ptr<Operator> &&op) {
        drop_operators_precedence(op->precedence());

        if (op->arity() == 1) {
            push_node(op);
        } else {
            stack.push_back(op);
        }
    }

    void AST::Parser::drop_operators_precedence(int precedence) {
        while (!stack.empty() &&
               stack.back()->internal_node_type() == InternalNode::Type::OPERATOR) {
            auto op = std::dynamic_pointer_cast<Operator>(stack.back());

            if (op->precedence() <= precedence) {
                stack.pop_back();
                push_node(op);
            } else {
                break;
            }
        }
    }

    void AST::Parser::drop_operators_until_group() {
        while (!stack.empty() && stack.back()->internal_node_type() != InternalNode::Type::GROUP) {
            auto op = std::dynamic_pointer_cast<Operator>(stack.back());
            stack.pop_back();
            push_node(op);
        }
    }
}