//
// Created by bercik on 15.08.2021.
//

#include <iostream>
#include "dfa.cpp"
#include "parser.cpp"

void print_node(PowerDFA::TmpNode const &dfa_node) {
    std::size_t size = dfa_node.nodes.size();
    std::size_t i = 0;
    std::cout << "{";

    for (auto nfa_node : dfa_node.nodes) {
        std::cout << nfa_node->id;

        if (i < size - 1) {
            std::cout << ", ";
        }

        ++i;
    }

    std::cout << "}";
}

void print_dfa(PowerDFA const &dfa) {
    std::unordered_map<PowerDFA::TmpNode *, int> ids;
    int i = 0;

    for (auto node : dfa.all_nodes) {
        ids[node] = ++i;
    }

    std::cout << "0 (#) {0} -> ";

    for (auto nbh : dfa.start_node.edges) {
        std::cout << ids[nbh] << ", ";
    }

    std::cout << std::endl;

    for (auto dfa_node : dfa.all_nodes) {
        std::cout << ids[dfa_node] << " (" << dfa_node->c << ") ";
        print_node(*dfa_node);
        std::cout << " -> ";

        for (auto nbh : dfa_node->edges) {
            std::cout << ids[nbh] << ", ";
        }

        std::cout << std::endl;
    }

    std::cout << "\nEND: ";

    if (dfa.start_node.accepting) {
        std::cout << "0, ";
    }

    for (auto node : dfa.all_nodes) {
        if (node->accepting) {
            std::cout << ids[node] << ", ";
        }
    }

    std::cout << std::endl;
}

// ((ab?)|ba?bb)*abb

int main() {
    while (true) {
        std::string input;
        std::cin >> input;
        Parser parser(input);
        Node *tree = parser.parse();
        NFA nfa(NFA::from_ast(tree));
        PowerDFA powerDfa = PowerDFA::from_NFA(nfa);
        print_dfa(powerDfa);
        std::cout << "------------------" << std::endl;
    }
}
