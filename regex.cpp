//
// Created by bercik on 10.08.2021.
//

#ifndef REGEX_CPP
#define REGEX_CPP

#include <vector>
#include <variant>
#include <memory>
#include "ast.cpp"
#include <iostream>
#include <list>
#include <unordered_map>

// DFA/NFA powinno zwracac iterator, na ktorym mozna wywolac metode .move(char)
// zapytac sie czy jest to stan koncowy, etc.
// Takie rozwlekle NFA da sie narysowac planarnie.

class Regex {
    struct NFA {
        struct Node {
            std::vector<Node *> edges;
            std::variant<std::monostate, char> edges_kind;
            bool accepting{false};
        };

        Node *start, *end;
        std::list<Node *> all_nodes;

        NFA() : start(new Node()), end(new Node()) {
            all_nodes.push_front(end);
            all_nodes.push_front(start);
            end->accepting = true;
        }

        NFA(NFA const &other) {
            // TODO: this variable would be useful in Regex
            //  maybe two copy constructors, one providing reference to a map?
            std::unordered_map<NFA::Node *, NFA::Node *> new_nodes;

            for (auto node : other.all_nodes) {
                auto new_node = new NFA::Node();
                new_node->accepting = node->accepting;
                new_node->edges_kind = node->edges_kind;
                all_nodes.push_back(new_node);
                new_nodes[node] = new_node;
            }

            for (auto[node, new_node] : new_nodes) {
                for (auto nbh : node->edges) {
                    new_node->edges.push_back(new_nodes[nbh]);
                }
            }

            start = new_nodes[other.start];
            end = new_nodes[other.end];
        }

        NFA(NFA &&other) = default;
        NFA &operator=(NFA const &) = default;
        NFA &operator=(NFA &&) = default;

        ~NFA() {
            for (auto node : all_nodes) {
                delete node;
            }
        }

        NFA from_ast(::Node *ast_node) {}

        static NFA for_nothing() {
            return {};
        }

        static NFA for_empty() {
            NFA nfa;
            nfa.start->edges.push_back(nfa.end);
            return nfa;
        }

        static NFA for_char(char c) {
            NFA nfa;
            nfa.start->edges.push_back(nfa.end);
            nfa.start->edges_kind = c;
            return nfa;
        }

        static NFA for_concat(NFA left, NFA right) {
            left.end->edges.push_back(right.start);
            left.all_nodes.splice(left.all_nodes.begin(), right.all_nodes);
            std::swap(left.all_nodes, right.all_nodes);
            left.end = right.end;
            right.start = left.start = nullptr;
            return left;
        }

        static NFA for_union(NFA left, NFA right) {
            NFA result;
            result.start->edges.push_back(left.start);
            result.start->edges.push_back(right.start);
            left.end->edges.push_back(result.end);
            right.end->edges.push_back(result.end);
            left.all_nodes.splice(left.all_nodes.end(), right.all_nodes);
            result.all_nodes.splice(++result.all_nodes.cbegin(), left.all_nodes);
            left.start = left.end = right.start = right.end = nullptr;
            return result;
        }

        static NFA for_star(NFA nfa) {
            NFA result;
            nfa.end->edges.push_back(result.start);
            result.start->edges.push_back(result.end);
            result.end->edges.push_back(nfa.start);
            result.all_nodes.splice(++result.all_nodes.begin(), nfa.all_nodes);
            nfa.start = nfa.end = nullptr;
            return result;
        }

        static NFA for_plus(NFA nfa) {
            NFA star = for_star(nfa);
            return for_concat(std::move(nfa), std::move(star));
        }

        static NFA for_qmark(NFA nfa) {
            return for_union(std::move(nfa), for_empty());
        }

        static NFA for_power(NFA nfa, int power) {
            return for_range(std::move(nfa), power, power);
        }

        static NFA for_min(NFA nfa, int min) {
            if (min == 0) {
                return for_star(std::move(nfa));
            }

            NFA star = for_star(nfa);
            return for_concat(for_power(std::move(nfa), min), star);
        }

        static NFA for_max(NFA nfa, int max) {
            return for_range(std::move(nfa), 0, max);
        }

        static NFA for_range(NFA nfa, int min, int max) {
            if (max == 0) {
                // TODO: is it OK that we are loosing information?
                //  what about regex (hello){0,0} and groups collected before?
                //  should they be removed?
                return for_empty();
            }

            std::vector<NFA> copies(max - 1, nfa);
            copies.push_back(std::move(nfa));

            for (int i = min; i < max; ++i) {
                copies[i] = for_union(std::move(copies[i]), for_empty());
            }

            while (copies.size() != 1) {
                copies[0] = for_concat(std::move(copies[0]), std::move(copies.back()));
            }

            return std::move(copies.back());
        }

        static NFA for_group(NFA nfa) {
            NFA result;
            result.start->edges.push_back(nfa.start);
            nfa.end->edges.push_back(result.end);
            result.all_nodes.splice(++result.all_nodes.begin(), nfa.all_nodes);
            nfa.start = nfa.end = nullptr;
            return result;
        }
    };

    // TODO: should return Regex?
    NFA from_ast(::Node *ast_node) {
    }

    struct DFA {

    };

    Regex &concat(Regex other) {
        nfa = NFA::for_concat(std::move(nfa), std::move(other.nfa));
        return *this;
    }

    static Regex concat(Regex left, Regex right) {
        return left.concat(std::move(right));
    }

    NFA nfa;
};

#endif