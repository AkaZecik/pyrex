#include "new_regex.h"

namespace pyrex {
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

            if (accepting && (match_type == MatchType::AMATCH || match_type == MatchType::LMATCH ||
                              match_type == MatchType::FMATCH && text_it == text.cend())) {
            }

            if (match_type == MatchType::LMATCH || match_type == MatchType::AMATCH) {
                new_state.push_back(&start_node);
            }

            std::swap(old_state, new_state);
        }

        return false;
    }



Regex::NFA::MatchResult Regex::NFA::traverse(std::string const &text, Group *group) const {
    auto text_it = text.cbegin();
    auto text_end = text.cend();

    if (text_it == text_end) {
        if (start_node.epsilon_edge) {
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
                if (node->epsilon_edge) {
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

}