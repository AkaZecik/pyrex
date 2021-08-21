//
// Created by bercik on 14.08.2021.
//

#ifndef NFA_CPP
#define NFA_CPP

#include <set>
#include <forward_list>
#include <unordered_map>
#include "ast.cpp"


struct NFA {
    struct Node {
        std::set<Node *> edges;
        int id;
        char c;
        bool end;

        explicit Node(int id) : id(id), c{}, end{} {}

        Node(int id, char c) : id(id), c(c), end{} {}
    };

    Node start_node{0};
    std::forward_list<Node *> all_nodes;
    std::forward_list<Node *> end_nodes;
    bool contains_empty = false;

    NFA() = default;

    NFA(NFA const &other) {
        std::unordered_map<Node const *, Node *> new_nodes;
        new_nodes[&other.start_node] = &start_node;
        auto it = all_nodes.cbefore_begin();

        for (auto node : other.all_nodes) {
            auto new_node = new Node(node->id, node->c);
            new_nodes[node] = new_node;
            all_nodes.insert_after(it, new_node);
            ++it;
        }
    }

    NFA(NFA &&other) noexcept: NFA() {
        swap(*this, other);
    }

    friend void swap(NFA &left, NFA &right) {
        using std::swap;
        std::swap(left.start_node, right.start_node);
        std::swap(left.all_nodes, right.all_nodes);
        std::swap(left.end_nodes, right.end_nodes);
        std::swap(left.contains_empty, right.contains_empty);
    }

    NFA &operator=(NFA nfa) {
        swap(*this, nfa);
        return *this;
    }

    ~NFA() {
        for (auto node : all_nodes) {
            delete node;
        }
    }

    static NFA for_nothing() {
        return {};
    }

     static NFA for_empty() {
        NFA nfa;
        nfa.contains_empty = true;
        return nfa;
    }

    static NFA for_char(int id, char c) {
        NFA nfa;
        auto node = new Node(id, c);
        nfa.start_node.edges.insert(node);
        nfa.all_nodes.push_front(node);
        nfa.end_nodes.push_front(node);
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
                return from_ast(concat->left_operand).concatenate(
                    from_ast(concat->right_operand)
                );
            }
            case UNION: {
                auto union_ = reinterpret_cast<UnionNode *>(node);
                return from_ast(union_->left_operand).union_(
                    from_ast(union_->right_operand)
                );
            }
            default:
                throw std::runtime_error("Unknown node kind");
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

        if (other.contains_empty) {
            other.end_nodes.splice_after(other.end_nodes.cbefore_begin(), end_nodes);
        }

        std::swap(end_nodes, other.end_nodes);

        if (contains_empty) {
            start_node.edges.insert(other_first_pos.begin(), other_first_pos.end());
        }

        other.all_nodes.splice_after(other.all_nodes.cbefore_begin(), all_nodes);
        std::swap(all_nodes, other.all_nodes);
        contains_empty = contains_empty && other.contains_empty;

        other.start_node.edges.clear();
        other.all_nodes.resize(0);
        other.end_nodes.resize(0);
        return *this;
    }

    NFA &union_(NFA &&other) {
        start_node.edges.insert(
            other.start_node.edges.begin(),
            other.start_node.edges.end()
        );
        other.start_node.edges.clear();

        other.all_nodes.splice_after(other.all_nodes.cbefore_begin(), all_nodes);
        std::swap(all_nodes, other.all_nodes);

        other.end_nodes.splice_after(other.end_nodes.cbefore_begin(), end_nodes);
        std::swap(end_nodes, other.end_nodes);

        contains_empty = contains_empty || other.contains_empty;
        return *this;
    }
};

#endif // NFA_CPP

// ((ab?)|ba?bb)*abb