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
    int leaf_id = 1;

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
                if (curr_pos < tokens.size() &&
                    tokens[curr_pos].type == TokenType::RPAREN) {
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
            } else if (token.type == TokenType::LCURLY) {
                parse_range();
            } else if (token.type == TokenType::RCURLY) {
                throw std::runtime_error("Unpaired closing curly brace");
            } else if (token.type == TokenType::STAR) {
                interpret_operator(new StarNode());
            } else if (token.type == TokenType::PLUS) {
                interpret_operator(new PlusNode());
            } else if (token.type == TokenType::QMARK) {
                interpret_operator(new QMarkNode());
            } else if (token.type == TokenType::UNION) {
                interpret_operator(new UnionNode());
            } else if (token.type == TokenType::CHAR ||
                       token.type == TokenType::DIGIT) {
                results.push_back(new CharNode(leaf_id, token.value));
                leaf_id += 1;
            } else if (token.type == TokenType::DOT) {
                results.push_back(new DotNode(leaf_id));
                leaf_id += 1;
            } else if (token.type == TokenType::SMALL_D) {
                results.push_back(new SmallDNode(leaf_id));
                leaf_id += 1;
            } else if (token.type == TokenType::BIG_D) {
                results.push_back(new BigDNode(leaf_id));
                leaf_id += 1;
            } else if (token.type == TokenType::SMALL_S) {
                results.push_back(new SmallSNode(leaf_id));
                leaf_id += 1;
            } else if (token.type == TokenType::BIG_S) {
                results.push_back(new BigSNode(leaf_id));
                leaf_id += 1;
            } else if (token.type == TokenType::SMALL_W) {
                results.push_back(new SmallWNode(leaf_id));
                leaf_id += 1;
            } else if (token.type == TokenType::BIG_W) {
                results.push_back(new BigWNode(leaf_id));
                leaf_id += 1;
            } else if (token.type == TokenType::EMPTY) {
                results.push_back(new EmptyNode(leaf_id));
            } else if (token.type == TokenType::NOTHING) {
                results.push_back(new NothingNode(leaf_id));
                leaf_id += 1;
            } else if (token.type == TokenType::END) {
                drop_operators_until_group();

                if (!stack.empty()) {
                    throw std::runtime_error("Missing closing parenthesis");
                }

                if (results.size() > 1) {
                    throw std::runtime_error("Not enough operators");  // TODO: improve
                }

                auto result = results.back();
                results.pop_back();
                return result;
            } else {
                throw std::runtime_error("Unknown token type");
            }
        }
    }

    static bool can_insert_concat(Token before, Token after) {
        return before_concat(before) && after_concat(after);
    }

    static inline bool before_concat(Token token) {
        return (
            token.type == TokenType::RPAREN ||
            token.type == TokenType::RCURLY ||
            token.type == TokenType::STAR ||
            token.type == TokenType::PLUS ||
            token.type == TokenType::QMARK ||
            token.type == TokenType::CHAR ||
            token.type == TokenType::DIGIT ||
            token.type == TokenType::DOT ||
            token.type == TokenType::EMPTY ||
            token.type == TokenType::SMALL_D ||
            token.type == TokenType::BIG_D ||
            token.type == TokenType::SMALL_S ||
            token.type == TokenType::BIG_S ||
            token.type == TokenType::SMALL_W ||
            token.type == TokenType::BIG_W
        );
    }

    static inline bool after_concat(Token token) {
        return (
            token.type == TokenType::LPAREN ||
            token.type == TokenType::CHAR ||
            token.type == TokenType::DIGIT ||
            token.type == TokenType::DOT ||
            token.type == TokenType::EMPTY ||
            token.type == TokenType::SMALL_D ||
            token.type == TokenType::BIG_D ||
            token.type == TokenType::SMALL_S ||
            token.type == TokenType::BIG_S ||
            token.type == TokenType::SMALL_W ||
            token.type == TokenType::BIG_W
        );
    }

    void parse_range() {
        // TODO: check if values are not starting with 0s
        std::string num1, num2;

        while (tokens[curr_pos].type == TokenType::DIGIT) {
            num1.push_back(tokens[curr_pos].value);
            curr_pos += 1;
        }

        if (tokens[curr_pos].type == TokenType::CHAR && tokens[curr_pos].value == ',') {
            curr_pos += 1;

            while (tokens[curr_pos].type == TokenType::DIGIT) {
                num2.push_back(tokens[curr_pos].value);
                curr_pos += 1;
            }

            if (tokens[curr_pos].type != TokenType::RCURLY) {
                throw std::runtime_error("Unclosed range operator");
            }

            curr_pos += 1;

            if (num1.empty() && num2.empty()) {
                throw std::runtime_error("Incorrect value in range operator");
            } else if (num1.empty()) {
                interpret_operator(new MaxNode(std::stoi(num2)));
            } else if (num2.empty()) {
                interpret_operator(new MinNode(std::stoi(num1)));
            } else {
                interpret_operator(new RangeNode(std::stoi(num1), std::stoi(num2)));
            }
        } else {
            if (num1.empty()) {
                throw std::runtime_error("Incorrect value in range operator");
            } else if (tokens[curr_pos].type != TokenType::RCURLY) {
                throw std::runtime_error("Unclosed range operator");
            }

            curr_pos += 1;
            interpret_operator(new PowerNode{std::stoi(num1)});
        }
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