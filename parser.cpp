//
// Created by bercik on 07.08.2021.
//

/* Repetition qualifiers (*, +, ?, {m,n}, etc) cannot be directly nested. */

#include <iostream>
#include <stack>
#include <utility>
#include <vector>


enum class NodeKind {
    CHAR,
    ESCAPE,
    CHARSET,
    CONCAT,
    UNION,
    INTERSECT,
    MINUS,
    XOR,
    STAR,
    PLUS,
    REPEAT,
    ALTERNATE,
    COMPLEMENT,
    OPTIONAL,
    NOT_GREEDY,  /* does it make sense in DFA implementation? */
    CASE_INSENSITIVE,
    NUMBERED_CAPTURE_GROUP,
    NAMED_CAPTURE_GROUP,
    NON_CAPTURING_GROUP,
};


struct Node {
    virtual void sth() = 0;
};

struct CharNode : Node {
    char value;

    explicit CharNode(char value) : value(value) {}
};

struct RangeNode : Node {
    char begin;
    char end;

    RangeNode(char begin, char end) : begin(begin), end(end) {}
};

struct EscapeCharNode : Node {
    /* SpecialCharNode ? */
    char value;

    explicit EscapeCharNode(char value) : value(value) {}
};

struct CharsetNode : Node {
    std::vector<Node> sets;
};

struct UnaryOperatorNode : Node {
    Node *operand;

    explicit UnaryOperatorNode(Node *operand) : operand(operand) {}
};

struct BinaryOperatorNode : Node {
    Node *left_operand;
    Node *right_operand;

    BinaryOperatorNode(Node *left_operand, Node *right_operand) : left_operand(left_operand),
                                                                  right_operand(right_operand) {}
};

struct ConcatNode : BinaryOperatorNode {
    using BinaryOperatorNode::BinaryOperatorNode;
};

struct UnionNode : BinaryOperatorNode {
    using BinaryOperatorNode::BinaryOperatorNode;
};

struct IntersectNode : BinaryOperatorNode {
    using BinaryOperatorNode::BinaryOperatorNode;
};

struct MinusNode : BinaryOperatorNode {
    using BinaryOperatorNode::BinaryOperatorNode;
};

struct XorNode : BinaryOperatorNode {
    using BinaryOperatorNode::BinaryOperatorNode;
};

struct StarNode : UnaryOperatorNode {
    using UnaryOperatorNode::UnaryOperatorNode;
};

struct PlusNode : UnaryOperatorNode {
    using UnaryOperatorNode::UnaryOperatorNode;
};

struct RepeatNode : UnaryOperatorNode {
    long long min;
    long long max;

    RepeatNode(Node *operand, long long min, long long max) : UnaryOperatorNode(operand), min(min), max(max) {
        // assert 0 <= min <= max
    }
};

struct AlternateNode : BinaryOperatorNode {
    using BinaryOperatorNode::BinaryOperatorNode;
};

struct ComplementNode : UnaryOperatorNode {
    using UnaryOperatorNode::UnaryOperatorNode;
};

struct OptionalNode : UnaryOperatorNode {
    using UnaryOperatorNode::UnaryOperatorNode;
};

struct NotGreedyNode : UnaryOperatorNode {
    using UnaryOperatorNode::UnaryOperatorNode;
    /* does it make sense in DFA implementation?
     * can we somehow emulate it? */
};

struct CaseInsensitiveNode : UnaryOperatorNode {
    using UnaryOperatorNode::UnaryOperatorNode;
};

struct GroupNode : UnaryOperatorNode {
    using UnaryOperatorNode::UnaryOperatorNode;
};

struct NumberedCapturingGroupNode : GroupNode {
    long long number;

    explicit NumberedCapturingGroupNode(Node *operand, long long number) : GroupNode(operand), number(number) {}
};

struct NamedCapturingGroupNode : GroupNode {
    std::string name;

    NamedCapturingGroupNode(Node *operand, std::string name) : GroupNode(operand), name(std::move(name)) {}
};

struct NonCapturingGroupNode : GroupNode {
    using GroupNode::GroupNode;
};


struct Regex {
    Node *regex;

    explicit Regex(std::string const &regex) {
        this->regex = parse(regex);
    }

    static Node *parse(std::string const &regex) {
        return nullptr;
    }
};


int parse(std::string const &regex) {
    bool in_char_set = false;
    std::vector<Node> nodes;
    std::stack<int> operators;

    for (int i = 0; i < regex.size();) {
        if (in_char_set) {
            switch (regex[i]) {
                case '(':
                    break;
            }
        } else {

        }
        if (regex[i] == '[') {
            if (in_char_set) {
                output.emplace_back(TokenKind::op, '[');
            } else {
                in_char_set = true;
            }
        }

    }
}


int print() {
    std::string a[] = {
        "abc", "ab|cd", "a(ba)*", "(a)+a*(a+)+", "[a-cx-z]abc", "((a))((b)|c)",
    };
    for (std::string const &b : a) {
        std::cout << b << std::endl;
    }
    return 0;
}


int main(int argc, char **argv) {
    std::string a;
    std::cin >> a;
    std::cout << "Hello " << a << std::endl;
    print();
    parse(a);
    return 0;
}
