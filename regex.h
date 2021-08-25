//
// Created by bercik on 24.08.2021.
//

#ifndef PYREX_REGEX_H
#define PYREX_REGEX_H

#include <vector>
#include <variant>
#include "ast.cpp"
#include <list>

int counter = 0;

class Regex {
    struct NFA {
        struct Node {
            std::vector<Node *> edges;
            std::variant<std::monostate, char> edges_kind;
            bool accepting;
        };

        Node *start, *end;
        std::list<Node *> all_nodes;

        NFA();
        NFA(NFA const &other);
        NFA(NFA &&other) noexcept;
        ~NFA();
        NFA from_ast(::Node *ast_node);

        static NFA for_nothing();
        static NFA for_empty();
        static NFA for_char(char c);
        static NFA for_concat(NFA left, NFA right);
        static NFA for_union(NFA left, NFA right);
        static NFA for_star(NFA nfa);
        static NFA for_plus(NFA nfa);
    };

    NFA from_ast(::Node *ast_node);

    struct DFA {};
};

#endif

#endif //PYREX_REGEX_H
