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
        bool accepting;

        explicit Node(int id, bool accepting) : id(id), c{}, accepting{accepting} {}

        Node(int id, char c, bool accepting) : id(id), c(c), accepting{accepting} {}
    };

    Node start_node{0, false};
    std::forward_list<Node *> all_nodes;
    std::forward_list<Node *> end_nodes;

    NFA() = default;

    NFA(NFA const &other) {
        start_node.accepting = other.start_node.accepting;
        std::unordered_map<Node const *, Node *> new_nodes;
        auto all_nodes_it = all_nodes.cbefore_begin();
        auto end_nodes_it = end_nodes.cbefore_begin();

        for (auto node : other.all_nodes) {
            auto new_node = new Node(node->id, node->c, node->accepting);
            new_nodes[node] = new_node;
            all_nodes.insert_after(all_nodes_it, new_node);
            ++all_nodes_it;

            if (node->accepting) {
                end_nodes.insert_after(end_nodes_it, new_node);
                ++end_nodes_it;
            }
        }

        for (auto nbh : other.start_node.edges) {
            start_node.edges.insert(new_nodes[nbh]);
        }

        for (auto node : other.all_nodes) {
            auto curr_node = new_nodes[node];

            for (auto nbh : node->edges) {
                curr_node->edges.insert(new_nodes[nbh]);
            }
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
        nfa.start_node.accepting = true;
        return nfa;
    }

    static NFA for_char(int id, char c) {
        NFA nfa;
        auto node = new Node(id, c, true);
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
                return std::move(from_ast(star->operand).star());
            }
            case PLUS: {
                auto plus = reinterpret_cast<PlusNode *>(node);
                return std::move(from_ast(plus->operand).plus());
            }
            case POWER: {
                auto power = reinterpret_cast<PowerNode *>(node);
                return std::move(from_ast(power->operand).power(power->power));
            }
            case MIN: {
                auto min = reinterpret_cast<MinNode *>(node);
                return std::move(from_ast(min->operand).min(min->min));
            }
            case MAX: {
                auto max = reinterpret_cast<MaxNode *>(node);
                return std::move(from_ast(max->operand).max(max->max));
            }
            case RANGE: {
                auto range = reinterpret_cast<RangeNode *>(node);
                return std::move(from_ast(range->operand).range(range->min, range->max));
            }
            case QMARK: {
                auto qmark = reinterpret_cast<QMarkNode *>(node);
                return std::move(from_ast(qmark->operand).qmark());
            }
            case CONCAT: {
                auto concat = reinterpret_cast<ConcatNode *>(node);
                return std::move(from_ast(concat->left_operand).concatenate(
                    from_ast(concat->right_operand)
                ));
            }
            case UNION: {
                auto union_ = reinterpret_cast<UnionNode *>(node);
                return std::move(from_ast(union_->left_operand).union_(
                    from_ast(union_->right_operand)
                ));
            }
            default:
                throw std::runtime_error("Unknown node kind");
        }
    }

    NFA &star() {
        // TODO: what about for_nothing().star()? is it for_nothing() or for_empty()?
        auto &first_pos = start_node.edges;

        for (auto end_node : end_nodes) {
            end_node->edges.insert(first_pos.begin(), first_pos.end());
        }

        start_node.accepting = true;
        return *this;
    }

    NFA &qmark() {
        start_node.accepting = true;
        return *this;
    }

    NFA &concatenate(NFA &&other) {
        auto &other_first_pos = other.start_node.edges;

        for (auto end_node : end_nodes) {
            end_node->edges.insert(other_first_pos.begin(), other_first_pos.end());
        }

        if (other.start_node.accepting) {
            other.end_nodes.splice_after(other.end_nodes.cbefore_begin(), end_nodes);
        } else {
            for (auto node : end_nodes) {
                node->accepting = false;
            }
        }

        std::swap(end_nodes, other.end_nodes);

        if (start_node.accepting) {
            start_node.edges.insert(other_first_pos.begin(), other_first_pos.end());
        }

        other.all_nodes.splice_after(other.all_nodes.cbefore_begin(), all_nodes);
        std::swap(all_nodes, other.all_nodes);
        start_node.accepting = start_node.accepting && other.start_node.accepting;

        other.start_node.edges.clear();
        other.start_node.accepting = false;
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

        start_node.accepting = start_node.accepting || other.start_node.accepting;
        other.start_node.accepting = false;
        return *this;
    }

    NFA &plus() {
        concatenate(std::move(NFA(*this).star()));
        return *this;
    }

    NFA &power(int n) {
        range(n, n);
        return *this;
    }

    NFA &range(int min, int max) {
        if (max == 0) {
            *this = for_empty();
            return *this;
        }

        if (min == 0) {
            start_node.accepting = true;
        }

        if (max == 1) {
            return *this;
        }

        std::vector<NFA> copies(max - 1, *this);

        if (!start_node.accepting) {
            for (int i = min - 1; i < max - 1; ++i) {
                copies[i].start_node.accepting = true;
            }
        }

        for (NFA &copy : copies) {
            concatenate(std::move(copy));
        }

        return *this;
    }

    NFA &min(int min) {
        if (min == 0) {
            star();
            return *this;
        }

        std::vector<NFA> copies(min, *this);
        copies.back().star();

        for (NFA &copy : copies) {
            concatenate(std::move(copy));
        }

        return *this;
    }

    NFA &max(int n) {
        range(0, n);
        return *this;
    }
};

struct Match;
struct IterMatch;

struct Match {
    std::string const text;
    int start;
    int end;

    [[nodiscard]] std::string to_string() const {
        return text.substr(start, end - start);
    }
};

struct IterMatch {
    std::string const text;
    int start;
    int end;
    int pos = 0;

    IterMatch(std::string text, int start, int end) : text(std::move(text)), start(start), end(end) {}

    Match next() {

    }
};

#endif // NFA_CPP
