#ifndef NFA_CPP
#define NFA_CPP

#include <unordered_set>
#include <set>
#include <list>
#include <map>
#include <unordered_map>
#include <optional>
#include "ast.cpp"


struct Regex {
    struct NFA;

    struct Group {
        Node *node = nullptr; // pointer to AST tree node
    };

    enum class GroupToken {
        ENTER, LEAVE,
    };

    struct NFA {
        typedef std::unordered_map<Group *, std::vector<GroupToken>> GroupToTokens;

        struct Node {
            typedef std::unordered_map<char, std::map<Node *, GroupToTokens>> Edges;
            typedef std::optional<GroupToTokens> EmptyEdge;

            Edges edges;
            EmptyEdge empty_edge;
            int id; // TODO: can be removed
            char c; // TODO: can be removed

            explicit Node(int id) : id(id), c{} {}

            Node(int id, char c) : id(id), c(c) {}

            void connect_to_firstpos(NFA const &nfa) {
                // TODO: this might not work for union and start node
                // this method should be called only on nodes with EmptyEdge,
                // i.e. lastpos nodes or start node with empty edge
                for (auto const &[chr, nfa_start_edges_for_chr] : nfa.start_node.edges) {
                    auto &edges_for_chr = edges[chr];
                    auto edges_it = edges_for_chr.begin();

                    for (auto &[firstpos_node, firstpos_tokens] : nfa_start_edges_for_chr) {
                        while (edges_it != edges_for_chr.end() && edges_it->first < firstpos_node) {
                            ++edges_it;
                        }

                        if (edges_it != edges_for_chr.end() && edges_it->first == firstpos_node) {
                            ++edges_it;
                        } else {
                            GroupToTokens new_tokens(*empty_edge);

                            for (auto &[group, tokens] : firstpos_tokens) {
                                auto &new_tokens_for_group = new_tokens[group];
                                new_tokens_for_group.insert(
                                    new_tokens_for_group.cend(), tokens.cbegin(), tokens.cend());
                            }

                            edges_for_chr.emplace_hint(
                                edges_it, firstpos_node, std::move(new_tokens));
                        }
                    }
                }
            }

            void clear() {
                edges.clear();
                empty_edge.reset();
            }
        };

        Node start_node{0};
        std::list<Node *> all_nodes;
        std::list<Node *> lastpos;

        NFA() = default;

        NFA(NFA const &other) {
            std::unordered_map<Node const *, Node *> new_nodes;

            for (auto orig_node : other.all_nodes) {
                auto new_node = new Node(orig_node->id, orig_node->c);
                new_nodes[orig_node] = new_node;
                all_nodes.push_back(new_node);

                if (orig_node->empty_edge) {
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

        void connect_to_firstpos(NFA const &other) {
            for (auto node : lastpos) {
                node->connect_to_firstpos(other);
            }

            // TODO: do we want that here?
            if (start_node.empty_edge) {
                start_node.connect_to_firstpos(other);
            }
        }

        typedef std::pair<std::size_t, std::size_t> Match;
        typedef std::set<Match> Matches;
        typedef std::optional<Matches> MatchResult;

        MatchResult traverse(
            std::string const &text, std::size_t start, std::size_t end, Group *group
        ) {
            auto text_it = text.cbegin();
            auto text_end = text.cend();

            if (text_it == text_end) {
                if (start_node.empty_edge) {
                    return {{{0, 0}}};
                } else {
                    return {};
                }
            }

            struct Pawn {
                std::set<std::size_t> entered;
                Matches matches;
            };

            std::unordered_map<Node *, Pawn> old_pawns, new_pawns;
            old_pawns[&start_node];
            std::size_t pos = 0;

            while (!old_pawns.empty()) {
                char c = *text_it;
                ++text_it;

                for (auto &[old_node, old_pawn]: old_pawns) {
                    auto edges_for_c_it = old_node->edges.find(c);

                    if (edges_for_c_it != old_node->edges.cend()) {
                        auto edges_it = edges_for_c_it->second.begin();
                        auto edges_end = edges_for_c_it->second.end();

                        while (edges_it != edges_end) {
                            auto &[new_node, tokens] = *edges_it;
                            auto &new_pawn = new_pawns[new_node];
                            auto group_tokens_it = tokens.find(group);

                            if (group_tokens_it != tokens.cend()) {
                                for (auto token : group_tokens_it->second) {
                                    switch (token) {
                                        case GroupToken::ENTER: {
                                            new_pawn.entered.insert(pos);
                                            break;
                                        }
                                        case GroupToken::LEAVE: {
                                            for (auto begin : old_pawn.entered) {
                                                new_pawn.matches.emplace(begin, pos);
                                            }

                                            old_pawn.entered.clear();
                                            break;
                                        }
                                    }
                                }
                            }

                            if (++edges_it == edges_end) { // last edge on that character
                                new_pawn.entered.merge(old_pawn.entered);
                                new_pawn.matches.merge(old_pawn.matches);
                            } else {
                                new_pawn.entered.insert(
                                    old_pawn.entered.cbegin(), old_pawn.entered.cend()
                                );
                                new_pawn.matches.insert(
                                    old_pawn.matches.cbegin(), old_pawn.matches.cend()
                                );
                            }
                        }
                    }
                }

                if (text_it == text_end) {
                    bool matched = false;
                    Matches result;

                    for (auto &[node, pawn] : new_pawns) {
                        if (node->empty_edge) {
                            matched = true;

                            for (auto begin : pawn.entered) {
                                result.emplace(begin, pos);
                            }
                        }
                    }

                    if (matched) {
                        return result;
                    } else {
                        return {};
                    }
                }

                old_pawns.clear();
                std::swap(new_pawns, old_pawns);
                pos += 1;
            }

            return {};
        }

        static NFA from_ast(::Node *node) {
            switch (node->node_kind()) {
                case NOTHING: {
                    return for_nothing();
                }
                case EMPTY: {
                    return for_empty();
                }
                case CHAR: {
                    auto c = reinterpret_cast<CharNode *>(node);
                    return for_char(c->id, c->chr);
                }
                case DOT: {
                    auto dot = reinterpret_cast<DotNode *>(node);
                    return for_dot(dot->id);
                }
                case SMALL_D: {
                    auto small_d = reinterpret_cast<SmallDNode *>(node);
                    return for_small_d(small_d->id);
                }
                case BIG_D: {
                    throw std::runtime_error("Not implemented");
                }
                case SMALL_S: {
                    auto small_s = reinterpret_cast<SmallSNode *>(node);
                    return for_small_s(small_s->id);
                }
                case BIG_S: {
                    throw std::runtime_error("Not implemented");
                }
                case SMALL_W: {
                    auto small_w = reinterpret_cast<SmallWNode *>(node);
                    return for_small_w(small_w->id);
                }
                case BIG_W: {
                    throw std::runtime_error("Not implemented");
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
            nfa.start_node.empty_edge.emplace();
            return nfa;
        }

        static NFA for_char(int id, char c) {
            NFA nfa;
            auto node = new Node(id, c);
            node->empty_edge.emplace();
            nfa.all_nodes.push_back(node);
            nfa.lastpos.push_back(node);
            nfa.start_node.edges[c].insert({node, {}});
            return nfa;
        }

        static NFA for_dot(int id) {
            NFA nfa;
            auto node = new Node(id);
            node->empty_edge.emplace();
            nfa.all_nodes.push_back(node);
            nfa.lastpos.push_back(node);

            for (int i = 0; i < 128; ++i) {
                char c = static_cast<char>(i);
                nfa.start_node.edges[c].insert({node, {}});
            }

            return nfa;
        }

        static NFA for_small_d(int id) {
            NFA nfa;
            auto node = new Node(id);
            node->empty_edge.emplace();
            nfa.all_nodes.push_back(node);
            nfa.lastpos.push_back(node);

            for (signed char c = '0'; c <= '9'; ++c) {
                nfa.start_node.edges[c].insert({node, {}});
            }

            return nfa;
        }

        static NFA for_small_s(int id) {
            NFA nfa;
            auto node = new Node(id);
            node->empty_edge.emplace();
            nfa.all_nodes.push_back(node);
            nfa.lastpos.push_back(node);

            for (char c : " \n\t\n\r\f\v") {
                nfa.start_node.edges[c].insert({node, {}});
            }

            return nfa;
        }

        static NFA for_small_w(int id) {
            NFA nfa;
            auto node = new Node(id);
            node->empty_edge.emplace();
            nfa.all_nodes.push_back(node);
            nfa.lastpos.push_back(node);

            for (char c : "_0123456789"
                          "abcdefghijklmnopqrstuvwxyz"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ") {
                nfa.start_node.edges[c].insert({node, {}});
            }

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
            if (start_node.empty_edge) {
                auto &tokens = (*start_node.empty_edge)[group];
                tokens.push_back(GroupToken::ENTER);
                tokens.push_back(GroupToken::LEAVE);
            }

            for (auto &[c, edges_for_c] : start_node.edges) {
                for (auto &[nbh, tokens] : edges_for_c) {
                    tokens[group].push_back(GroupToken::ENTER);
                }
            }

            for (auto lastpos_node : lastpos) {
                (*lastpos_node->empty_edge)[group].push_back(GroupToken::LEAVE);
            }

            return *this;
        }

        NFA &qmark() {
            if (!start_node.empty_edge) {
                start_node.empty_edge.emplace();
            }

            return *this;
        }

        NFA &concatenate(NFA other) {
            connect_to_firstpos(other);

            if (other.start_node.empty_edge) {
                lastpos.splice(lastpos.cend(), other.lastpos);
            } else {
                for (auto lastpos_node : lastpos) {
                    lastpos_node->empty_edge.reset();
                }

                lastpos.clear();
                std::swap(lastpos, other.lastpos);
            }

            all_nodes.splice(all_nodes.cend(), other.all_nodes);
            other.start_node.clear();
            return *this;
        }

        NFA &union_(NFA other) {
            for (auto &[c, other_start_edges_for_c] : other.start_node.edges) {
                start_node.edges[c].merge(other_start_edges_for_c);
            }

            if (other.start_node.empty_edge) {
                if (start_node.empty_edge) {
                    start_node.empty_edge->merge(*other.start_node.empty_edge);
                } else {
                    start_node.empty_edge = *other.start_node.empty_edge;
                }
            }

            all_nodes.splice(all_nodes.cend(), other.all_nodes);
            lastpos.splice(lastpos.cend(), other.lastpos);
            other.start_node.edges.clear();
            other.start_node.empty_edge.reset();
            return *this;
        }

        NFA &star() {
            connect_to_firstpos(*this);

            if (!start_node.empty_edge) {
                start_node.empty_edge.emplace();
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
                if (!start_node.empty_edge) {
                    start_node.empty_edge.emplace();
                }
            }

            if (max == 1) {
                return *this;
            }

            std::vector<NFA> copies(max - 1, *this);

            if (!start_node.empty_edge) {
                for (int i = min - 1; i < max - 1; ++i) {
                    copies[i].start_node.empty_edge.emplace();
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

        NFA &percent(NFA other) {
            connect_to_firstpos(other);
            other.connect_to_firstpos(*this);
            all_nodes.splice(all_nodes.cend(), other.all_nodes);
            other.lastpos.clear();
            other.start_node.clear();
            return *this;
        }
    };

    std::list<Group> numbered_groups;
    std::map<std::string, Group> named_groups;
    std::optional<std::string> regex;
    std::optional<NFA> nfaa;

    explicit Regex(std::string regex) : regex(regex) {}

    Regex(Regex const &other) = default;

    Regex(Regex &&) = default;

    Regex concat(Regex other) const {
        // concat ASTs, create new regex with just that AST, return it
    }
};

#endif // NFA_CPP
