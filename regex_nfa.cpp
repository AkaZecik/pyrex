#include "regex.h"

/* TODO
 *  - handle memory errors in functions representing operators
 *  - possible optimizations on epsilon edges on power, min, max, range, concat operators
 *  - it's possible to predict size of NFA ahead of time
 */

namespace pyrex {
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

            for (auto &[chr, orig_node_nbhs_for_chr] : orig_node->edges) {
                auto &new_node_nbhs_for_chr = new_node->edges[chr];

                for (auto nbh : orig_node_nbhs_for_chr) {
                    new_node_nbhs_for_chr.insert(new_nodes[nbh]);
                }
            }

            for (auto &[nbh, groups] : orig_node->node_to_groups) {
                new_node->node_to_groups[new_nodes[nbh]] = groups;
            }
        }

        start_node.epsilon_edge = other.start_node.epsilon_edge;

        for (auto &[chr, orig_start_node_nbhs_for_chr] : other.start_node.edges) {
            auto &start_node_nbhs_for_chr = start_node.edges[chr];

            for (auto nbh : orig_start_node_nbhs_for_chr) {
                start_node_nbhs_for_chr.insert(new_nodes[nbh]);
            }
        }

        for (auto &[nbh, groups] : other.start_node.node_to_groups) {
            start_node.node_to_groups[new_nodes[nbh]] = groups;
        }

        size = other.size;
    }

    Regex::NFA::~NFA() noexcept {
        for (auto node : all_nodes) {
            delete node;
        }
    }

    Regex::NFA Regex::NFA::from_ast(AST const &ast) {
        auto numbered_cgroups_it = ast.get_numbered_cgroups().cbegin();
        return from_ast_node(
            ast.get_root(), numbered_cgroups_it, ast.get_named_cgroups()
        );
    }

    Regex::NFA Regex::NFA::from_ast_node(
        std::shared_ptr<AST::Node> const &ast_node,
        AST::NumberedCGroups::const_iterator &curr_numbered_group,
        AST::NamedCGroups const &named_cgroups
    ) {
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
                auto &group = *curr_numbered_group;
                return std::move(from_ast_node(
                    group_node->operand, ++curr_numbered_group, named_cgroups
                ).for_group(&group));
            }
            case AST::Node::Kind::NAMED_CGROUP: {
                auto group_node = std::dynamic_pointer_cast<AST::NamedCGroupNode>(ast_node);
                auto &group = named_cgroups.find(group_node->name)->second;
                return std::move(from_ast_node(
                    group_node->operand, curr_numbered_group, named_cgroups
                ).for_group(&group));
            }
            case AST::Node::Kind::NON_CGROUP: {
                auto group_node = std::dynamic_pointer_cast<AST::NonCGroupNode>(ast_node);
                return std::move(from_ast_node(
                    group_node->operand, curr_numbered_group, named_cgroups
                ));
            }
            case AST::Node::Kind::STAR: {
                auto star_node = std::dynamic_pointer_cast<AST::StarNode>(ast_node);
                return std::move(from_ast_node(
                    star_node->operand, curr_numbered_group, named_cgroups
                ).star());
            }
            case AST::Node::Kind::PLUS: {
                auto plus_node = std::dynamic_pointer_cast<AST::PlusNode>(ast_node);
                return std::move(from_ast_node(
                    plus_node->operand, curr_numbered_group, named_cgroups
                ).plus());
            }
            case AST::Node::Kind::POWER: {
                auto power_node = std::dynamic_pointer_cast<AST::PowerNode>(ast_node);
                return std::move(from_ast_node(
                    power_node->operand, curr_numbered_group, named_cgroups
                ).power(power_node->power));
            }
            case AST::Node::Kind::MIN: {
                auto min_node = std::dynamic_pointer_cast<AST::MinNode>(ast_node);
                return std::move(from_ast_node(
                    min_node->operand, curr_numbered_group, named_cgroups
                ).min(min_node->min));
            }
            case AST::Node::Kind::MAX: {
                auto max_node = std::dynamic_pointer_cast<AST::MaxNode>(ast_node);
                return std::move(from_ast_node(
                    max_node->operand, curr_numbered_group, named_cgroups
                ).max(max_node->max));
            }
            case AST::Node::Kind::RANGE: {
                auto range_node = std::dynamic_pointer_cast<AST::RangeNode>(ast_node);
                return std::move(
                    from_ast_node(
                        range_node->operand, curr_numbered_group, named_cgroups
                    ).range(range_node->min, range_node->max));
            }
            case AST::Node::Kind::QMARK: {
                auto qmark_node = std::dynamic_pointer_cast<AST::QMarkNode>(ast_node);
                return std::move(from_ast_node(
                    qmark_node->operand, curr_numbered_group, named_cgroups
                ).qmark());
            }
            case AST::Node::Kind::CONCAT: {
                auto concat_node = std::dynamic_pointer_cast<AST::ConcatNode>(ast_node);
                return std::move(from_ast_node(
                    concat_node->left_operand, curr_numbered_group, named_cgroups
                ).concatenate(from_ast_node(
                    concat_node->right_operand, curr_numbered_group, named_cgroups
                )));
            }
            case AST::Node::Kind::UNION: {
                auto union_node = std::dynamic_pointer_cast<AST::UnionNode>(ast_node);
                return std::move(from_ast_node(
                    union_node->left_operand, curr_numbered_group, named_cgroups
                ).union_(from_ast_node(
                    union_node->right_operand, curr_numbered_group, named_cgroups
                )));
            }
            case AST::Node::Kind::PERCENT: {
                auto percent_node = std::dynamic_pointer_cast<AST::PercentNode>(ast_node);
                return std::move(from_ast_node(
                    percent_node->left_operand, curr_numbered_group, named_cgroups
                ).percent(from_ast_node(
                    percent_node->right_operand, curr_numbered_group, named_cgroups
                )));
            }
            default:
                throw std::runtime_error("Unknown AST node kind");
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
        nfa.start_node.edges[chr].insert({node});
        nfa.size = 1;
        return nfa;
    }

    Regex::NFA Regex::NFA::for_dot() {
        NFA nfa;
        auto node = new Node();
        node->epsilon_edge.emplace();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);
        nfa.size = 1;

        for (int i = 0; i < 128; ++i) {
            char chr = static_cast<char>(i);
            nfa.start_node.edges[chr].insert({node});
        }

        return nfa;
    }

    Regex::NFA Regex::NFA::for_small_d() {
        NFA nfa;
        auto node = new Node();
        node->epsilon_edge.emplace();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);
        nfa.size = 1;

        for (signed char chr = '0'; chr <= '9'; ++chr) {
            nfa.start_node.edges[chr].insert({node});
        }

        return nfa;
    }

    Regex::NFA Regex::NFA::for_small_s() {
        NFA nfa;
        auto node = new Node();
        node->epsilon_edge.emplace();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);
        nfa.size = 1;

        for (char chr : " \n\t\n\r\f\v") {
            nfa.start_node.edges[chr].insert({node});
        }

        return nfa;
    }

    Regex::NFA Regex::NFA::for_small_w() {
        NFA nfa;
        auto node = new Node();
        node->epsilon_edge.emplace();
        nfa.all_nodes.push_back(node);
        nfa.lastpos.push_back(node);
        nfa.size = 1;

        for (char chr : "_0123456789"
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ") {
            nfa.start_node.edges[chr].insert({node});
        }

        return nfa;
    }

    Regex::NFA &Regex::NFA::for_group(AST::Group const *group) {
        if (start_node.epsilon_edge) {
            auto &tokens = (*start_node.epsilon_edge)[group];
            tokens.push_back(GroupToken::ENTER);
            tokens.push_back(GroupToken::LEAVE);
        }

        for (auto &[node, groups] : start_node.node_to_groups) {
            groups[group].push_back(GroupToken::ENTER);
        }

        for (auto node : lastpos) {
            (*node->epsilon_edge)[group].push_back(GroupToken::LEAVE);
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
        for (auto lastpos_node : lastpos) {
            for (auto const &[chr, firstpos_nodes] : start_node.edges) {
                auto &lastpos_edges_for_chr = lastpos_node->edges[chr];
                auto lastpos_edge_it = lastpos_edges_for_chr.cbegin();

                for (auto firstpos_node : firstpos_nodes) {
                    while (lastpos_edge_it != lastpos_edges_for_chr.cend() &&
                           *lastpos_edge_it < firstpos_node) {
                        ++lastpos_edge_it;
                    }

                    if (lastpos_edge_it != lastpos_edges_for_chr.cend() &&
                        *lastpos_edge_it == firstpos_node) {
                        ++lastpos_edge_it;
                    } else {
                        lastpos_edges_for_chr.emplace_hint(lastpos_edge_it, lastpos_node);
                        auto &firstpos_groups = start_node.node_to_groups[firstpos_node];
                        auto[new_tokens_it, _] = lastpos_node->node_to_groups
                            .emplace(firstpos_node, *lastpos_node->epsilon_edge);
                        new_tokens_it->second.insert(firstpos_groups.cbegin(),
                                                     firstpos_groups.cend());
                    }
                }
            }
        }

        if (!start_node.epsilon_edge) {
            start_node.epsilon_edge.emplace();
        }

        return *this;
    }

    Regex::NFA &Regex::NFA::plus() {
        if (2 * size > MAX_NUM_OF_NODES) {
            throw std::runtime_error("Maximum number of NFA nodes reached");
        }

        return concatenate(std::move(NFA(*this).star()));
    }

    Regex::NFA &Regex::NFA::power(int n) {
        return range(n, n);
    }

    Regex::NFA &Regex::NFA::min(int min) {
        if (min == 0) {
            return star();
        }

        if ((min + 1) * size > MAX_NUM_OF_NODES) {
            throw std::runtime_error("Maximum number of NFA nodes reached");
        }

        std::vector<NFA> copies(min, *this);
        copies.back().star();

        for (NFA &copy : copies) {
            concatenate(std::move(copy));
        }

        return *this;
    }

    Regex::NFA &Regex::NFA::max(int n) {
        return range(0, n);
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

        if (max * size > MAX_NUM_OF_NODES) {
            throw std::runtime_error("Maximum number of NFA nodes reached");
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
        if (size + other.size > NFA::MAX_NUM_OF_NODES) {
            throw std::runtime_error("Maximum number of NFA nodes reached");
        }

        auto connect_to_other = [&other](NFA::Node *node) {
            for (auto const &[chr, firstpos_nodes] : other.start_node.edges) {
                node->edges[chr].insert(firstpos_nodes.cbegin(), firstpos_nodes.cend());
            }

            for (auto &[firstpos_node, other_groups] : other.start_node.node_to_groups) {
                auto[groups_it, _] = node->node_to_groups
                    .emplace(firstpos_node, *node->epsilon_edge);

                for (auto &[group, other_tokens] : other_groups) {
                    auto &tokens = groups_it->second[group];
                    tokens.insert(tokens.cend(), other_tokens.cbegin(), other_tokens.cend());
                }
            }

            if (other.start_node.epsilon_edge) {
                for (auto &[group, other_tokens] : *other.start_node.epsilon_edge) {
                    auto &tokens = (*node->epsilon_edge)[group];
                    tokens.insert(tokens.cend(), other_tokens.cbegin(), other_tokens.cend());
                }
            } else {
                node->epsilon_edge.reset();
            }
        };

        for (auto lastpos_node : lastpos) {
            connect_to_other(lastpos_node);
        }

        if (start_node.epsilon_edge) {
            connect_to_other(&start_node);
        }

        if (other.start_node.epsilon_edge) {
            lastpos.splice(lastpos.cend(), other.lastpos);
        } else {
            lastpos.clear();
            std::swap(lastpos, other.lastpos);
        }

        all_nodes.splice(all_nodes.cend(), other.all_nodes);
        size += other.size;
        other.start_node.clear();
        other.size = 0;
        return *this;
    }

    Regex::NFA &Regex::NFA::union_(NFA other) {
        if (size + other.size > NFA::MAX_NUM_OF_NODES) {
            throw std::runtime_error("Maximum number of NFA nodes reached");
        }

        for (auto &[chr, other_firstpos_nodes] : other.start_node.edges) {
            start_node.edges[chr].merge(other_firstpos_nodes);
        }

        start_node.node_to_groups.merge(other.start_node.node_to_groups);

        if (other.start_node.epsilon_edge) {
            if (start_node.epsilon_edge) {
                start_node.epsilon_edge->merge(*other.start_node.epsilon_edge);
            } else {
                start_node.epsilon_edge = *other.start_node.epsilon_edge;
            }
        }

        all_nodes.splice(all_nodes.cend(), other.all_nodes);
        lastpos.splice(lastpos.cend(), other.lastpos);
        size += other.size;
        other.start_node.clear();
        other.size = 0;
        return *this;
    }

    Regex::NFA &Regex::NFA::percent(NFA other) {
        if (2 * size + other.size > MAX_NUM_OF_NODES) {
            throw std::runtime_error("Maximum number of NFA nodes reached");
        }

        return concatenate(std::move(other.concatenate(*this).star()));
    }
}