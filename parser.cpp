//
// Created by bercik on 07.08.2021.
//

/* Repetition qualifiers (*, +, ?, {m,n}, etc) cannot be directly nested. */
/* ja pozwalam! */

#include <iostream>
#include <stack>
#include <utility>
#include <vector>
#include <typeindex>
#include "tokenizer.cpp"


/************************************************
 *                    Nodes                     *
 ************************************************/

struct Node {
    virtual ~Node() = default;
};


struct InternalNode : Node {
    enum class Type {
        GROUP, OPERATOR
    };

    virtual Type type() = 0;
};

struct LeafNode : Node {
};


/************************************************
 *                  Char Nodes                  *
 ************************************************/

struct AsciiCharNode : LeafNode {
    char value;

    explicit AsciiCharNode(char value) : value(value) {}
};

struct CharsRangeNode : LeafNode {
    char begin;
    char end;

    CharsRangeNode(char begin, char end) : begin(begin), end(end) {}
};

struct EscapeCharNode : LeafNode {
    /* SpecialCharNode ? */
    char value;

    explicit EscapeCharNode(char value) : value(value) {}
};

struct CharsetNode : LeafNode {
    std::vector<Node *> sets;  // maybe differently?
};


/************************************************
 *                 Group nodes                  *
 ************************************************/

struct GroupNode : InternalNode {
    Node *operand = nullptr;

    Type type() override {
        return Type::GROUP;
    }
};

struct NumberedCapturingGroupNode : GroupNode {
    long long number;

    explicit NumberedCapturingGroupNode(long long number) : number(number) {}
};

struct NamedCapturingGroupNode : GroupNode {
    std::string name;

    explicit NamedCapturingGroupNode(std::string name) : name(std::move(name)) {}
};

struct NonCapturingGroupNode : GroupNode {
};


/************************************************
 *               Operator nodes                 *
 ************************************************/

struct OperatorNode : InternalNode {
    virtual int arity() = 0;

    virtual int precedence() = 0;

    Type type() override {
        return Type::OPERATOR;
    }
};

struct UnaryOperatorNode : OperatorNode {
    enum class Placement {
        LEFT, RIGHT
    };

    Node *operand = nullptr;

    virtual Placement placement() = 0;

    int arity() override {
        return 1;
    }
};

struct BinaryOperatorNode : OperatorNode {
    Node *left_operand = nullptr;
    Node *right_operand = nullptr;

    int arity() override {
        return 2;
    }
};

struct TildeNode : UnaryOperatorNode {
    int precedence() override {
        return 1;
    }

    Placement placement() override {
        return Placement::LEFT;
    }
};

struct StarNode : UnaryOperatorNode {
    int precedence() override {
        return 2;
    }

    Placement placement() override {
        return Placement::RIGHT;
    }
};

struct PlusNode : UnaryOperatorNode {
    int precedence() override {
        return 2;
    }

    Placement placement() override {
        return Placement::RIGHT;
    }
};

struct RepeatNode : UnaryOperatorNode {
    long long min;
    long long max;

    RepeatNode(long long min, long long max) : min(min), max(max) {}

    int precedence() override {
        return 2;
    }

    Placement placement() override {
        return Placement::RIGHT;
    }
};

struct OptionalNode : UnaryOperatorNode {
    int precedence() override {
        return 2;
    }

    Placement placement() override {
        return Placement::RIGHT;
    }
};

struct ComplementNode : UnaryOperatorNode {
    int precedence() override {
        return 3;
    }

    Placement placement() override {
        return Placement::LEFT;
    }
};

struct MinusNode : BinaryOperatorNode {
    int precedence() override {
        return 4;
    }
};

struct ConcatNode : BinaryOperatorNode {
    int precedence() override {
        return 5;
    }
};

struct IntersectNode : BinaryOperatorNode {
    int precedence() override {
        return 6;
    }
};

struct XorNode : BinaryOperatorNode {
    int precedence() override {
        return 7;
    }
};

struct UnionNode : BinaryOperatorNode {
    int precedence() override {
        return 8;
    }
};


/************************************************
 *                    Parser                    *
 ************************************************/

struct Parser {
    std::string const &regex;
    std::vector<Node *> nodes;
    std::vector<InternalNode *> operators;

    explicit Parser(std::string const &regex) : regex(regex) {}

    Node *parse() {
        Tokenizer tokenizer(regex);
        auto tokens = tokenizer.get_all_tokens();
        int pos = 0;

        while (true) {
            Token token = tokens[pos];

            if (pos > 0 && can_insert_concat(tokens[pos - 1], tokens[pos])) {
                interpret_operator(new ConcatNode());
            }

            if (token.type == TokenType::LBRACK) {
                // sparsuj charset
            } else if (token.type == TokenType::LPAREN) {
                GroupNode *node = nullptr;  // sprawdz jaka to grupa!
                operators.push_back(node);
            } else if (token.type == TokenType::RPAREN) {
                drop_operators_until_group();

                if (operators.empty()) {
                    // error!
                }

                push_node(operators.back());
                operators.pop_back();
                // zdejmuj operatory, az napotkasz lewy nawias
            } else if (token.type == TokenType::LITERAL) {

            } else if (token.type == TokenType::BYTE) {

            } else if (token.type == TokenType::SHORT_UNICODE) {

            } else if (token.type == TokenType::LONG_UNICODE) {

            }

            break;
        }

        drop_operators_until_group();

        if (!operators.empty()) {
            // error!
        } else if (nodes.size() != 1) {
            // error!
        }

        return nodes.back();
    }

    static inline bool can_insert_concat(Token before, Token after) {
        return before_concat(before) && after_concat(after);
    }

    static inline bool before_concat(Token token) {
        return (
            token.type == TokenType::RPAREN ||
            token.type == TokenType::RBRACK ||
            token.type == TokenType::RCURLY ||
            token.type == TokenType::STAR ||
            token.type == TokenType::PLUS ||
            token.type == TokenType::QMARK ||
            token.type == TokenType::LITERAL ||
            token.type == TokenType::DOT ||
            token.type == TokenType::SHORT_UNICODE ||
            token.type == TokenType::LONG_UNICODE ||
            token.type == TokenType::ESCAPE // check if special or not
        );
    }

    static inline bool after_concat(Token token) {
        return (
            token.type == TokenType::LPAREN ||
            token.type == TokenType::LBRACK ||
            token.type == TokenType::EMARK ||
            token.type == TokenType::TILDE ||
            token.type == TokenType::LITERAL ||
            token.type == TokenType::DOT ||
            token.type == TokenType::ESCAPE // check if special or not
        );
    }

    void push_node(InternalNode *op) {
        if (op->type() == InternalNode::Type::GROUP) {
            if (nodes.empty()) {
                // error!
            }


        } else {
            auto _op = reinterpret_cast<OperatorNode *>(op);

            if (nodes.size() < _op->arity()) {
                // error!
            }

            if (_op->arity() == 1) {
                auto uop = reinterpret_cast<UnaryOperatorNode *>(_op);
                Node *operand = nodes.back();
                nodes.pop_back();
                uop->operand = operand;
            } else {
                auto bop = reinterpret_cast<BinaryOperatorNode *>(_op);
                Node *right_operand = nodes.back();
                nodes.pop_back();
                Node *left_operand = nodes.back();
                nodes.pop_back();
                bop->left_operand = left_operand;
                bop->right_operand = right_operand;
            }

            nodes.push_back(_op);
        }
    };

    void interpret_operator(OperatorNode *op) {
        if (op->arity() == 1) {
            auto uop = reinterpret_cast<UnaryOperatorNode *>(op);

            if (uop->placement() == UnaryOperatorNode::Placement::LEFT) {
                operators.push_back(op);
            } else {
                drop_operators_with_higher_or_equal_precedence(op->precedence());
                push_node(op);
            }
        } else {
            drop_operators_with_higher_or_equal_precedence(op->precedence());
            operators.push_back(op);
        }
    };

    void drop_operators_with_higher_or_equal_precedence(int precedence) {
        while (!operators.empty() &&
               operators.back()->type() != InternalNode::Type::GROUP) {
            auto op = reinterpret_cast<OperatorNode *>(operators.back());

            if (op->precedence() >= precedence) {
                operators.pop_back();
                push_node(op);
            } else {
                break;
            }
        }
    }

    void drop_operators_until_group() {
        while (!operators.empty() &&
               operators.back()->type() != InternalNode::Type::GROUP) {
            auto op = reinterpret_cast<OperatorNode *>(operators.back());
            operators.pop_back();
            push_node(op);
        }
    }

    int parse_integer() {
        // determine appropriate type for this function
        return -1;
    }

    std::string parse_identifier() {
        // determine appropriate type for this function
        return "";
    }
};
