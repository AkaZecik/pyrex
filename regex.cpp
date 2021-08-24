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
#include <forward_list>
#include <unordered_map>

// DFA/NFA powinno zwracac iterator, na ktorym mozna wywolac metode .move(char)

int counter = 0;

class Regex {
    struct NFA {
        struct Node {
            std::vector<Node *> edges;
            std::variant<std::monostate, char> edges_kind;
            bool accepting{false};

            Node() {
                counter += 1;
                std::cout << "Node()\t" << counter << std::endl;
            }

            ~Node() {
                counter -= 1;
                std::cout << "~Node()\t" << counter << std::endl;
            }
        };

        Node *start, *end;
        std::forward_list<Node *> all_nodes;

        NFA() : start(new Node()), end(new Node()) {
            all_nodes.push_front(end);
            all_nodes.push_front(start);
            end->accepting = true;
        };

        NFA(NFA const &other) {
            std::unordered_map<NFA::Node *, NFA::Node *> new_nodes;
            auto it = all_nodes.cbefore_begin();

            for (auto node : other.all_nodes) {
                auto new_node = new NFA::Node();
                new_node->accepting = node->accepting;
                new_node->edges_kind = node->edges_kind;
                all_nodes.emplace_after(it);
                ++it;
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

        NFA from_ast(::Node *ast_node) {
        }

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
            right.all_nodes.splice_after(
                right.all_nodes.cbefore_begin(),
                left.all_nodes
            );
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

        }
    };

    // TODO: should return Regex?
    NFA from_ast(::Node *ast_node) {
    }

    struct DFA {

    };
};

#endif