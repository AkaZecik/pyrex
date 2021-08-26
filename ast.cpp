//
// Created by bercik on 13.08.2021.
//

#ifndef AST_CPP
#define AST_CPP

#include <string>
#include <vector>

enum NodeKind {
    EMPTY,
    CHAR,
    GROUP,
    STAR,
    PLUS,
    POWER,
    MIN,
    MAX,
    RANGE,
    QMARK,
    CONCAT,
    UNION,
};


struct Node {
    virtual ~Node() = default;

    virtual NodeKind node_kind() = 0;

    virtual std::string to_string() = 0;
};

struct LeafNode : Node {
};

struct CharNode : LeafNode {
    int id;
    char value;

    explicit CharNode(int id, char value) : id(id), value(value) {}

    NodeKind node_kind() override {
        return NodeKind::CHAR;
    }

    std::string to_string() override {
        switch (value) {
            case '\n':
                return "\\n";
            case '\r':
                return "\\r";
            case '\f':
                return "\\f";
            case '\t':
                return "\\t";
            case '\\':
                return "\\\\";
            case '(':
                return "\\(";
            case ')':
                return "\\)";
            case '*':
                return "\\*";
            case '|':
                return "\\|";
            case '?':
                return "\\?";
            default:
                if (' ' <= value && value <= '~') {
                    return {1, value};
                } else {
                    // TODO: assumes size of char is 1 byte
                    static char const *const hex = "0123456789abcdef";
                    auto n = static_cast<unsigned char>(value);
                    std::string result = "\\x00";
                    result[2] = hex[n / 16];
                    result[3] = hex[n % 16];
                    return result;
                }
        }
    }
};

struct Empty : LeafNode {
    NodeKind node_kind() override {
        return NodeKind::EMPTY;
    }

    std::string to_string() override {
        return "\\e";
    }
};

struct InternalNode : Node {
    enum class Type {
        GROUP, OPERATOR,
    };

    virtual int arity() = 0;

    virtual Type internal_node_type() = 0;
};

struct GroupNode : InternalNode {
    int number;
    Node *operand = nullptr;

    explicit GroupNode(int number) : number(number) {}

    ~GroupNode() override {
        delete operand;
    }

    NodeKind node_kind() override {
        return NodeKind::GROUP;
    }

    InternalNode::Type internal_node_type() override {
        return InternalNode::Type::GROUP;
    }

    int arity() override {
        return 1;
    }

    std::string to_string() override {
        return std::string("(").append(operand->to_string()).append(")");
    }
};

struct Operator : InternalNode {
    virtual int precedence() = 0;

    InternalNode::Type internal_node_type() override {
        return InternalNode::Type::OPERATOR;
    }
};

struct UnaryOperator : Operator {
    Node *operand = nullptr;

    ~UnaryOperator() override {
        delete operand;
    }

    int arity() override {
        return 1;
    }
};

struct BinaryOperator : Operator {
    Node *left_operand = nullptr;
    Node *right_operand = nullptr;

    ~BinaryOperator() override {
        delete left_operand;
        delete right_operand;
    }

    int arity() override {
        return 2;
    }
};

struct StarNode : UnaryOperator {
    NodeKind node_kind() override {
        return NodeKind::STAR;
    }

    int precedence() override {
        return 1;
    }

    std::string to_string() override {
        return operand->to_string().append("*");
    }
};

struct PlusNode : UnaryOperator {
    NodeKind node_kind() override {
        return NodeKind::PLUS;
    }

    int precedence() override {
        return 1;
    }

    std::string to_string() override {
        return operand->to_string().append("+");
    }
};

struct PowerNode : UnaryOperator {
    int power;

    explicit PowerNode(int power) : power(power) {}

    NodeKind node_kind() override {
        return NodeKind::POWER;
    }

    int precedence() override {
        return 1;
    }

    std::string to_string() override {
        return operand->to_string().append("{").append(std::to_string(power)).append(
            "}");
    }
};

struct MinNode : UnaryOperator {
    int min;

    explicit MinNode(int min) : min(min) {}

    NodeKind node_kind() override {
        return NodeKind::MIN;
    }

    int precedence() override {
        return 1;
    }

    std::string to_string() override {
        return operand->to_string().append("{").append(std::to_string(min)).append(
            ",}");
    }
};

struct MaxNode : UnaryOperator {
    int max;

    explicit MaxNode(int max) : max(max) {}

    NodeKind node_kind() override {
        return NodeKind::MAX;
    }

    int precedence() override {
        return 1;
    }

    std::string to_string() override {
        return operand->to_string().append("{,").append(std::to_string(max)).append(
            "}");
    }
};

struct RangeNode : UnaryOperator {
    int min;
    int max;

    RangeNode(int min, int max) : min(min), max(max) {}

    NodeKind node_kind() override {
        return NodeKind::RANGE;
    }

    int precedence() override {
        return 1;
    }

    std::string to_string() override {
        return operand->to_string().append("{").append(std::to_string(min)).append(
            ",").append(std::to_string(max)).append("}");
    }
};

struct QMarkNode : UnaryOperator {
    NodeKind node_kind() override {
        return NodeKind::QMARK;
    }

    int precedence() override {
        return 1;
    }

    std::string to_string() override {
        return operand->to_string().append("?");
    }
};

struct ConcatNode : BinaryOperator {
    NodeKind node_kind() override {
        return NodeKind::CONCAT;
    }

    int precedence() override {
        return 2;
    }

    std::string to_string() override {
        return left_operand->to_string().append(right_operand->to_string());
    }
};

struct UnionNode : BinaryOperator {
    NodeKind node_kind() override {
        return NodeKind::UNION;
    }

    int precedence() override {
        return 3;
    }

    std::string to_string() override {
        return left_operand->to_string().append("|").append(right_operand->to_string());
    }
};

#endif // AST_CPP
