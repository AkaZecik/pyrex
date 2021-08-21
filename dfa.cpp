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
        std::set<TmpNode *> edges;
        char c;
        bool accepting;

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
    std::vector<TmpNode *> all_nodes;

    static PowerDFA from_NFA(NFA const &nfa) {
        PowerDFA dfa{
            .start_node = {
                .nodes = {&nfa.start_node},
                .accepting = nfa.start_node.accepting
            }
        };
        std::set<TmpNode *, TmpNode::Compare> all_nodes;
        subset_construction(&dfa.start_node, all_nodes);
        dfa.all_nodes = std::vector<TmpNode *>(all_nodes.begin(), all_nodes.end());
        return dfa;
    }

    static void subset_construction(
        TmpNode *curr_node, std::set<TmpNode *, TmpNode::Compare> &all_nodes
    ) {
        std::unordered_map<char, std::set<NFA::Node const *>> nfa_edges;
        std::unordered_map<char, bool> accepting_edge;

        for (NFA::Node const *nfa_node : curr_node->nodes) {
            for (NFA::Node const *nbh : nfa_node->edges) {
                nfa_edges[nbh->c].insert(nbh);
                accepting_edge[nbh->c] = accepting_edge[nbh->c] || nbh->accepting;
            }
        }

        for (auto &[c, nfa_nodes] : nfa_edges) {
            auto result = all_nodes.find(nfa_nodes);

            if (result == all_nodes.end()) {
                auto new_node = new TmpNode{std::move(nfa_nodes), {}, c,
                                            accepting_edge[c]};
                curr_node->edges.insert(new_node);
                all_nodes.insert(new_node);
                subset_construction(new_node, all_nodes);
            } else {
                curr_node->edges.insert(*result);
            }
        }
    }
};

#endif
