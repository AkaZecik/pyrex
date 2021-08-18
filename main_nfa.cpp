//
// Created by bercik on 15.08.2021.
//

#include <string>
#include <iostream>
#include <algorithm>
#include "parser.cpp"
#include "nfa.cpp"

void print_nfa(NFA const &nfa) {
    std::cout << "0 (#) -> ";

    for (auto nbh : nfa.start_node.edges) {
        std::cout << nbh->id << ", ";
    }

    std::cout << std::endl;

    for (auto node : nfa.all_nodes) {
        std::cout << node->id << " (" << node->c << ")" << " -> ";

        for (auto nbh : node->edges) {
            std::cout << nbh->id << ", ";
        }

        std::cout << std::endl;
    }

    std::cout << "end: ";

    for (auto node : nfa.end_nodes) {
        std::cout << node->id << " ";
    }

    std::cout << std::endl;
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