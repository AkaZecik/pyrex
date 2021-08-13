//
// Created by bercik on 13.08.2021.
//

enum NodeType {
    CHAR,
    GROUP,
    STAR,
    QMARK,
    CONCAT,
    UNION,
};


struct Node {
    virtual NodeType node_type() = 0;

    virtual ~Node() = default;
};

struct LeafNode : Node {
};

struct CharNode : LeafNode {
    char value;

    explicit CharNode(char value) : value(value) {}

    NodeType node_type() override {
        return NodeType::CHAR;
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

    NodeType node_type() override {
        return NodeType::GROUP;
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
    NodeType node_type() override {
        return NodeType::STAR;
    }

    int precedence() override {
        return 1;
    }
};

struct QMarkNode : UnaryOperator {
    NodeType node_type() override {
        return NodeType::QMARK;
    }

    int precedence() override {
        return 1;
    }
};

struct ConcatNode : BinaryOperator {
    NodeType node_type() override {
        return NodeType::CONCAT;
    }

    int precedence() override {
        return 2;
    }
};

struct UnionNode : BinaryOperator {
    NodeType node_type() override {
        return NodeType::UNION;
    }

    int precedence() override {
        return 3;
    }
};
