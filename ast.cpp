//
// Created by bercik on 13.08.2021.
//


struct Node {
    virtual ~Node() = default;
};

struct LeafNode : Node {
};

struct CharNode : LeafNode {
    char value;

    explicit CharNode(char value) : value(value) {}
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

    InternalNode::Type internal_node_type() override {
        return InternalNode::Type::GROUP;
    }

    int arity() override {
        return 1;
    }
};

struct Operator : InternalNode {
    enum class Type {
        STAR, QMARK, CONCAT, UNION,
        };

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