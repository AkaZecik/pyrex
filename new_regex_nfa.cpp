#include "new_regex.h"

/* TODO
 *  - handle memory errors in functions representing operators
 */

namespace pyrex {
    void Regex::NFA::Node::connect_to_firstpos(NFA const &nfa) {
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
                    GroupToTokens new_tokens(*epsilon_edge);

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

    void Regex::NFA::Node::clear() {
        edges.clear();
        epsilon_edge.reset();
    }

    Regex::NFA::NFA(NFA const &other) {
        std::unordered_map<Node const *, Node *> new_nodes;

        for (auto orig_node : other.all_nodes) {
            auto new_node = new Node();
            new_nodes[orig_node] = new_node;
            all_nodes.push_back(new_node);

            if (orig_node->epsilon_edge) {
                lastpos.push_back(new_node);
            }
        }

        for (auto[orig_node, new_node] : new_nodes) {
            new_node->epsilon_edge = orig_node->epsilon_edge;

            for (auto &[c, orig_node_edges_for_c] : orig_node->edges) {
                auto &new_node_edges_for_c = new_node->edges[c];

                for (auto &[nbh, tokens]: orig_node_edges_for_c) {
                    new_node_edges_for_c[new_nodes[nbh]] = tokens;
                }
            }
        }

        start_node.epsilon_edge = other.start_node.epsilon_edge;

        for (auto &[c, orig_start_node_edges_for_c] : other.start_node.edges) {
            auto &start_node_edges_for_c = start_node.edges[c];

            for (auto &[nbh, tokens] : orig_start_node_edges_for_c) {
                start_node_edges_for_c[new_nodes[nbh]] = tokens;
            }
        }
    }

    Regex::NFA::~NFA() noexcept {
        for (auto node : all_nodes) {
            delete node;
        }
    }

    void Regex::NFA::connect_to_firstpos(NFA const &other) {
        for (auto node : lastpos) {
            node->connect_to_firstpos(other);
        }

        // TODO: do we want that here?
        if (start_node.epsilon_edge) {
            start_node.connect_to_firstpos(other);
        }
    }

    Regex::NFA Regex::NFA::from_ast(AST const &ast) {
        return from_ast_node(ast.get_root());
    }

    Regex::NFA Regex::NFA::from_ast_node(std::shared_ptr<AST::Node> const &ast_node) {
        switch (ast_node->kind()) {
            case AST::Node::Kind::NOTHING: {
                return for_nothing();
            }
            case AST::Node::Kind::EMPTY: {
                return for_empty();
            }
            case AST::Node::Kind::CHAR: {
                auto chr_node = std::dynamic_pointer_cast<AST::CharNode>(ast_node);
                return for_char(chr_node->chr);
            }
            case AST::Node::Kind::DOT: {
                auto dot_node = std::dynamic_pointer_cast<AST::DotNode>(ast_node);
                return for_dot();
            }
            case AST::Node::Kind::SMALL_D: {
                return for_small_d();
            }
            case AST::Node::Kind::SMALL_S: {
                return for_small_s();
            }
            case AST::Node::Kind::SMALL_W: {
                return for_small_w();
            }
            case AST::Node::Kind::NUMBERED_CGROUP: {
                auto group_node = std::dynamic_pointer_cast<AST::NumberedCGroupNode>(ast_node);
                return std::move(from_ast_node(group_node->operand).for_numbered_cgroup());
            }
            case AST::Node::Kind::NAMED_CGROUP: {
                auto group_node = std::dynamic_pointer_cast<AST::NamedCGroupNode>(ast_node);
                return std::move(from_ast_node(group_node->operand).for_named_cgroup());
            }
            case AST::Node::Kind::NON_CGROUP: {
                auto group_node = std::dynamic_pointer_cast<AST::NonCGroupNode>(ast_node);
                return std::move(from_ast_node(group_node->operand).for_non_cgroup());
            }
            case AST::Node::Kind::STAR: {
                auto star_node = std::dynamic_pointer_cast<AST::StarNode>(ast_node);
                return std::move(from_ast_node(star_node->operand).star());
            }
            case AST::Node::Kind::PLUS: {
                auto plus_node = std::dynamic_pointer_cast<AST::PlusNode>(ast_node);
                return std::move(from_ast_node(plus_node->operand).plus());
            }
            case AST::Node::Kind::POWER: {
                auto power_node = std::dynamic_pointer_cast<AST::PowerNode>(ast_node);
                return std::move(from_ast_node(power_node->operand).power(power_node->power));
            }
            case AST::Node::Kind::MIN: {
                auto min_node = std::dynamic_pointer_cast<AST::MinNode>(ast_node);
                return std::move(from_ast_node(min_node->operand).min(min_node->min));
            }
            case AST::Node::Kind::MAX: {
                auto max_node = std::dynamic_pointer_cast<AST::MaxNode>(ast_node);
                return std::move(from_ast_node(max_node->operand).max(max_node->max));
            }
            case AST::Node::Kind::RANGE: {
                auto range_node = std::dynamic_pointer_cast<AST::RangeNode>(ast_node);
                return std::move(
                    from_ast_node(range_node->operand).range(range_node->min, range_node->max));
            }
            case AST::Node::Kind::QMARK: {
                auto qmark_node = std::dynamic_pointer_cast<AST::QMarkNode>(ast_node);
                return std::move(from_ast_node(qmark_node->operand).qmark());
            }
            case AST::Node::Kind::CONCAT: {
                auto concat_node = std::dynamic_pointer_cast<AST::ConcatNode>(ast_node);
                return std::move(from_ast_node(concat_node->left_operand).concatenate(
                    from_ast_node(concat_node->right_operand)
                ));
            }
            case AST::Node::Kind::UNION: {
                auto union_node = std::dynamic_pointer_cast<AST::UnionNode>(ast_node);
                return std::move(from_ast_node(union_node->left_operand).union_(
                    from_ast_node(union_node->right_operand)
                ));
            }
            case AST::Node::Kind::PERCENT: {
                auto percent_node = std::dynamic_pointer_cast<AST::PercentNode>(ast_node);
                return std::move(from_ast_node(percent_node->left_operand).percent(
                    from_ast_node(percent_node->right_operand)
                ));
            }
            default:
                throw std::runtime_error("Unknown node kind");
        }
    }

    Regex::NFA Regex::NFA::for_nothing() {
        return {};
    }

    Regex::NFA Regex::NFA::for_empty() {
        NFA nfa;
        nfa.start_node.epsilon_edge.emplace();
        return nfa;
    }

    Regex::NFA Regex::NFA::for_char(char chr) {
        NFA nfa;
        auto node = new Node();
        node->epsilon_edge.emplace();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);
        nfa.start_node.edges[chr].insert({node, {}});
        return nfa;
    }

    Regex::NFA Regex::NFA::for_dot() {
        NFA nfa;
        auto node = new Node();
        node->epsilon_edge.emplace();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);

        for (int i = 0; i < 128; ++i) {
            char chr = static_cast<char>(i);
            nfa.start_node.edges[chr].insert({node, {}});
        }

        return nfa;
    }

    Regex::NFA Regex::NFA::for_small_d() {
        NFA nfa;
        auto node = new Node();
        node->epsilon_edge.emplace();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);

        for (signed char chr = '0'; chr <= '9'; ++chr) {
            nfa.start_node.edges[chr].insert({node, {}});
        }

        return nfa;
    }

    Regex::NFA Regex::NFA::for_small_s() {
        NFA nfa;
        auto node = new Node();
        node->epsilon_edge.emplace();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);

        for (char chr : " \n\t\n\r\f\v") {
            nfa.start_node.edges[chr].insert({node, {}});
        }

        return nfa;
    }

    Regex::NFA Regex::NFA::for_small_w() {
        NFA nfa;
        auto node = new Node();
        node->epsilon_edge.emplace();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);

        for (char chr : "_0123456789"
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ") {
            nfa.start_node.edges[chr].insert({node, {}});
        }

        return nfa;
    }

    Regex::NFA &Regex::NFA::for_non_cgroup() {
        return *this;
    }

    Regex::NFA &Regex::NFA::for_numbered_cgroup() {
        Group *numbered_group = nullptr; // TODO
        for_group(numbered_group);
        return *this;
    }

    Regex::NFA &Regex::NFA::for_named_cgroup() {
        Group *named_group = nullptr; // TODO
        for_group(named_group);
        return *this;
    }

    Regex::NFA &Regex::NFA::for_group(Group *group) {
        if (start_node.epsilon_edge) {
            auto &tokens = (*start_node.epsilon_edge)[group];
            tokens.push_back(GroupToken::ENTER);
            tokens.push_back(GroupToken::LEAVE);
        }

        for (auto &[c, edges_for_c] : start_node.edges) {
            for (auto &[nbh, tokens] : edges_for_c) {
                tokens[group].push_back(GroupToken::ENTER);
            }
        }

        for (auto lastpos_node : lastpos) {
            (*lastpos_node->epsilon_edge)[group].push_back(GroupToken::LEAVE);
        }

        return *this;
    }

    Regex::NFA &Regex::NFA::qmark() {
        if (!start_node.epsilon_edge) {
            start_node.epsilon_edge.emplace();
        }

        return *this;
    }

    Regex::NFA &Regex::NFA::star() {
        connect_to_firstpos(*this);

        if (!start_node.epsilon_edge) {
            start_node.epsilon_edge.emplace();
        }

        return *this;
    }

    Regex::NFA &Regex::NFA::plus() {
        concatenate(std::move(NFA(*this).star()));
        return *this;
    }

    Regex::NFA &Regex::NFA::power(int n) {
        range(n, n);
        return *this;
    }

    Regex::NFA &Regex::NFA::min(int min) {
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

    Regex::NFA &Regex::NFA::max(int n) {
        range(0, n);
        return *this;
    }

    Regex::NFA &Regex::NFA::range(int min, int max) {
        if (max == 0) {
            *this = for_empty();
            return *this;
        }

        if (min == 0) {
            if (!start_node.epsilon_edge) {
                start_node.epsilon_edge.emplace();
            }
        }

        if (max == 1) {
            return *this;
        }

        std::vector<NFA> copies(max - 1, *this);

        if (!start_node.epsilon_edge) {
            for (int i = min - 1; i < max - 1; ++i) {
                copies[i].start_node.epsilon_edge.emplace();
            }
        }

        for (NFA &copy : copies) {
            concatenate(std::move(copy));
        }

        return *this;
    }

    Regex::NFA &Regex::NFA::concatenate(NFA other) {
        connect_to_firstpos(other);

        if (other.start_node.epsilon_edge) {
            lastpos.splice(lastpos.cend(), other.lastpos);
        } else {
            for (auto lastpos_node : lastpos) {
                lastpos_node->epsilon_edge.reset();
            }

            lastpos.clear();
            std::swap(lastpos, other.lastpos);
        }

        all_nodes.splice(all_nodes.cend(), other.all_nodes);
        other.start_node.clear();
        return *this;
    }

    Regex::NFA &Regex::NFA::union_(NFA other) {
        for (auto &[c, other_start_edges_for_c] : other.start_node.edges) {
            start_node.edges[c].merge(other_start_edges_for_c);
        }

        if (other.start_node.epsilon_edge) {
            if (start_node.epsilon_edge) {
                start_node.epsilon_edge->merge(*other.start_node.epsilon_edge);
            } else {
                start_node.epsilon_edge = *other.start_node.epsilon_edge;
            }
        }

        all_nodes.splice(all_nodes.cend(), other.all_nodes);
        lastpos.splice(lastpos.cend(), other.lastpos);
        other.start_node.edges.clear();
        other.start_node.epsilon_edge.reset();
        return *this;
    }

    Regex::NFA &Regex::NFA::percent(NFA other) {
        connect_to_firstpos(other);
        other.connect_to_firstpos(*this);
        all_nodes.splice(all_nodes.cend(), other.all_nodes);
        other.lastpos.clear();
        other.start_node.clear();
        return *this;
    }
}