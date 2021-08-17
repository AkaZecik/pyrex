//
// Created by bercik on 15.08.2021.
//

#ifndef DFA_CPP
#define DFA_CPP

#include <vector>
#include <unordered_map>
#include "nfa.cpp"

struct PowerDFA {
    struct Node {
        std::unordered_map<char, Node *> map;
        // do we perhaps want to collect NFA::Node * instead?
        std::set<int> ids;
        char c{};

        explicit Node(int id) : ids({id}) {}

        Node(std::set<int> &&ids, char c) : ids(ids), c(c) {}

        struct Compare {
            using is_transparent = void;

            bool operator()(Node *left, Node *right) const {
                return left->ids < right->ids;
            }

            bool operator()(Node *left, std::set<int> const &right) {
                return left->ids < right;
            }

            bool operator()(std::set<int> const &left, Node *right) {
                return left < right->ids;
            }
        };
    };

    Node start_node{0};
    std::vector<Node *> end_nodes;
    bool contains_empty = false;

    static PowerDFA from_NFA(NFA const &nfa) {
        PowerDFA dfa;
        std::set<PowerDFA::Node *, PowerDFA::Node::Compare> dfa_nodes(
            {&dfa.start_node}
        );
        std::set<NFA::Node const *> nfa_nodes({&nfa.start_node});
        subset_construction(nfa_nodes, &dfa.start_node, dfa_nodes);
        return dfa;
    }

    static void subset_construction(
        std::set<NFA::Node const *> const &curr_nfa_nodes,
        PowerDFA::Node const *curr_dfa_node,
        std::set<PowerDFA::Node *, PowerDFA::Node::Compare> &all_dfa_nodes
    ) {
        // is it ok that type of values is std::set and not std::set *?
        std::unordered_map<char, std::set<NFA::Node const *>> nfa_edges;

        for (auto nfa_node : curr_nfa_nodes) {
            for (auto nbh : nfa_node->edges) {
                nfa_edges[nbh->c].insert(nbh);
            }
        }

        std::unordered_map<char, Node *> dfa_edges;

        for (auto &[c, nfa_nodes] : nfa_edges) {
            std::set<int> ids;

            for (auto node : nfa_nodes) {
                ids.insert(node->id);
            }

            auto result = all_dfa_nodes.find(ids);
            Node *node = nullptr;

            if (result == all_dfa_nodes.end()) {
                node = new Node(std::move(set), c);
                all_dfa_nodes.insert(node);
            } else {
                node = *result;
            }

            subset_construction(node, all_dfa_nodes);
        }
    }
};

#endif
