//
// Created by bercik on 13.08.2021.
//

#include <string>

enum NodeKind {
    CHAR,
    GROUP,
    STAR,
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
    char value;

    explicit CharNode(char value) : value(value) {}

    NodeKind node_kind() override {
        return NodeKind::CHAR;
    }
};

struct InternalNode : Node {
    enum class Type {
        GROUP, OPERATOR,
    };

    virtual int arity() = 0;

    virtual Type internal_node_type() = 0;
};

struct Group : InternalNode {
    int number;
    Node *operand = nullptr;

    explicit Group(int number) : number(number) {}

    ~Group() override {
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
