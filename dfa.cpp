//
// Created by bercik on 15.08.2021.
//

#ifndef DFA_CPP
#define DFA_CPP

#include <vector>
#include <unordered_map>
#include "nfa.cpp"

struct PowerDFA {
    struct TmpNode {
        std::set<NFA::Node const *> nodes;
        std::unordered_map<char, TmpNode *> edges;

        struct Compare {
            using is_transparent = void;

            bool operator()(TmpNode const *left, TmpNode const *right) const {
                return left->nodes < right->nodes;
            }

            bool operator()(TmpNode const *left,
                            std::set<NFA::Node const *> const &right) const {
                return left->nodes < right;
            }

            bool operator()(std::set<NFA::Node const *> const &left,
                            TmpNode const *right) const {
                return left < right->nodes;
            }
        };
    };

    TmpNode start_node;
    std::vector<TmpNode *> end_nodes;
    bool contains_empty = false;

    static PowerDFA from_NFA(NFA const &nfa) {
        PowerDFA dfa{
            .start_node = {.nodes = {&nfa.start_node}},
            .contains_empty = nfa.contains_empty
        };
        std::set<TmpNode *, TmpNode::Compare> all_nodes({&dfa.start_node});
        subset_construction(&dfa.start_node, all_nodes);
        return dfa;
    }

    static void subset_construction(
        TmpNode *curr_node, std::set<TmpNode *, TmpNode::Compare> &all_nodes
    ) {
        // make "all nodes of dfa" a vector and just emplace on the back?
        // and for comparison just store pointers into that vector? (be careful,
        // vector will change in runtime)
        // fill in "end_states"
        std::unordered_map<char, std::set<NFA::Node const *>> nfa_edges;

        for (NFA::Node const *nfa_node : curr_node->nodes) {
            for (NFA::Node const *nbh : nfa_node->edges) {
                nfa_edges[nbh->c].insert(nbh);
            }
        }

        for (auto &[c, nfa_nodes] : nfa_edges) {
            auto result = all_nodes.find(nfa_nodes);

            if (result == all_nodes.end()) {
                auto new_node = new TmpNode{std::move(nfa_nodes), {}};
                curr_node->edges[c] = new_node;
                subset_construction(new_node, all_nodes);
            } else {
                curr_node->edges[c] = *result;
            }
        }
    }
};

#endif
