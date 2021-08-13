//
// Created by bercik on 13.08.2021.
//

#include <string>
#include <utility>
#include "tokenizer.cpp"

struct Node {
    virtual ~Node() = default;
};

struct LeafNode : Node {
};

struct CharNode : LeafNode {
    char c;
};

struct InternalNode : Node {
    enum class Type {
        GROUP, OPERATOR,
    };

    virtual Type internal_node_type() = 0;
};

struct Group : InternalNode {
    int number;

    InternalNode::Type internal_node_type() override {
        return InternalNode::Type::GROUP;
    }
};

struct Operator : InternalNode {
    enum class Type {
        STAR, QMARK, CONCAT, UNION,
    };

    virtual int arity() = 0;

    virtual int precedence() = 0;

    virtual Type operator_type() = 0;

    InternalNode::Type internal_node_type() override {
        return InternalNode::Type::OPERATOR;
    }
};

struct UnaryOperator : Operator {
    Node *operand = nullptr;

    int arity() override {
        return 1;
    }
};

struct BinaryOperator : Operator {
    Node *left_operand = nullptr;
    Node *right_operand = nullptr;

    int arity() override {
        return 2;
    }
};

struct StarNode : UnaryOperator {
    int precedence() override {
        return 1;
    }

    Operator::Type operator_type() override {
        return Operator::Type::STAR;
    }
};

struct QMarkNode : UnaryOperator {
    int precedence() override {
        return 1;
    }

    Operator::Type operator_type() override {
        return Operator::Type::QMARK;
    }
};

struct ConcatNode : BinaryOperator {
    int precedence() override {
        return 2;
    }

    Operator::Type operator_type() override {
        return Operator::Type::CONCAT;
    }
};

struct UnionNode : BinaryOperator {
    int precedence() override {
        return 3;
    }

    Operator::Type operator_type() override {
        return Operator::Type::UNION;
    }
};

struct Parser {
    std::vector<Token> tokens;
    std::vector<Node *> results;
    std::vector<InternalNode *> stack;

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

    Node *parse(std::string const &regex) {
        long long curr_pos = 0;

        while (true) {
            if (curr_pos > 0 &&
                can_insert_concat(tokens[curr_pos - 1], tokens[curr_pos])) {
                interpret_operator(new ConcatNode());
            }

            break;
        }

        return nullptr;
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

    void push_node(Node *node) {

    }

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

            if (op->precedence() >= precedence) {
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