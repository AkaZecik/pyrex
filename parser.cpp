//
// Created by bercik on 07.08.2021.
//

/* Repetition qualifiers (*, +, ?, {m,n}, etc) cannot be directly nested. */
/* ja pozwalam! */

/* TODO: splitnij ten plik na dwa: parser + regex, regex ma tylko i wylacznie
 *  wciagnac parser i go odpalic i zapisac sobie wynikowe drzewko.
 *  Klasa i plik Regex maja obslugiwac logike juz sparsowanego drzewka,
 *  nie obchodza ich szczegoly parsowania.
 */

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
 *                    Regex                     *
 ************************************************/

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
        std::vector<OperatorNode *> operators;
        int i = 0;

        auto push_node = [&nodes](OperatorNode *op) {
            if (nodes.size() < op->arity()) {
                // error!
            }

            if (op->arity() == 1) {
                auto _op = reinterpret_cast<UnaryOperatorNode *>(op);
                Node *operand = nodes.back();
                nodes.pop_back();
                _op->operand = operand;
            } else {
                auto _op = reinterpret_cast<BinaryOperatorNode *>(op);
                Node *right_operand = nodes.back();
                nodes.pop_back();
                Node *left_operand = nodes.back();
                nodes.pop_back();
                _op->left_operand = left_operand;
                _op->right_operand = right_operand;
            }

            nodes.push_back(op);
        };

        auto interpret_operator = [&operators, &push_node](OperatorNode *op) {
            if (op->arity() == 1) {
                auto _op = reinterpret_cast<UnaryOperatorNode *>(op);

                if (_op->placement() == UnaryOperatorNode::Placement::LEFT) {
                    operators.push_back(op);
                } else {
                    while (!operators.empty() && operators.back()->precedence() >= op->precedence()) {
                        push_node(operators.back());
                        operators.pop_back();
                    }

                    push_node(op);
                }
            } else {
                while (!operators.empty() && operators.back()->precedence() >= op->precedence()) {
                    push_node(operators.back());
                    operators.pop_back();
                }

                operators.push_back(op);
            }
        };

        while (true) {
            Token token = tokens[i];

            if (i > 0 && can_insert_concat(tokens[i - 1], tokens[i])) {
                auto concat = new ConcatNode();
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
