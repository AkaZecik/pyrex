//
// Created by bercik on 15.08.2021.
//

#ifndef DFA_CPP
#define DFA_CPP

#include <unordered_map>
#include "nfa.cpp"

struct PowerDFA {
    struct Node {
        std::unordered_map<char, Node *> map;
        std::set<int> ids;
        char c{};

        explicit Node(int id) : ids({id}) {}

        Node(std::set<int> &&ids, char c) : ids(ids), c(c) {}

        struct Compare {
            bool operator()(Node const *l, Node const *r) const {
                return l->ids < r->ids;
            }
        };
    };

    Node start_node{0};
    std::vector<Node *> end_nodes;
    bool contains_empty = false;

    static PowerDFA from_NFA(NFA const &nfa) {
        PowerDFA dfa;
        std::set<Node *, Node::Compare> all_nodes({&dfa.start_node});
        subset_construction(&nfa.start_node, all_nodes);
        return dfa;
    }

    static void
    subset_construction(NFA::Node const *node, std::set<Node *, Node::Compare> &nodes) {
        std::unordered_map<char, std::set<int>> dfa_edges;

        for (auto *nbh : node->edges) {
            dfa_edges[nbh->c].insert(nbh->id);
        }

        for (auto[c, set] : dfa_edges) {
        }
    }
};

#endif
