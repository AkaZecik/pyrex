//
// Created by bercik on 15.08.2021.
//

#include <string>
#include <iostream>
#include <algorithm>
#include "parser.cpp"
#include "nfa.cpp"

void collect_nodes(NFA::Node const *node, std::set<NFA::Node const *> &all_nodes) {
    all_nodes.insert(node);

    for (auto *nbh : node->edges) {
        if (all_nodes.find(nbh) == all_nodes.end()) {
            collect_nodes(nbh, all_nodes);
        }
    }
}

void print_nfa(NFA const &nfa) {
    std::set<NFA::Node const *> all_nodes;
    NFA::Node const *start = &nfa.start_node;
    collect_nodes(start, all_nodes);

    for (auto *node : all_nodes) {
        std::cout << node->id << " (" << node->c << ")" << " -> ";

        for (auto *nbh : node->edges) {
            std::cout << nbh->id << ", ";
        }

        std::cout << std::endl;
    }
}

int main() {
    while (true) {
        std::string input;
        std::cin >> input;
        Parser parser(input);
        Node *tree = parser.parse();
        NFA nfa = NFA::from_ast(tree);
        print_nfa(nfa);
        std::cout << "---" << std::endl;
    }
}