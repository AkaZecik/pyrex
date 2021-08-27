//
// Created by bercik on 14.08.2021.
//

#ifndef NFA_CPP
#define NFA_CPP

#include <set>
#include <list>
#include <map>
#include <unordered_map>
#include <variant>
#include <iostream>
#include "ast.cpp"

/*
 * Alphabet mozna zrobic tak, ze jest klasa Char, ktora ma w sobie std::variant
 * dla monostate, char, range, charset, etc.
 *
 * mozna optymalizowac tak, ze jak mamy pionki chodzace po NFA,
 * to i tak rownolegle chodzimy po DFA. Jak dojdziemy do pozycji "end" w stringu
 * to przestajemy zapamietywac nowe substringi, ale dalej pamietamy pionki, ktore
 * chodzily po NFA, z tym, ze juz nie tworzymy nowych pionkow, a te ktore znikly
 * po prostu znikly. Jak liczba pionkow sie skonczy albo dojdziemy do stanu koncowego
 */

struct Group {
    Node *node = nullptr; // pointer to AST tree node
};

enum class GroupToken {
    ENTER, LEAVE,
};

std::list<Group> groups;

struct NFA {
    typedef std::unordered_map<Group *, std::vector<GroupToken>> GroupToTokens;

    struct Node {
        typedef std::unordered_map<char, std::map<Node *, GroupToTokens>> Edges;

        // store independently:
        // - std::map<char, std::set<Node *>>
        // - std::unordered_map<Node *, GroupToTokens>
        Edges edges;
        std::variant<std::monostate, GroupToTokens> empty_edge;
        int id;
        char c;

        explicit Node(int id) : id(id), c{} {}

        Node(int id, char c) : id(id), c(c) {}
    };

    Node start_node{0};
    std::list<Node *> all_nodes;
    std::list<Node *> lastpos;

    NFA() = default;

    NFA(NFA const &other) {
//        std::cout << "NFA(NFA const &)" << std::endl;
        std::unordered_map<Node const *, Node *> new_nodes;

        for (auto orig_node : other.all_nodes) {
            auto new_node = new Node(orig_node->id, orig_node->c);
            new_nodes[orig_node] = new_node;
            all_nodes.push_back(new_node);

            if (std::holds_alternative<GroupToTokens>(orig_node->empty_edge)) {
                lastpos.push_back(new_node);
            }
        }

        for (auto[orig_node, new_node] : new_nodes) {
            new_node->empty_edge = orig_node->empty_edge;

            for (auto &[c, orig_node_edges_for_c] : orig_node->edges) {
                auto &new_node_edges_for_c = new_node->edges[c];

                for (auto &[nbh, tokens]: orig_node_edges_for_c) {
                    new_node_edges_for_c[new_nodes[nbh]] = tokens;
                }
            }
        }

        start_node.empty_edge = other.start_node.empty_edge;

        for (auto &[c, orig_start_node_edges_for_c] : other.start_node.edges) {
            auto &start_node_edges_for_c = start_node.edges[c];

            for (auto &[nbh, tokens] : orig_start_node_edges_for_c) {
                start_node_edges_for_c[new_nodes[nbh]] = tokens;
            }
        }
    }

    NFA(NFA &&other) = default;

    NFA &operator=(NFA nfa) {
        std::swap(*this, nfa);
        return *this;
    }

    ~NFA() {
        for (auto node : all_nodes) {
            delete node;
        }
    }

    bool traverse(std::string const &text, std::size_t start, std::size_t end,
                  Group *group) {
        std::vector<Node *> old_state, new_state;
        std::unordered_map<Node *, bool> visited;

        for (auto node : all_nodes) {
            visited[node] = false;
        }

        new_state.push_back(&start_node);
        std::size_t pos = 0;

        while (!new_state.empty()) {
            if (pos == text.size()) {
                for (auto node : new_state) {
                    if (std::holds_alternative<GroupToTokens>(node->empty_edge)) {
                        return true;
                    }
                }

                return false;
            }

            std::swap(new_state, old_state);
            char c = text[pos];
            pos += 1;

            while (!old_state.empty()) {
                auto top = old_state.back();
                old_state.pop_back();
                auto edges_for_c_it = top->edges.find(c);

                if (edges_for_c_it != top->edges.end()) {
                    for (auto &[nbh, tokens] : edges_for_c_it->second) {
                        if (!visited[nbh]) {
                            visited[nbh] = true;
                            new_state.push_back(nbh);
                        }
                    }
                }
            }

            for (auto node : new_state) {
                visited[node] = false;
            }
        }

        return false;
    }

    static NFA from_ast(::Node *node) {
        switch (node->node_kind()) {
            case CHAR: {
                auto c = reinterpret_cast<CharNode *>(node);
                return for_char(c->id, c->value);
            }
            case GROUP: {
                auto group = reinterpret_cast<GroupNode *>(node);
                return std::move(from_ast(group->operand).for_numbered_group());
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
                return std::move(
                    from_ast(range->operand).range(range->min, range->max));
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

    static NFA for_nothing() {
        return {};
    }

    static NFA for_empty() {
        NFA nfa;
        nfa.start_node.empty_edge.emplace<GroupToTokens>();
        return nfa;
    }

    static NFA for_char(int id, char c) {
        NFA nfa;
        auto node = new Node(id, c);
        node->empty_edge.emplace<GroupToTokens>();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);
        nfa.start_node.edges[c].insert({node, {}});
        return nfa;
    }

    NFA &for_non_capturing_group() {
        return *this;
    }

    NFA &for_numbered_group() {
        Group *numbered_group = nullptr;
        for_group(numbered_group);
        return *this;
    }

    NFA &for_named_group() {
        Group *named_group = nullptr;
        for_group(named_group);
        return *this;
    }

    NFA &for_group(Group *group) {
        if (auto empty_edge = std::get_if<GroupToTokens>(&start_node.empty_edge)) {
            auto &tokens = (*empty_edge)[group];
            tokens.push_back(GroupToken::ENTER);
            tokens.push_back(GroupToken::LEAVE);
        }

        for (auto &[c, edges_for_c] : start_node.edges) {
            for (auto &[nbh, tokens] : edges_for_c) {
                tokens[group].push_back(GroupToken::ENTER);
            }
        }

        for (auto lastpos_node : lastpos) {
            std::get<GroupToTokens>(lastpos_node->empty_edge)[group]
                .push_back(GroupToken::LEAVE);
        }

        return *this;
    }

    NFA &qmark() {
        if (std::holds_alternative<std::monostate>(start_node.empty_edge)) {
            start_node.empty_edge.emplace<GroupToTokens>();
        }

        return *this;
    }

    NFA &concatenate(NFA other) {
        auto empty_left = std::get_if<GroupToTokens>(&start_node.empty_edge);
        auto empty_right = std::get_if<GroupToTokens>(&other.start_node.empty_edge);

        auto connect_to_other = [&other](NFA::Node *node) {
            auto const &empty_edge = std::get<GroupToTokens>(node->empty_edge);

            for (auto const &[c, edges_for_c] : other.start_node.edges) {
                for (auto &[nbh, tokens] : edges_for_c) {
                    GroupToTokens new_tokens(empty_edge);
                    new_tokens.insert(tokens.cbegin(), tokens.cend());
                    node->edges[c].emplace(nbh, std::move(new_tokens));
                }
            }
        };

        for (auto lastpos_node : lastpos) {
            connect_to_other(lastpos_node);
        }

        if (empty_left) {
            connect_to_other(&start_node);
        }

        if (empty_right) {
            for (auto lastpos_node : lastpos) {
                std::get<GroupToTokens>(lastpos_node->empty_edge).insert(
                    empty_right->cbegin(), empty_right->cend()
                );
            }

            lastpos.splice(lastpos.cend(), other.lastpos);
        } else {
            for (auto lastpos_node : lastpos) {
                lastpos_node->empty_edge.emplace<std::monostate>();
            }

            std::swap(lastpos, other.lastpos);
            other.lastpos.clear();
        }

        if (empty_left && empty_right) {
            empty_left->merge(*empty_right);
        } else {
            start_node.empty_edge.emplace<std::monostate>();
        }

        all_nodes.splice(all_nodes.cend(), other.all_nodes);
        other.start_node.edges.clear();
        other.start_node.empty_edge.emplace<std::monostate>();
        return *this;
    }

    NFA &union_(NFA other) {
        auto empty_left = std::get_if<GroupToTokens>(&start_node.empty_edge);
        auto empty_right = std::get_if<GroupToTokens>(&other.start_node.empty_edge);

        for (auto &[c, other_start_edges_for_c] : other.start_node.edges) {
            start_node.edges[c].merge(other_start_edges_for_c);
        }

        if (empty_right) {
            if (empty_left) {
                empty_left->merge(*empty_right);
            } else {
                start_node.empty_edge = *empty_right;
            }
        }

        all_nodes.splice(all_nodes.cend(), other.all_nodes);
        lastpos.splice(lastpos.cend(), other.lastpos);
        other.start_node.edges.clear();
        other.start_node.empty_edge.emplace<std::monostate>();
        return *this;
    }

    NFA &star() {
        for (auto lastpos_node : lastpos) {
            auto &lastpos_node_empty_edge = std::get<GroupToTokens>(
                lastpos_node->empty_edge);

            for (auto &[c, start_edges_for_c] : start_node.edges) {
                auto &lastpos_node_edges_for_c = lastpos_node->edges[c];
                auto edge_it = lastpos_node_edges_for_c.begin();

                for (auto &[firstpos_node, firstpos_tokens] : start_edges_for_c) {
                    while (edge_it != lastpos_node_edges_for_c.end() &&
                           edge_it->first < firstpos_node) {
                        ++edge_it;
                    }

                    if (edge_it != lastpos_node_edges_for_c.end() && edge_it->first == firstpos_node) {
                        ++edge_it;
                    } else {
                        GroupToTokens new_tokens(lastpos_node_empty_edge);
                        new_tokens.insert(firstpos_tokens.cbegin(),
                                          firstpos_tokens.cend());
                        lastpos_node_edges_for_c.emplace_hint(
                            edge_it, firstpos_node, std::move(new_tokens)
                        );
                    }
                }
            }
        }

        if (std::holds_alternative<std::monostate>(start_node.empty_edge)) {
            start_node.empty_edge.emplace<GroupToTokens>();
        }

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
            if (std::holds_alternative<std::monostate>(start_node.empty_edge)) {
                start_node.empty_edge.emplace<GroupToTokens>();
            }
        }

        if (max == 1) {
            return *this;
        }

        std::vector<NFA> copies(max - 1, *this);

        if (std::holds_alternative<std::monostate>(start_node.empty_edge)) {
            for (int i = min - 1; i < max - 1; ++i) {
                copies[i].start_node.empty_edge.emplace<GroupToTokens>();
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

        std::vector<NFA> copies(min, *this); // TODO: is count correct?
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

    IterMatch(std::string text, int start, int end) : text(std::move(text)),
                                                      start(start), end(end) {}

    Match next() {

    }
};

#endif // NFA_CPP
