//
// Created by bercik on 07.08.2021.
//

/* Repetition qualifiers (*, +, ?, {m,n}, etc) cannot be directly nested. */

#include <iostream>
#include <stack>
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
    NOT_GREEDY,
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

struct EscapeCharNode : Node {
    char value;
    explicit EscapeCharNode(char value) : value(value) {}
};

struct CharsetNode : Node {

};

struct ConcatNode : Node {

};

struct UnionNode : Node {

};

struct IntersectNode : Node {

};

struct MinusNode : Node {

};

struct XorNode : Node {

};

struct StarNode : Node {

};

struct PlusNode : Node {

};

struct RepeatNode : Node {

};

struct AlternateNode : Node {

};

struct ComplementNode : Node {

};

struct OptionalNode : Node {

};

struct NotGreedyNode : Node {

};

struct CaseInsensitiveNode : Node {

};

struct NumberedCapturingGroupNode : Node {

};

struct NamedCapturingGroupNode : Node {

};

struct NonCapturingGroupNode : Node {

};


int parse(std::string const &regex) {
    bool in_char_set = false;
    std::vector<Node> nodes;
    std::stack<int> operators;

    for (int i = 0; i < regex.size();) {
        if (in_char_set) {
            switch (regex[i]) {
                case '(': break;
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
