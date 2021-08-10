//
// Created by bercik on 07.08.2021.
//

/* Repetition qualifiers (*, +, ?, {m,n}, etc) cannot be directly nested. */

#include <iostream>
#include <stack>
#include <utility>
#include <vector>
#include <typeindex>
#include "tokenizer.cpp"


union Char {
    char c;
    unsigned int i;
};


struct Range {
    Char start;
    Char end;
};

int hehe() {
    Range range{.start = {.i = 10}, .end = {.c = 'c'}};
    std::cout << range.start.i << std::endl;
    return 0;
}


enum class NodeType2 {
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


enum class NodeType {
    NUMBERED_CG,
    NAMED_CG,
    NON_CG,
    CHARSET,
    CHARSET_COMPLEMENT,
    RANGE,
    STAR,
    PLUS,
    COMPLEMENT,
    UNION,
    INTERSECTION,
    OPTIONAL,
    EXACT_REPEAT,
    MIN_REPEAT,
    MAX_REPEAT,
    RANGE_REPEAT,
    TILDE,
    PERCENT,
};

struct NodeRepr {
    NodeType type;
    union {
        char c;
        unsigned int i;
        std::string s;
    } value;
};


int operator_arity(OperatorNode *node) {
    if (dynamic_cast<UnaryOperatorNode *>(node)) {
        return 1;
    } else if (dynamic_cast<BinaryOperatorNode *>(node)) {
        return 2;
    } else {
        throw std::runtime_error(
            "Expected either UnaryOperatorNode or BinaryOperatorNode"
        );
    }
}


int operator_precedence(OperatorNode *node) {
    auto ti = std::type_index(typeid(*node));

    if (ti == std::type_index(typeid(NamedCapturingGroupNode)) ||
        ti == std::type_index(typeid(NumberedCapturingGroupNode)) ||
        ti == std::type_index(typeid(NonCapturingGroupNode))) {
        return 0;
    }

    if (ti == std::type_index(typeid(CaseInsensitiveNode))) {
        return 1;
    }

    if (ti == std::type_index(typeid()))

    if (ti == std::type_index(typeid()))

    if (ti == std::type_index(typeid(GroupNode))) {

    } else if (ti == std::type_index(typeid(StarNode))) {
        return 1;
    } else if (ti == std::type_index(typeid()))
}


struct Regex {
    std::string const &regex;
    Node *tree;

    explicit Regex(std::string const &regex) : regex(regex) {
        tree = parse(regex);
    }

    static Node *parse(std::string const &regex) {
        Tokenizer tokenizer(regex);
        std::vector<Token> tokens = tokenizer.get_all_tokens();
        std::vector<Node *> nodes;
        std::vector<Node *> operators;
        int i = 0;

        while (true) {
            Token token = tokens[i];

            if (i > 0 && before_concat(tokens[i - 1]) && after_concat(tokens[i])) {

                // interpret 'concat'
            }

            if (token.type == TokenType::LBRACK) {
                // sparsuj charset
            } else if (token.type == TokenType::LPAREN) {
                // sprawdz jaka to grupa
                // wrzuc na stos
            } else if (token.type == TokenType::RPAREN) {
                // zdejmuj operatory, az napotkasz lewy nawias
            } else if (token.type == TokenType::LITERAL) {

            } else if (token.type == TokenType::BYTE) {

            } else if (token.type == TokenType::SHORT_UNICODE) {

            } else if (token.type == TokenType::LONG_UNICODE) {

            } else if (token.type == TokenType::)

                break;
        }
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
