//
// Created by bercik on 14.08.2021.
//

#include <set>
#include "ast.cpp"


struct NFA {
    struct Node {
        std::set<Node *> edges;
        int id;
        char c;

        explicit Node(int id) : id(id), c{} {}
        Node(int id, char c) : id(id), c(c) {}
    };

    Node start_node{0};
    std::vector<Node *> end_nodes;
    bool contains_empty = false;

    static NFA for_char(int id, char c) {
        NFA nfa;
        auto node = new Node(id, c);
        nfa.end_nodes.push_back(node);
        return nfa;
    }

    static NFA from_ast(::Node *node) {
        switch (node->node_kind()) {
            case CHAR: {
                auto c = reinterpret_cast<CharNode *>(node);
                return for_char(c->id, c->value);
            }
            case GROUP: {
                auto group = reinterpret_cast<GroupNode *>(node);
                return from_ast(group->operand);
            }
            case STAR: {
                auto star = reinterpret_cast<StarNode *>(node);
                return from_ast(star->operand).star();
            }
            case QMARK: {
                auto qmark = reinterpret_cast<QMarkNode *>(node);
                return from_ast(qmark->operand).qmark();
            }
            case CONCAT: {
                auto concat = reinterpret_cast<ConcatNode *>(node);
                return from_ast(concat->left_operand).concatenate(from_ast(concat->right_operand));
            }
            case UNION: {
                auto union_ = reinterpret_cast<UnionNode *>(node);
                return from_ast(union_->left_operand).union_(from_ast(union_->right_operand));
            }
        }
    }

    NFA &star() {
        auto &first_pos = start_node.edges;

        for (auto end_node : end_nodes) {
            end_node->edges.insert(first_pos.begin(), first_pos.end());
        }

        contains_empty = true;
        return *this;
    }

    NFA &qmark() {
        contains_empty = true;
        return *this;
    }

    NFA &concatenate(NFA &&other) {
        auto &other_first_pos = other.start_node.edges;

        for (auto end_node : end_nodes) {
            end_node->edges.insert(other_first_pos.begin(), other_first_pos.end());
        }

        end_nodes = std::move(other.end_nodes);

        if (contains_empty) {
            start_node.edges.insert(other_first_pos.begin(), other_first_pos.end());
        }

        contains_empty = contains_empty && other.contains_empty;
        other.start_node.edges.clear();
        other.end_nodes.resize(0);
        return *this;
    }

    NFA &union_(NFA &&other) {
        start_node.edges.insert(
            other.start_node.edges.begin(),
            other.start_node.edges.end()
        );
        end_nodes.reserve(end_nodes.size() + other.end_nodes.size());
        end_nodes.insert(
            end_nodes.end(),
            other.end_nodes.begin(),
            other.end_nodes.end()
        );
        other.start_node.edges.clear();
        other.end_nodes.resize(0);
        contains_empty = contains_empty || other.contains_empty;
        return *this;
    }
};