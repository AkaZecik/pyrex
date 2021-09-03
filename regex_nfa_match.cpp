#include "regex.h"
#include <iostream>

/* TODO:
 *  - what should happen if text contains characters outside of ASCII?
 *  - move MatchResult to Regex
 */

namespace pyrex {
    // TODO: it might be possible to extend NFA::match with first and last match of each group
    //  by tracking only two matches on each pawn
    // TODO: maybe it can be refactored into findall
    bool Regex::NFA::match(const std::string &text, MatchType match_type) const {
        if (text.empty()) {
            return start_node.epsilon_edge.has_value();
        } else if (start_node.epsilon_edge && (
            match_type == MatchType::LMATCH ||
            match_type == MatchType::RMATCH ||
            match_type == MatchType::AMATCH
        )) {
            return true;
        }

        std::vector<Node const *> old_state, new_state;
        std::unordered_map<Node const *, bool> already_on;
        bool accepting;
        auto text_it = text.cbegin();

        for (auto node : all_nodes) {
            already_on[node] = false;
        }

        old_state.push_back(&start_node);

        while (!old_state.empty()) {
            char chr = *text_it;
            ++text_it;
            accepting = false;

            for (auto old_node : old_state) {
                auto chr_edges_it = old_node->edges.find(chr);

                if (chr_edges_it != old_node->edges.cend()) {
                    for (auto new_node : chr_edges_it->second) {
                        if (!already_on[new_node]) {
                            already_on[new_node] = true;
                            new_state.push_back(new_node);

                            if (new_node->epsilon_edge) {
                                accepting = true;
                            }
                        }
                    }
                }
            }

            for (auto node : new_state) {
                already_on[node] = false;
            }

            if (match_type == MatchType::RMATCH || match_type == MatchType::AMATCH) {
                new_state.push_back(&start_node);
                accepting = accepting || start_node.epsilon_edge.has_value();
            }

            if (accepting && (
                match_type == MatchType::AMATCH ||
                match_type == MatchType::LMATCH ||
                text_it == text.cend() && (
                    match_type == MatchType::FMATCH ||
                    match_type == MatchType::RMATCH
                ))) {
                return true;
            }

            if (text_it == text.cend()) {
                return false;
            }

            old_state.clear();
            std::swap(old_state, new_state);
        }

        return false;
    }

    Regex::NFA::MatchResult Regex::NFA::submatches(
        std::string const &text, MatchType match_type, AST::Group *group
    ) const {
        struct Pawn {
            std::set<std::size_t> entered;
            Matches matches;
        };

        std::unordered_map<Node const *, Pawn> old_pawns, new_pawns;
        bool accepting = start_node.epsilon_edge.has_value();
        auto text_it = text.cbegin();
        std::size_t pos = 0;
        new_pawns[&start_node]; // implicit emplace with Pawn()

        while (!new_pawns.empty()) {
            if (accepting && (
                match_type == MatchType::AMATCH ||
                match_type == MatchType::LMATCH ||
                text_it == text.cend() && (
                    match_type == MatchType::FMATCH ||
                    match_type == MatchType::RMATCH
                ))) {
                Matches result;
                bool can_match_without_group = false;

                for (auto &[node, pawn] : new_pawns) {
                    if (node->epsilon_edge) {
                        auto tokens_for_group_it = node->epsilon_edge->find(group);

                        if (tokens_for_group_it != node->epsilon_edge->cend()) {
                            for (auto token : tokens_for_group_it->second) {
                                switch (token) {
                                    case GroupToken::ENTER: {
                                        pawn.entered.insert(pos);
                                        break;
                                    }
                                    case GroupToken::LEAVE: {
                                        for (auto start : pawn.entered) {
                                            pawn.matches.emplace(start, pos);
                                        }

                                        pawn.entered.clear();
                                        break;
                                    }
                                }
                            }

                            pawn.entered.clear();
                            result.merge(pawn.matches);
                        } else {
                            // there is a way to match without going through a group,
                            // same as substituting \# for a group in regex
                            can_match_without_group = true;
                        }
                    } else {
                        // node is not a lastpos/start node
                        pawn.entered.clear();
                        pawn.matches.clear();
                    }
                }

                return result;
            }

            if (text_it == text.cend()) {
                return {};
            }

            std::swap(old_pawns, new_pawns);
            char chr = *text_it;

            for (auto &[old_node, old_pawn] : old_pawns) {
                auto nbhs_for_chr_it = old_node->edges.find(chr);

                if (nbhs_for_chr_it == old_node->edges.cend()) {
                    // pawn gets destroyed, because it cannot progress any further
                    old_pawn.entered.clear();
                    old_pawn.matches.clear();
                    continue;
                }

                for (auto new_node : nbhs_for_chr_it->second) {
                    Pawn tmp_pawn(old_pawn);
                    auto &groups = old_node->node_to_groups.find(new_node)->second;
                    // stuff below could possibly be extracted
                    auto tokens_for_group_it = groups.find(group);

                    if (tokens_for_group_it != groups.cend()) {
                        for (auto token : tokens_for_group_it->second) {
                            switch (token) {
                                case GroupToken::ENTER: {
                                    tmp_pawn.entered.emplace(pos);
                                    break;
                                }
                                case GroupToken::LEAVE: {
                                    for (auto start : tmp_pawn.entered) {
                                        tmp_pawn.matches.emplace(start, pos);
                                    }

                                    tmp_pawn.entered.clear();
                                    break;
                                }
                            }
                        }
                    }
                    // stuff above could possibly be extracted

                    auto &new_pawn = new_pawns[new_node];
                    new_pawn.entered.merge(tmp_pawn.entered);
                    new_pawn.matches.merge(tmp_pawn.matches);
                }
            }

            if (match_type == MatchType::RMATCH || match_type == MatchType::AMATCH) {
                new_pawns[&start_node];
                accepting = accepting || start_node.epsilon_edge.has_value();
            }

            old_pawns.clear();
            ++text_it;
            pos += 1;
        }

        return {};
    }
}