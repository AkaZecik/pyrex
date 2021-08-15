//
// Created by bercik on 13.08.2021.
//

#ifndef PARSER_CPP
#define PARSER_CPP

#include <string>
#include <utility>
#include "tokenizer.cpp"
#include "ast.cpp"

struct Parser {
    std::vector<Token> tokens;
    std::vector<Node *> results;
    std::vector<InternalNode *> stack;
    long long curr_pos = 0;
    int group_id = 1;
    int char_id = 1;

    explicit Parser(std::string regex) {
        Tokenizer tokenizer(std::move(regex));
        tokens = tokenizer.get_all_tokens();
    }

    ~Parser() {
        for (auto node : results) {
            delete node;
        }

        for (auto internal_node : stack) {
            delete internal_node;
        }
    }

    Node *parse() {
        while (true) {
            if (curr_pos > 0 &&
                can_insert_concat(tokens[curr_pos - 1], tokens[curr_pos])) {
                interpret_operator(new ConcatNode());  // might throw, deconstruct it!
            }

            Token token = tokens[curr_pos];
            curr_pos += 1;

            if (token.type == TokenType::LPAREN) {
                if (curr_pos < tokens.size() && tokens[curr_pos].type == TokenType::RPAREN) {
                    throw std::runtime_error("Empty group");
                }

                stack.push_back(new GroupNode(group_id));
                group_id += 1;
            } else if (token.type == TokenType::RPAREN) {
                drop_operators_until_group();

                if (stack.empty()) {
                    throw std::runtime_error("Too many closing parenthesis");
                }

                InternalNode *group = stack.back();
                stack.pop_back();
                push_node(group);
            } else if (token.type == TokenType::STAR) {
                interpret_operator(new StarNode());
            } else if (token.type == TokenType::QMARK) {
                interpret_operator(new QMarkNode());
            } else if (token.type == TokenType::UNION) {
                interpret_operator(new UnionNode());
            } else if (token.type == TokenType::CHAR) {
                results.push_back(new CharNode(char_id, token.value));
                char_id += 1;
            } else if (token.type == TokenType::END) {
                drop_operators_until_group();

                if (!stack.empty()) {
                    throw std::runtime_error("Missing closing parenthesis");
                }

                if (results.size() > 1) {
                    throw std::runtime_error("Not enough operators");  // TODO: improve
                }

                return results.back();
            }
        }
    }

    static bool can_insert_concat(Token before, Token after) {
        return before_concat(before) && after_concat(after);
    }

    static inline bool before_concat(Token token) {
        return (
            token.type == TokenType::RPAREN ||
            token.type == TokenType::STAR ||
            token.type == TokenType::QMARK ||
            token.type == TokenType::CHAR
        );
    }

    static inline bool after_concat(Token token) {
        return (
            token.type == TokenType::LPAREN ||
            token.type == TokenType::CHAR
        );
    }

    void push_node(InternalNode *node) {
        if (node->arity() > results.size()) {
            delete node;
            throw std::runtime_error("Too little operands!");  // TODO: improve
        }

        if (node->internal_node_type() == InternalNode::Type::GROUP) {
            auto group = reinterpret_cast<GroupNode *>(node);
            Node *operand = results.back();
            results.pop_back();
            group->operand = operand;
        } else {
            auto op = reinterpret_cast<Operator *>(node);

            if (op->arity() == 1) {
                auto uop = reinterpret_cast<UnaryOperator *>(op);
                Node *operand = results.back();
                results.pop_back();
                uop->operand = operand;
            } else {
                auto bop = reinterpret_cast<BinaryOperator *>(op);
                Node *right_operand = results.back();
                results.pop_back();
                Node *left_operand = results.back();
                results.pop_back();
                bop->left_operand = left_operand;
                bop->right_operand = right_operand;
            }
        }

        results.push_back(node);
    }

    // assumes unary operators are only right-hand-side unary operators
    void interpret_operator(Operator *op) {
        drop_operators_precedence(op->precedence());

        if (op->arity() == 1) {
            push_node(op);
        } else {
            stack.push_back(op);
        }
    }

    void drop_operators_precedence(int precedence) {
        while (!stack.empty() &&
               stack.back()->internal_node_type() == InternalNode::Type::OPERATOR) {
            auto op = reinterpret_cast<Operator *>(stack.back());

            if (op->precedence() <= precedence) {
                stack.pop_back();
                push_node(op);
            } else {
                break;
            }
        }
    }

    void drop_operators_until_group() {
        while (!stack.empty() &&
        stack.back()->internal_node_type() != InternalNode::Type::GROUP) {
            auto op = reinterpret_cast<Operator *>(stack.back());
            stack.pop_back();
            push_node(op);
        }
    }
};

#endif // PARSER_CPP