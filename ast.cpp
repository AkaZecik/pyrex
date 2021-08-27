//
// Created by bercik on 13.08.2021.
//

#ifndef AST_CPP
#define AST_CPP

#include <vector>

enum NodeKind {
    NOTHING,
    EMPTY,
    CHAR,
    DOT,
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
};

struct DotNode : LeafNode {
    int id;

    explicit DotNode(int id) : id(id) {}

    NodeKind node_kind() override {
        return NodeKind::DOT;
    }
};

struct EmptyNode : LeafNode {
    NodeKind node_kind() override {
        return NodeKind::EMPTY;
    }
};

struct NothingNode : LeafNode {
    NodeKind node_kind() override {
        return NodeKind::NOTHING;
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
};

struct PlusNode : UnaryOperator {
    NodeKind node_kind() override {
        return NodeKind::PLUS;
    }

    int precedence() override {
        return 1;
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
};

struct QMarkNode : UnaryOperator {
    NodeKind node_kind() override {
        return NodeKind::QMARK;
    }

    int precedence() override {
        return 1;
    }
};

struct ConcatNode : BinaryOperator {
    NodeKind node_kind() override {
        return NodeKind::CONCAT;
    }

    int precedence() override {
        return 2;
    }
};

struct UnionNode : BinaryOperator {
    NodeKind node_kind() override {
        return NodeKind::UNION;
    }

    int precedence() override {
        return 3;
    }
};

#endif // AST_CPP
