//
// Created by bercik on 07.08.2021.
//

/* Repetition qualifiers (*, +, ?, {m,n}, etc) cannot be directly nested. */
/*
 * TODO:
 *  - handle all ASCII (not just printable ones?). Watch out for sign of c++ char.
 */

#include <iostream>
#include <stack>
#include <utility>
#include <vector>


enum class NodeType {
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
    CASE_INSENSITIVE,
    NUMBERED_CAPTURE_GROUP,
    NAMED_CAPTURE_GROUP,
    NON_CAPTURING_GROUP,
};


struct Node {
//    NodeType node_type;
//
//    NodeType get_node_type() const {
//        return node_type;
//    }

    virtual void sth() = 0;
};

struct AsciiCharNode : Node {
    char value;

    explicit AsciiCharNode(char value) : value(value) {}
};

struct CharsRangeNode : Node {
    char begin;
    char end;

    CharsRangeNode(char begin, char end) : begin(begin), end(end) {}
};

struct EscapeCharNode : Node {
    /* SpecialCharNode ? */
    char value;

    explicit EscapeCharNode(char value) : value(value) {}
};

struct OperatorNode : Node {

};

struct CharsetNode : OperatorNode {
    std::vector<Node *> sets; /* maybe differently */
};

struct UnaryOperatorNode : OperatorNode {
    Node *operand = nullptr;
};

struct BinaryOperatorNode : OperatorNode {
    Node *left_operand = nullptr;
    Node *right_operand = nullptr;
};

struct ConcatNode : BinaryOperatorNode {
};

struct UnionNode : BinaryOperatorNode {
};

struct IntersectNode : BinaryOperatorNode {
};

struct MinusNode : BinaryOperatorNode {
};

struct XorNode : BinaryOperatorNode {
};

struct StarNode : UnaryOperatorNode {
};

struct PlusNode : UnaryOperatorNode {
};

struct RepeatNode : UnaryOperatorNode {
    long long min;
    long long max;

    RepeatNode(long long min, long long max) : min(min), max(max) {}
};

struct AlternateNode : BinaryOperatorNode {
};

struct ComplementNode : UnaryOperatorNode {
};

struct OptionalNode : UnaryOperatorNode {
};

struct CaseInsensitiveNode : UnaryOperatorNode {
};

struct GroupNode : UnaryOperatorNode {
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


inline bool is_literal(char c) {
    static std::string const literals =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789 \"#%',/:;<=>@_`";
    return literals.find(c) != std::string::npos || c < ' ';
}


inline bool before_concat(char c) {
    static std::string const chars = "])}+*?";
    return chars.find(c) != std::string::npos || is_literal(c);
}


inline bool after_concat(char c) {
    static std::string const chars = "[(!~";
    return chars.find(c) != std::string::npos || is_literal(c);
}


inline bool can_insert_concat(char prev, char curr) {
    //
    return (prev == ')' || prev == ']' || prev == '}' || prev == '*' || prev == '+' || prev == '?') && (curr == '(' || curr == '[' || curr == '!' || curr == '~' || );
}


struct Regex {
    Node *regex;

    explicit Regex(std::string const &regex) {
        this->regex = parse(regex);
    }

    static Node *parse(std::string const &regex) {
        bool in_char_set = false;
        std::vector<Node *> nodes;
        std::vector<Node *> operators;


        for (int i = 0; i < regex.size();) {
            if (i > 0 && (regex[i - 1]))
        }

        return nullptr;
    }
};


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
    std::string input;
    std::cin >> input;
    std::cout << "Hello " << input << std::endl;
    print();
    Regex regex(input);
    return 0;
}
