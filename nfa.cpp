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
 *
 * Krawedzie w NFA podzielimy na typy:
 * - entering (ze startu)
 * - leaving (do konca)
 * - internal (nie leaving, nie entering)
 * - backing (z lastpos do firstpos)
 * - normal (internal, ale nie backing)
 * entering == backing jest max. jedna, tylko jezeli NFA akceptuje pusty string
 */

struct Group {
    Node *node = nullptr; // pointer to AST tree node
};

enum class GroupToken {
    ENTER, LEAVE,
};

std::list<Group> groups;

struct NFA {
    // TODO: maybe make the value an encapsulated 2-element array?
    typedef std::unordered_map<Group *, std::vector<GroupToken>> GroupToTokens;
    struct Node;

    struct Node {
        std::map<Node *, GroupToTokens> edges;
        std::variant<std::monostate, GroupToTokens> empty_edge;
        int id;
        char c;

        explicit Node(int id) : id(id), c{} {}

        Node(int id, char c) : id(id), c(c) {}
    };

    Node start_node{0};
    std::list<Node *> all_nodes;
    std::list<Node *> lastpos;  // Maybe it should hold only internal nodes, not start?

    NFA() = default;

    NFA(NFA const &other) {
//        start_node.accepting = other.start_node.accepting;
//        std::unordered_map<Node const *, Node *> new_nodes;
//        auto all_nodes_it = all_nodes.cbefore_begin();
//        auto end_nodes_it = end_nodes.cbefore_begin();
//
//        for (auto node : other.all_nodes) {
//            auto new_node = new Node(node->id, node->c, node->accepting);
//            new_nodes[node] = new_node;
//            all_nodes.insert_after(all_nodes_it, new_node);
//            ++all_nodes_it;
//
//            if (node->accepting) {
//                end_nodes.insert_after(end_nodes_it, new_node);
//                ++end_nodes_it;
//            }
//        }
//
//        for (auto nbh : other.start_node.edges) {
//            start_node.edges.insert(new_nodes[nbh]);
//        }
//
//        for (auto node : other.all_nodes) {
//            auto curr_node = new_nodes[node];
//
//            for (auto nbh : node->edges) {
//                curr_node->edges.insert(new_nodes[nbh]);
//            }
//        }
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

    static NFA from_ast(::Node *node) {
        // TODO: zrobic z tego visitora? i zrobic wierzcholki AST jako std::variant?
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
//        nfa.lastpos.push_back(&nfa.start_node);
        return nfa;
    }

    static NFA for_char(int id, char c) {
        NFA nfa;
        auto node = new Node(id, c);
        node->empty_edge.emplace<GroupToTokens>();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);
        return nfa;
    }

    // TODO: named vs. numbered group vs. unnamed
    NFA &for_group() {
        groups.push_back({}); // TODO: PLACEHOLDER

        if (auto empty_edge = std::get_if<GroupToTokens>(&start_node.empty_edge)) {
            auto &tokens = (*empty_edge)[&groups.back()];
            tokens.push_back(GroupToken::ENTER);
            tokens.push_back(GroupToken::LEAVE);
        }

        for (auto &[_, tokens] : start_node.edges) {
            tokens[&groups.back()].push_back(GroupToken::ENTER);
        }

        for (auto lastpos_node : lastpos) {
            std::get<GroupToTokens>(lastpos_node->empty_edge)[&groups.back()]
                .push_back(GroupToken::LEAVE);
        }

        return *this;
    }

    NFA &star() {
        for (auto lastpos_node : lastpos) {
            auto const &empty_edge_tokens = std::get<GroupToTokens>(
                lastpos_node->empty_edge);
            auto edge_it = lastpos_node->edges.begin();

            for (auto &[firstpos_node, firstpos_tokens] : start_node.edges) {
                while (edge_it != lastpos_node->edges.end() &&
                       edge_it->first <= firstpos_node) {
                    ++edge_it;
                }

                if (edge_it != lastpos_node->edges.end()) {
                    lastpos_node->edges.emplace_hint(
                        edge_it, firstpos_node, empty_edge_tokens
                    );
                }
            }
        }

        if (std::holds_alternative<std::monostate>(start_node.empty_edge)) {
            start_node.empty_edge.emplace<GroupToTokens>();
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
        auto empty_left =std::get_if<GroupToTokens>(&start_node.empty_edge);
        auto empty_right =std::get_if<GroupToTokens>(&other.start_node.empty_edge);

//        auto connect_to_other = [&other](NFA::Node *node) {
//            auto const &empty_edge = std::get<GroupToTokens>(node->empty_edge);
//
//            for (auto const &[nbh, tokens] : other.start_node.edges) {
//                GroupToTokens new_tokens(empty_edge);
//                new_tokens.insert(tokens.begin(), tokens.end());
//                node->edges.emplace(nbh, std::move(new_tokens));
//            }
//        };

        for (auto lastpos_node : lastpos) {
            auto const &empty_lastpos = std::get<GroupToTokens>(
                lastpos_node->empty_edge);

            for (auto const &[nbh, tokens] : other.start_node.edges) {
                GroupToTokens new_tokens(empty_lastpos);
                new_tokens.insert(tokens.begin(), tokens.end());
                lastpos_node->edges.emplace(nbh, std::move(new_tokens));
            }
        }

        if (empty_left) {
            for (auto const &[nbh, tokens] : other.start_node.edges) {
                GroupToTokens new_tokens(*empty_left);
                new_tokens.insert(tokens.begin(), tokens.end());
                start_node.edges.emplace(nbh, std::move(new_tokens));
            }
        }

        if (empty_right) {
            for (auto lastpos_node : lastpos) {
                std::get<GroupToTokens>(lastpos_node->empty_edge).insert(
                    empty_right->begin(), empty_right->end()
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

        /////////////////////////////////////////

        all_nodes.splice(all_nodes.end(), other.all_nodes);

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
